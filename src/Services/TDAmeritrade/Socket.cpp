#include "Socket.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

// Code is based on
// https://github.com/boostorg/beast/blob/develop/example/websocket/client/async-ssl/websocket_client_async_ssl.cpp
// Will improve/change it as I see fit in response to the needs of the
// TDAmeritrade WebSocket client.
namespace Premia {
namespace tda {
void Socket::fail(beast::error_code ec, char const* what) const {
  if (ec == net::error::operation_aborted || ec == websocket::error::closed)
    return;

  _logger("WebSocket::fail(error: " + ec.message() + ", what: " + what + ")");
}

bool Socket::io_in_progress() const { return _io_in_progress; }

void Socket::open(char const* host, char const* port) {
  _host = host;
  _port = port;

  _logger("WebSocket::open(host: " + _host + ", port: " + _port + ")");

  _resolver.async_resolve(
      host, port,
      beast::bind_front_handler(&Socket::on_resolve, shared_from_this()));
}

void Socket::on_resolve(beast::error_code ec,
                        tcp::resolver::results_type results) {
  if (ec) return fail(ec, "resolve");

  _logger("WebSocket::on_resolve");

  beast::get_lowest_layer(_ws).expires_after(std::chrono::seconds(30));

  beast::get_lowest_layer(_ws).async_connect(
      results,
      beast::bind_front_handler(&Socket::on_connect, shared_from_this()));
}

void Socket::on_connect(beast::error_code ec,
                        tcp::resolver::results_type::endpoint_type ep) {
  if (ec) return fail(ec, "connect");

  _logger("WebSocket::on_connect");

  // Update the _host string. This will provide the value of the
  // Host HTTP header during the WebSocket handshake.
  _host += ':' + std::to_string(ep.port());

  // Set a timeout on the operation
  beast::get_lowest_layer(_ws).expires_after(std::chrono::seconds(30));

  // Set SNI Hostname (many hosts need this to handshake successfully)
  if (!SSL_set_tlsext_host_name(_ws.next_layer().native_handle(),
                                _host.c_str())) {
    ec = beast::error_code(static_cast<int>(::ERR_get_error()),
                           net::error::get_ssl_category());
    return fail(ec, "connect");
  }

  // Perform the SSL handshake
  _ws.next_layer().async_handshake(
      ssl::stream_base::client,
      beast::bind_front_handler(&Socket::on_ssl_handshake, shared_from_this()));
}

void Socket::on_ssl_handshake(beast::error_code ec) {
  if (ec) return fail(ec, "ssl_handshake");

  _logger("WebSocket::on_ssl_handshake");

  // Turn off the timeout on the tcp_stream, because
  // the websocket stream has its own timeout system.
  beast::get_lowest_layer(_ws).expires_never();

  // Set suggested timeout settings for the websocket
  _ws.set_option(
      websocket::stream_base::timeout::suggested(beast::role_type::client));

  // Set a decorator to change the User-Agent of the handshake
  _ws.set_option(
      websocket::stream_base::decorator([](websocket::request_type& req) {
        req.set(http::field::user_agent, String(BOOST_BEAST_VERSION_STRING) +
                                             " websocket-client-async-ssl");
      }));

  // Perform the websocket handshake
  _ws.async_handshake(
      _host, "/ws",
      beast::bind_front_handler(&Socket::on_handshake, shared_from_this()));
}

void Socket::on_handshake(beast::error_code ec) {
  if (ec) return fail(ec, "handshake");

  _logger("WebSocket::on_handshake: success!");

  // Send the message
  write(_requests);
}

/**
 * @brief Write/Read Sequence
 *
 * @param s
 */
void Socket::write(String request) {
  _logger("WebSocket::write");
  _io_in_progress = true;

  _ws.async_write(
      net::buffer(request),
      beast::bind_front_handler(&Socket::on_write, shared_from_this()));
}

void Socket::on_write(beast::error_code ec, std::size_t bytes) {
  boost::ignore_unused(bytes);

  if (ec) return fail(ec, "write");

  _logger("WebSocket::on_write");

  // read a message into our buffer
  _ws.async_read(
      _buffer, beast::bind_front_handler(&Socket::on_read, shared_from_this()));
}

void Socket::on_read(beast::error_code ec, std::size_t bytes) {
  boost::ignore_unused(bytes);

  if (ec) return fail(ec, "read");

  // handle the server response here
  _logger("WebSocket::on_read: ");
  _logger(beast::buffers_to_string(_buffer.data()));

  // clear the buffer
  _buffer.consume(_buffer.size());

  _ws.async_read(
      _buffer, beast::bind_front_handler(&Socket::on_read, shared_from_this()));
}

void Socket::close() {
  _io_in_progress = true;
  _logger("WebSocket::close");
  _ws.async_close(
      websocket::close_code::normal,
      beast::bind_front_handler(&Socket::on_close, shared_from_this()));
}

/**
 * @brief Closing the WebSocket Stream
 *
 * @param ec
 */
void Socket::on_close(beast::error_code ec) {
  _io_in_progress = false;

  if (ec) return fail(ec, "close");

  _logger("WebSocket::on_close: ");
  _logger(beast::buffers_to_string(_buffer.data()));
}
}  // namespace tda
}  // namespace Premia