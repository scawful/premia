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
    Session::fail( beast::error_code ec, char const* what )
    {
        SDL_Log( "Session::fail( %s: %s )", ec.message().c_str(), what );
    }

    void 
    Session::run( char const* host, char const* port, char const* login_text, char const* request_text )
    {
        _host = host;
        _text = login_text;
        _request_text = request_text;

        SDL_Log("Session::run( host: %s ) ", _host.c_str() );
        SDL_Log("Session::run( text ): \n%s", _text.c_str() );

        _resolver.async_resolve( host, port, 
                                 beast::bind_front_handler( 
                                     &Session::on_resolve, 
                                     shared_from_this() ) );
    }

    void 
    Session::on_resolve( beast::error_code ec, tcp::resolver::results_type results )
    {
        if ( ec )
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

        // Set the request parameters generated from TDA User Principals
        // auto self = shared_from_this();
        // _ws.set_option(websocket::stream_base::decorator(
        //     [self]( websocket::request_type& req )
        //     {
        //         for ( auto& req_it: self->_request_tree )
        //         {
        //             req.set( req_it.first, req_it.second.get_value<std::string>() );
        //         }
        //     }));

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
            net::buffer(_text),
            beast::bind_front_handler(
                &Session::on_write,
                shared_from_this()));
    }

    void
    Session::on_write( beast::error_code ec, std::size_t bytes_transferred )
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "write");

        SDL_Log("Session::on-write");

        // Read a message into our buffer
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

        // attempt to login, if fail then close connection 
        if ( !on_login( ec ) )
        {
            // not logged in
        }
        else
        {
            _ws.async_write(
            net::buffer(_request_text),
            beast::bind_front_handler(
                &Session::on_write,
                shared_from_this()));
        }

        // Close the WebSocket connection
        _ws.async_close(websocket::close_code::normal,
            beast::bind_front_handler(
                &Session::on_close,
                shared_from_this()));
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

    bool 
    Session::on_login( beast::error_code ec )
    {
        pt::ptree login_response;
        std::stringstream buffer_stream;
        buffer_stream << beast::make_printable( _buffer.data() );

        pt::json_parser::read_json( buffer_stream, login_response );

        if ( login_response.get<std::string>("response.content.code") == "3" )
        {
            // login failed
            SDL_Log("Session::on_login( msg: %s )", login_response.get<std::string>("response.content.msg").c_str() );
            _logged_in = false;
        }
        else if ( login_response.get<std::string>("response.content.code") == "0" )
        {
            // login success
            SDL_Log("Session::on_login( msg: %s )", login_response.get<std::string>("response.content.msg").c_str() );
            _logged_in = true;
        }

        return _logged_in;
    }


}