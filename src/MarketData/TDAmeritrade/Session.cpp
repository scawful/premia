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
        SDL_Log( "%s: %s", ec.message().c_str(), what );
    }

    void 
    Session::run( char const* host, char const* port, char const* text )
    {
        _host = host;
        _text = text;

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
        _ws.async_handshake(_host, "/",
            beast::bind_front_handler(
                &Session::on_handshake,
                shared_from_this()));
    }

    void
    Session::on_handshake(beast::error_code ec)
    {
        if(ec)
            return fail(ec, "handshake");

        // Send the message
        _ws.async_write(
            net::buffer(_text),
            beast::bind_front_handler(
                &Session::on_write,
                shared_from_this()));
    }

    void
    Session::on_write( beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "write");

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

        // If we get here then the connection is closed gracefully

        // The make_printable() function helps print a ConstBufferSequence
        std::cout << beast::make_printable(_buffer.data()) << std::endl;
    }


}