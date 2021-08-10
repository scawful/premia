#ifndef Session_hpp
#define Session_hpp

#include "../../core.hpp"

namespace tda 
{
    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace websocket = beast::websocket;
    namespace net = boost::asio;
    namespace ssl = boost::asio::ssl; 
    using tcp = boost::asio::ip::tcp;
    
    class Session : public std::enable_shared_from_this<Session>
    {
    private:
        net::io_context _ioc;
        tcp::resolver _resolver;
        beast::flat_buffer _buffer;
        websocket::stream<beast::ssl_stream <beast::tcp_stream>> _ws;
        std::string _host;
        std::string _text;

        void fail( beast::error_code ec, char const* what );

    public:
        explicit Session( net::io_context& ioc, ssl::context& ctx )
            : _resolver(net::make_strand(ioc))
            , _ws(net::make_strand(ioc), ctx)
        {
        }

        void run( char const* host, char const* port, char const* text );
        void on_resolve( beast::error_code ec, tcp::resolver::results_type results );
        void on_connect( beast::error_code ec, tcp::resolver::results_type::endpoint_type ep );
        void on_ssl_handshake( beast::error_code ec );
        void on_handshake( beast::error_code ec );
        void on_write( beast::error_code ec, std::size_t bytes_transferred );
        void on_read( beast::error_code ec, std::size_t bytes_transferred );
        void on_close(beast::error_code ec);

    };
}

#endif