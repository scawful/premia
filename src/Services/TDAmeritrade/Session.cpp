#include "Session.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl; 
using tcp = boost::asio::ip::tcp;

// Code is based on https://github.com/boostorg/beast/blob/develop/example/websocket/client/async-ssl/websocket_client_async_ssl.cpp
// Will improve/change it as I see fit in response to the needs of the TDAmeritrade WebSocket client. 
namespace tda
{
    void 
    Session::fail(beast::error_code ec, char const* what) const
    {
        if(ec == net::error::operation_aborted || ec == websocket::error::closed)
            return;
        
        SDL_Log("Session::fail( %s: %s )", ec.message().c_str(), what);
    }

    void 
    Session::run( char const* host, char const* port )
    {
        _host = host;
        _queue_size = _queue.size();

        SDL_Log("Session::run( host: %s ) ", _host.c_str() );

        _resolver.async_resolve( host, port, 
                                 beast::bind_front_handler( 
                                     &Session::on_resolve, 
                                     shared_from_this() ) );
    }

    void 
    Session::on_resolve( beast::error_code ec, tcp::resolver::results_type results )
    {
        if (ec)
            return fail(ec, "resolve");

        SDL_Log("Session::on_resolve");

        beast::get_lowest_layer(_ws).expires_after(std::chrono::seconds(30));

        beast::get_lowest_layer(_ws).async_connect(
            results,
            beast::bind_front_handler(
                    &Session::on_connect,
                    shared_from_this()
            )
        );
    }

    void 
    Session::on_connect( beast::error_code ec, tcp::resolver::results_type::endpoint_type ep )
    {
        if(ec)
            return fail(ec, "connect");

        SDL_Log("Session::on_connect");

        // Update the _host string. This will provide the value of the
        // Host HTTP header during the WebSocket handshake.
        _host += ':' + std::to_string(ep.port());

        // Set a timeout on the operation
        beast::get_lowest_layer(_ws).expires_after(std::chrono::seconds(30));

        // Set SNI Hostname (many hosts need this to handshake successfully)
        if(! SSL_set_tlsext_host_name(
                _ws.next_layer().native_handle(),
                _host.c_str()))
        {
            ec = beast::error_code(static_cast<int>(::ERR_get_error()),
                net::error::get_ssl_category());
            return fail(ec, "connect");
        }

        // Perform the SSL handshake
        _ws.next_layer().async_handshake(
            ssl::stream_base::client,
            beast::bind_front_handler(
                &Session::on_ssl_handshake,
                shared_from_this()));
    }

