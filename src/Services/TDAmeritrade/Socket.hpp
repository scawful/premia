#ifndef Socket_hpp
#define Socket_hpp

#include "Library/Boost.hpp"
#include "Library/Curl.hpp"
#include "Library/SDL.hpp"
#include "Metatypes.hpp"

namespace Premia {
namespace tda {
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

class Socket : public std::enable_shared_from_this<Socket> {
 private:
  String _host;
  String _port;
  String _requests;
  Logger _logger;
  net::io_context _ioc;
  tcp::resolver _resolver;
  beast::flat_buffer _buffer;
  std::atomic<bool> _io_in_progress{false};
  websocket::stream<beast::ssl_stream<beast::tcp_stream>> _ws;

  void fail(beast::error_code ec, char const* what) const;

 public:
  template <typename Executor>
  explicit Socket(Executor executor, ssl::context& ctx, Logger logger,
                  CRString requests)
      : _resolver(executor),
        _ws(executor, ctx),
        _logger(logger),
        _requests(requests) {}

  bool io_in_progress() const;

  // connect sequence
  void open(char const* host, char const* port);
  void on_resolve(beast::error_code ec, tcp::resolver::results_type results);
  void on_connect(beast::error_code ec,
                  tcp::resolver::results_type::endpoint_type ep);
  void on_ssl_handshake(beast::error_code ec);
  void on_handshake(beast::error_code ec);

  // read/write sequence
  void write(String request);
  void on_write(beast::error_code ec, std::size_t bytes);
  void on_read(beast::error_code ec, std::size_t bytes);

  // exit sequence
  void close();
  void on_close(beast::error_code ec);
};
}  // namespace tda
}  // namespace Premia

#endif