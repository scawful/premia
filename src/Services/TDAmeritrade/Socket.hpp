#ifndef Socket_hpp
#define Socket_hpp

#include "Metatypes.hpp"
#include "Library/Boost.hpp"
#include "Library/SDL.hpp"
#include "Library/Curl.hpp"

namespace tda 
{
    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace websocket = beast::websocket;
    namespace net = boost::asio;
    namespace ssl = boost::asio::ssl;
    namespace pt = boost::property_tree; 
    using tcp = boost::asio::ip::tcp;
    
    class Socket 
        : public std::enable_shared_from_this<Socket>
    {
    private:
        String _host;
        String _port;
        bool _logged_in;
        bool _subscribed;
        bool _notified;
        bool _interrupt;
        net::io_context _ioc;
        tcp::resolver _resolver;
        beast::multi_buffer _buffer;
        std::atomic<bool> _io_in_progress{false};
        websocket::stream<beast::ssl_stream<beast::tcp_stream>> _ws;

        std::size_t _queue_size;

        void fail( beast::error_code ec, char const* what ) const;

    public:
        template <typename Executor>
        explicit Socket(Executor executor, ssl::context & ctx)
            : _resolver(executor) ,_ws(executor, ctx) { }


        bool io_in_progress() const;

        // connect sequence 
        void open( char const* host, char const* port );
        void on_resolve(beast::error_code ec, tcp::resolver::results_type results);
        void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep);
        void on_ssl_handshake(beast::error_code ec);
        void on_handshake(beast::error_code ec);

        // read/write sequence 
        void write(CRString request);
        void on_write(beast::error_code ec, std::size_t bytes);
        void on_read(beast::error_code ec, std::size_t bytes);

        bool on_login(beast::error_code ec);
        void on_logout(beast::error_code ec) const;
        void on_notify(beast::error_code ec);
        void on_subscription(beast::error_code ec);

        void close();
        void on_close(beast::error_code ec);
    };
}

#endif