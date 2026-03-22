#include "http/server.h"

#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/json.hpp>

#include "http/json.h"
#include "premia/core/application/composition_root.hpp"
#include "premia/core/application/scaffold_application_service.hpp"

namespace premia::api::http {

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;
using tcp = asio::ip::tcp;

namespace {

auto MakeJsonResponse(http::status status, const std::string& body)
    -> http::response<http::string_body> {
  http::response<http::string_body> response{status, 11};
  response.set(http::field::server, "premia_api");
  response.set(http::field::content_type, "application/json");
  response.keep_alive(false);
  response.body() = body;
  response.prepare_payload();
  return response;
}

auto MakeTextResponse(http::status status, const std::string& content_type,
                      const std::string& body)
    -> http::response<http::string_body> {
  http::response<http::string_body> response{status, 11};
  response.set(http::field::server, "premia_api");
  response.set(http::field::content_type, content_type);
  response.keep_alive(false);
  response.body() = body;
  response.prepare_payload();
  return response;
}

auto StartsWith(const std::string& value, const std::string& prefix) -> bool {
  return value.rfind(prefix, 0) == 0;
}

auto SplitTarget(const std::string& raw_target)
    -> std::pair<std::string, std::string> {
  const auto query_offset = raw_target.find('?');
  if (query_offset == std::string::npos) {
    return {raw_target, ""};
  }
  return {raw_target.substr(0, query_offset),
          raw_target.substr(query_offset + 1)};
}

auto ParseQuery(const std::string& query) -> std::map<std::string, std::string> {
  std::map<std::string, std::string> values;
  std::size_t start = 0;
  while (start < query.size()) {
    const auto separator = query.find('&', start);
    const auto token = query.substr(
        start, separator == std::string::npos ? std::string::npos : separator - start);
    const auto equals = token.find('=');
    if (equals == std::string::npos) {
      values[token] = "";
    } else {
      values[token.substr(0, equals)] = token.substr(equals + 1);
    }
    if (separator == std::string::npos) {
      break;
    }
    start = separator + 1;
  }
  return values;
}

auto SplitPathSegments(const std::string& path) -> std::vector<std::string> {
  std::vector<std::string> segments;
  std::size_t start = 0;
  while (start < path.size()) {
    while (start < path.size() && path[start] == '/') {
      ++start;
    }
    if (start >= path.size()) {
      break;
    }
    const auto end = path.find('/', start);
    segments.push_back(path.substr(start, end == std::string::npos ? std::string::npos
                                                                   : end - start));
    if (end == std::string::npos) {
      break;
    }
    start = end + 1;
  }
  return segments;
}

auto ReadOpenApiOutline() -> std::string {
  std::ifstream file("contracts/openapi/premia-v1.yaml");
  if (!file.good()) {
    return "openapi outline not found\n";
  }
  return std::string{std::istreambuf_iterator<char>(file),
                     std::istreambuf_iterator<char>()};
}

auto ParseJsonObject(const std::string& body) -> json::object {
  if (body.empty()) {
    return {};
  }
  auto parsed = json::parse(body);
  if (!parsed.is_object()) {
    throw std::runtime_error("request body must be a JSON object");
  }
  return parsed.as_object();
}

auto GetOptionalString(const json::object& object, const std::string& key)
    -> std::string {
  const auto it = object.find(key);
  if (it == object.end()) {
    return "";
  }
  if (!it->value().is_string()) {
    throw std::runtime_error("field '" + key + "' must be a string");
  }
  return std::string(it->value().as_string().c_str());
}

auto GetRequiredString(const json::object& object, const std::string& key)
    -> std::string {
  auto value = GetOptionalString(object, key);
  if (value.empty()) {
    throw std::runtime_error("missing required field '" + key + "'");
  }
  return value;
}

auto GetOptionalBool(const json::object& object, const std::string& key,
                     bool fallback = false) -> bool {
  const auto it = object.find(key);
  if (it == object.end()) {
    return fallback;
  }
  if (!it->value().is_bool()) {
    throw std::runtime_error("field '" + key + "' must be a boolean");
  }
  return it->value().as_bool();
}

auto HandleGet(const std::string& raw_target)
    -> http::response<http::string_body> {
  const auto [path, query] = SplitTarget(raw_target);
  auto& service = core::application::CompositionRoot::Instance().AppService();

  if (path == "/health") {
    return MakeJsonResponse(http::status::ok, SerializeHealthResponse());
  }
  if (path == "/openapi") {
    return MakeTextResponse(http::status::ok, "application/yaml",
                            ReadOpenApiOutline());
  }
  if (path == "/v1/bootstrap") {
    return MakeJsonResponse(
        http::status::ok,
        SerializeBootstrapResponse(service.GetBootstrapData()));
  }
  if (path == "/v1/screens/home") {
    return MakeJsonResponse(
        http::status::ok,
        SerializeHomeScreenResponse(service.GetHomeScreenData()));
  }
  if (path == "/v1/screens/account") {
    return MakeJsonResponse(
        http::status::ok,
        SerializeAccountScreenResponse(
            core::application::CompositionRoot::Instance()
                .AccountDetails()
                .GetAccountDetail()));
  }
  if (StartsWith(path, "/v1/screens/options/")) {
    const auto symbol = path.substr(std::string("/v1/screens/options/").size());
    const auto params = ParseQuery(query);
    const auto strike_count = params.count("strikeCount") ? params.at("strikeCount") : "8";
    const auto strategy = params.count("strategy") ? params.at("strategy") : "SINGLE";
    const auto range = params.count("range") ? params.at("range") : "ALL";
    const auto exp_month = params.count("expMonth") ? params.at("expMonth") : "ALL";
    const auto option_type = params.count("optionType") ? params.at("optionType") : "ALL";
    return MakeJsonResponse(
        http::status::ok,
        SerializeOptionChainResponse(
            core::application::CompositionRoot::Instance()
                .Options()
                .GetOptionChainSnapshot(symbol, strike_count, strategy, range,
                                        exp_month, option_type)));
  }
  if (path == "/v1/watchlists") {
    return MakeJsonResponse(
        http::status::ok,
        SerializeWatchlistsResponse(service.ListWatchlists()));
  }
  if (StartsWith(path, "/v1/screens/watchlists/")) {
    const auto watchlist_id = path.substr(std::string("/v1/screens/watchlists/").size());
    return MakeJsonResponse(
        http::status::ok,
        SerializeWatchlistScreenResponse(service.GetWatchlistScreen(watchlist_id)));
  }
  if (StartsWith(path, "/v1/screens/quotes/")) {
    const auto symbol = path.substr(std::string("/v1/screens/quotes/").size());
    return MakeJsonResponse(http::status::ok,
                            SerializeQuoteScreenResponse(service.GetQuoteDetail(symbol)));
  }
  if (StartsWith(path, "/v1/screens/charts/")) {
    const auto symbol = path.substr(std::string("/v1/screens/charts/").size());
    const auto params = ParseQuery(query);
    const auto range = params.count("range") ? params.at("range") : "1M";
    const auto interval = params.count("interval") ? params.at("interval") : "1D";
    const auto extended_hours = params.count("extendedHours") != 0 &&
                                params.at("extendedHours") == "true";
    return MakeJsonResponse(
        http::status::ok,
        SerializeChartScreenResponse(
            service.GetChartScreen(symbol, range, interval, extended_hours)));
  }
  if (path == "/v1/stream/events") {
    return MakeTextResponse(
        http::status::ok, "text/event-stream",
        "event: ready\ndata: {\"message\":\"stream scaffold online\"}\n\n");
  }

  return MakeJsonResponse(
      http::status::not_found,
      SerializeErrorResponse("NOT_FOUND", "Route not found", "none"));
}

auto HandleMutation(const http::request<http::string_body>& request)
    -> http::response<http::string_body> {
  const std::string target{request.target().data(), request.target().size()};
  const auto [path, query] = SplitTarget(target);
  (void)query;

  auto& service = core::application::CompositionRoot::Instance().AppService();

  try {
    const auto payload = request.method() == http::verb::delete_
                             ? json::object{}
                             : ParseJsonObject(request.body());

    if (path == "/v1/connections/schwab/oauth/start") {
      core::application::SchwabOAuthStartRequest workflow_request;
      workflow_request.redirect_uri = GetOptionalString(payload, "redirectUri");
      workflow_request.client_platform = GetOptionalString(payload, "clientPlatform");
      return MakeJsonResponse(
          http::status::ok,
          SerializeSchwabOAuthStartResponse(service.StartSchwabOAuth(workflow_request)));
    }

    if (path == "/v1/connections/schwab/oauth/complete") {
      core::application::SchwabOAuthCompleteRequest workflow_request;
      workflow_request.callback = GetRequiredString(payload, "callback");
      workflow_request.state = GetOptionalString(payload, "state");
      return MakeJsonResponse(
          http::status::ok,
          SerializeConnectionSummaryResponse(
              service.CompleteSchwabOAuth(workflow_request)));
    }

    if (path == "/v1/connections/plaid/link-token") {
      core::application::PlaidLinkTokenRequest workflow_request;
      workflow_request.user_id = GetOptionalString(payload, "userId");
      if (workflow_request.user_id.empty()) {
        workflow_request.user_id = "premia-user";
      }
      workflow_request.redirect_uri = GetOptionalString(payload, "redirectUri");
      return MakeJsonResponse(
          http::status::ok,
          SerializePlaidLinkTokenResponse(
              service.CreatePlaidLinkToken(workflow_request)));
    }

    if (path == "/v1/connections/plaid/link-complete") {
      core::application::PlaidLinkCompleteRequest workflow_request;
      workflow_request.public_token = GetRequiredString(payload, "publicToken");
      workflow_request.institution_id = GetOptionalString(payload, "institutionId");
      return MakeJsonResponse(
          http::status::ok,
          SerializeConnectionSummaryResponse(service.CompletePlaidLink(workflow_request)));
    }

    if (path == "/v1/orders/preview") {
      core::application::OrderIntentRequest order_request;
      order_request.account_id = GetOptionalString(payload, "accountId");
      order_request.symbol = GetRequiredString(payload, "symbol");
      order_request.asset_type = GetRequiredString(payload, "assetType");
      order_request.instruction = GetRequiredString(payload, "instruction");
      order_request.quantity = GetRequiredString(payload, "quantity");
      order_request.order_type = GetRequiredString(payload, "orderType");
      order_request.limit_price = GetOptionalString(payload, "limitPrice");
      order_request.duration = GetOptionalString(payload, "duration");
      order_request.session = GetOptionalString(payload, "session");
      order_request.confirm_live = GetOptionalBool(payload, "confirmLive", false);
      return MakeJsonResponse(
          http::status::ok,
          SerializeOrderPreviewResponse(
              core::application::CompositionRoot::Instance().Orders().PreviewOrder(
                  order_request)));
    }

    if (path == "/v1/orders/submit") {
      core::application::OrderIntentRequest order_request;
      order_request.account_id = GetOptionalString(payload, "accountId");
      order_request.symbol = GetRequiredString(payload, "symbol");
      order_request.asset_type = GetRequiredString(payload, "assetType");
      order_request.instruction = GetRequiredString(payload, "instruction");
      order_request.quantity = GetRequiredString(payload, "quantity");
      order_request.order_type = GetRequiredString(payload, "orderType");
      order_request.limit_price = GetOptionalString(payload, "limitPrice");
      order_request.duration = GetOptionalString(payload, "duration");
      order_request.session = GetOptionalString(payload, "session");
      order_request.confirm_live = GetOptionalBool(payload, "confirmLive", false);
      return MakeJsonResponse(
          http::status::ok,
          SerializeOrderSubmissionResponse(
              core::application::CompositionRoot::Instance().Orders().SubmitOrder(
                  order_request)));
    }

    if (path == "/v1/watchlists" && request.method() == http::verb::post) {
      return MakeJsonResponse(
          http::status::created,
          SerializeWatchlistResponse(
              service.CreateWatchlist(GetRequiredString(payload, "name"))));
    }

    const auto segments = SplitPathSegments(path);
    if (segments.size() == 3 && segments[0] == "v1" && segments[1] == "watchlists" &&
        request.method() == http::verb::patch) {
      return MakeJsonResponse(
          http::status::ok,
          SerializeWatchlistResponse(service.RenameWatchlist(
              segments[2], GetRequiredString(payload, "name"))));
    }

    if (segments.size() == 4 && segments[0] == "v1" && segments[1] == "watchlists" &&
        segments[3] == "symbols" && request.method() == http::verb::post) {
      return MakeJsonResponse(
          http::status::ok,
          SerializeWatchlistResponse(service.AddWatchlistSymbol(
              segments[2], GetRequiredString(payload, "symbol"))));
    }

    if (segments.size() == 5 && segments[0] == "v1" && segments[1] == "watchlists" &&
        segments[3] == "symbols" && request.method() == http::verb::delete_) {
      return MakeJsonResponse(
          http::status::ok,
          SerializeWatchlistResponse(
              service.RemoveWatchlistSymbol(segments[2], segments[4])));
    }
  } catch (const std::exception& e) {
    return MakeJsonResponse(
        http::status::bad_request,
        SerializeErrorResponse("INVALID_REQUEST", e.what(), "retry"));
  }

  return MakeJsonResponse(
      http::status::not_implemented,
      SerializeErrorResponse("NOT_IMPLEMENTED",
                             "Mutation route is recognized but not implemented yet",
                             "none"));
}

auto HandleRequest(const http::request<http::string_body>& request)
    -> http::response<http::string_body> {
  const std::string target{request.target().data(), request.target().size()};
  switch (request.method()) {
    case http::verb::get:
      return HandleGet(target);
    case http::verb::post:
    case http::verb::patch:
    case http::verb::delete_:
      return HandleMutation(request);
    default:
      return MakeJsonResponse(
          http::status::method_not_allowed,
          SerializeErrorResponse("METHOD_NOT_ALLOWED", "Method not allowed",
                                 "none"));
  }
}

}  // namespace

auto RunHttpServer(const std::string& host, unsigned short port) -> void {
  asio::io_context ioc{1};
  const auto address = asio::ip::make_address(host);
  tcp::acceptor acceptor{ioc, {address, port}};

  std::cout << "Premia API listening on http://" << host << ":" << port
            << std::endl;
  std::cout << "Available demo routes: /health, /v1/bootstrap, /v1/screens/home"
            << std::endl;

  for (;;) {
    tcp::socket socket{ioc};
    acceptor.accept(socket);

    beast::flat_buffer buffer;
    http::request<http::string_body> request;
    http::read(socket, buffer, request);
    auto response = HandleRequest(request);
    http::write(socket, response);

    beast::error_code ec;
    socket.shutdown(tcp::socket::shutdown_send, ec);
  }
}

}  // namespace premia::api::http