    void
    Session::on_ssl_handshake( beast::error_code ec )
    {
        if (ec)
            return fail(ec, "ssl_handshake");

        SDL_Log("Session::on_ssl_handshake");

        // Turn off the timeout on the tcp_stream, because
        // the websocket stream has its own timeout system.
        beast::get_lowest_layer(_ws).expires_never();

        // Set suggested timeout settings for the websocket
        _ws.set_option(
            websocket::stream_base::timeout::suggested(
                beast::role_type::client));

        // Set a decorator to change the User-Agent of the handshake
        _ws.set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req)
            {
                req.set(http::field::user_agent,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-client-async-ssl");
            }));

        // Perform the websocket handshake
        _ws.async_handshake(_host, "/ws",
            beast::bind_front_handler(
                &Session::on_handshake,
                shared_from_this()));

    }

    void
    Session::on_handshake(beast::error_code ec)
    {
        if(ec)
            return fail(ec, "handshake");

        SDL_Log("Session::on_handshake");

        // Send the message
        _ws.async_write(
            net::buffer( *_queue.front() ),
            beast::bind_front_handler(
                &Session::on_write,
                shared_from_this()));
    }

    void
    Session::on_write(beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "write");

        SDL_Log("Session::on_write");

        if (_logged_in) {
            // clear request from the queue
            _queue.erase( _queue.begin() );

            if (!_queue.empty()) {
                SDL_Log("Session::on_write( Queue Not Empty )");
            }
            else
            {
                if (_interrupt == false) {
                    SDL_Log( "Uninterrupted stream");
                }
                else {
                    // logout
                    SDL_Log("Session::on_write( Logging out ) ");
                    _logged_in = false;
                }
            }
        }

        _ws.async_read(
            _buffer,
            beast::bind_front_handler(
                &Session::on_read,
                shared_from_this()));

    }

    void
    Session::on_read(
        beast::error_code ec,
        std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "read");

        SDL_Log("Session::on_read");
        std::string response(net::buffer_cast<const char*>( _buffer.data()), _buffer.size()); 
        SDL_Log("Server Response: %s", response.c_str() );
        _response_stack.push_back( response );

        // check login first
        // originally checked the boolean but this had a weird logical byproduct on subsequent streams
        // so now we check the queue size, first request is always login 
        if ( _queue_size == _queue.size() ) {
            on_login( ec );
            _buffer.consume( _buffer.size() );
        }
        else if (!_notified) {
            on_notify( ec );
        }
        else if (!_subscribed) {
            on_subscription( ec );
        }

        if (_logged_in) {
            if (_notified && !_subscribed) {
                SDL_Log("Notified, but not subscribed!");
                _ws.async_read(
                    _buffer,
                    beast::bind_front_handler(
                        &Session::on_read,
                        shared_from_this()));
            }
            else if ( _subscribed && !_interrupt ) {
                SDL_Log("Subscribed but not finished");
                _sub_count++;
                _ws.async_read(
                    _buffer,
                    beast::bind_front_handler(
                        &Session::on_read,
                        shared_from_this()));
            }
            else {
                _ws.async_write(
                    net::buffer( *_queue.front() ),
                    beast::bind_front_handler(
                        &Session::on_write,
                        shared_from_this()));
            }

        }
        else {
            // Close the WebSocket connection
            _ws.async_close(websocket::close_code::normal,
                beast::bind_front_handler(
                    &Session::on_close,
                    shared_from_this()));
        }

    }

    void
    Session::on_close(beast::error_code ec)
    {
        if (ec)
            return fail(ec, "close");

        SDL_Log("Session::on_close");

        // If we get here then the connection is closed gracefully

        // The make_printable() function helps print a ConstBufferSequence
        std::cout << beast::make_printable(_buffer.data()) << std::endl;
    }

    // sends a message to the queue from the main threads
    void 
    Session::send_message(const std::shared_ptr<std::string const> & s)
    {
        SDL_Log("Session::send_message");

        _queue.push_back( s );

        if (_queue.size() > 1) {
            SDL_Log("Session::send_message( Already Writing )");
            return;
        }

        _ws.async_write(
            net::buffer( *_queue.front() ),
                beast::bind_front_handler(
                    &Session::on_write,
                    shared_from_this()));
    }

    // returns the cumulative vector of responses from the server
    std::vector<std::string> 
    Session::receive_response() const
    {
        return _response_stack;
    }

    std::shared_ptr<std::vector<std::string>> 
    Session::receive_response_ptr()
    {
        return std::make_shared<std::vector<std::string>>(_response_stack);
    }

    bool 
    Session::on_login(beast::error_code ec)
    {
        std::string response_code;
        std::string s(net::buffer_cast<const char*>(_buffer.data()), _buffer.size());
        SDL_Log("Login Response Stream: \n%s", s.c_str() );

        std::size_t found = s.find("code");
        response_code = s[found + 6];
        SDL_Log("Code: %s", response_code.c_str() );

        found = s.find("msg");
        std::string response_msg = s.substr( found, 4 );

        if ( response_code == "3" )
        {
            // login failed
            _logged_in = false;
        }
        else if ( response_code == "0" )
        {
            // login success
            _logged_in = true;
        }

        // clear request from stream 
        _queue.erase( _queue.begin() );

        return _logged_in;
    }

    void 
    Session::on_logout(beast::error_code ec) const
    {
        // need a callback for this 
    }

    void
    Session::on_notify(beast::error_code ec)
    {
        SDL_Log("Session::on_notify");
        std::string s(net::buffer_cast<const char*>(_buffer.data()), _buffer.size());
        std::size_t found = s.find("notify");

        if (found != std::string::npos) {
            _notified = true;
            _buffer.consume( _buffer.size() );
        }
    }

    void
    Session::on_subscription(beast::error_code ec)
    {
        SDL_Log("Session::on_subscription");
        std::string sub_code;
        std::string s(net::buffer_cast<const char*>(_buffer.data()), _buffer.size());
        std::size_t found = s.find("code");

        if (found != std::string::npos) {
            sub_code = s[found + 6];
            SDL_Log("SUBS code %s", sub_code.c_str() );
            if (sub_code == "0") {
                _subscribed = true;
            }
            _buffer.consume(_buffer.size());
        }
    }

    void 
    Session::interrupt()
    {
        _interrupt = true;
    }

    void 
    Session::clear_buffer()
    {
        _buffer.clear();
    }

    bool
    Session::is_logged_in() const
    {
        return _logged_in;
    }

    bool
    Session::is_subscribed() const
    {
        return _subscribed;
    }

}