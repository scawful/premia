#ifndef Socket_hpp
#define Socket_hpp

#include <SDL2/SDL.h>
#include <curl/curl.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/ssl.h>

#include <boost/asio.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/tokenizer.hpp>

#include "Metatypes.hpp"


namespace premia::tda {
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
  explicit Socket(Executor executor, ssl::context& ctx, CRLogger logger,
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
}  // namespace premia::tda

#endif