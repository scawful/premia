#ifndef Session_hpp
#define Session_hpp

#include "Boost.hpp"
#include "SDL.hpp"
#include "Curl.hpp"

namespace tda 
{
    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace websocket = beast::websocket;
    namespace net = boost::asio;
    namespace ssl = boost::asio::ssl;
    namespace pt = boost::property_tree; 
    using tcp = boost::asio::ip::tcp;
    
    class Session 
        : public std::enable_shared_from_this<Session>
    {
    private:
        bool _logged_in;
        bool _subscribed;
        bool _notified;
        bool _interrupt;
        unsigned int _sub_count;
        net::io_context _ioc;
        tcp::resolver _resolver;
        beast::flat_buffer _buffer;
        websocket::stream<beast::ssl_stream <beast::tcp_stream>> _ws;
        std::vector<std::shared_ptr<std::string const>> _queue;
        std::vector<std::string> _response_stack;
        std::string _host;
        std::size_t _queue_size;

        void fail( beast::error_code ec, char const* what ) const;

    public:
        explicit Session( net::io_context & ioc, ssl::context & ctx, 
                          const std::vector<std::shared_ptr<std::string const>> & queue )
            : _resolver(net::make_strand(ioc))
            , _ws(net::make_strand(ioc), ctx)
            , _queue(queue)
        {
            _logged_in = false;
            _subscribed = false;
            _notified = false;
            _interrupt = false;
            _sub_count = 0;
        }

        void run( char const* host, char const* port );
        void on_resolve( beast::error_code ec, tcp::resolver::results_type results );
        void on_connect( beast::error_code ec, tcp::resolver::results_type::endpoint_type ep );
        void on_ssl_handshake( beast::error_code ec );
        void on_handshake( beast::error_code ec );
        void on_write( beast::error_code ec, std::size_t bytes_transferred );
        void on_read( beast::error_code ec, std::size_t bytes_transferred );
        void on_close( beast::error_code ec );

        void send_message( std::shared_ptr<std::string const> const& s );
        std::vector<std::string> receive_response() const;
        std::shared_ptr<std::vector<std::string>> receive_response_ptr();

        bool on_login( beast::error_code ec );
        void on_logout( beast::error_code ec ) const;
        void on_notify( beast::error_code ec );
        void on_subscription( beast::error_code ec );

        void interrupt();
        void clear_buffer();

        bool is_logged_in() const;
        bool is_subscribed() const;

    };
}

#endif