#ifndef Client_hpp
#define Client_hpp

#include <google/protobuf/message.h>
#include <grpc/support/log.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

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
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <vector>

#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "data/Order.hpp"
#include "data/UserPrincipals.hpp"
#include "handler/tdameritrade_service.h"
#include "parser.h"
#include "socket.h"
#include "src/service/TDAmeritrade/proto/tdameritrade.grpc.pb.h"
#include "src/service/TDAmeritrade/proto/tdameritrade.pb.h"

namespace premia {
namespace tda {

namespace json = boost::property_tree;
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;

using tcp = boost::asio::ip::tcp;

inline json::ptree bind_requests(std::vector<json::ptree> requests_array) {
  json::ptree requests;
  json::ptree children;
  for (const auto &each_request : requests_array) {
    children.push_back(std::make_pair("", each_request));
  }
  requests.add_child("requests", children);
  return requests;
}

static const std::string EnumAPIValues[]{"day", "month", "year", "ytd"};
static const std::string EnumAPIFreq[]{"minute", "daily", "weekly", "monthly"};
static const std::string EnumAPIPeriod[]{"1", "2",  "3",  "4", "5",
                                         "6", "10", "15", "20"};
static const std::string EnumAPIFreqAmt[]{"1", "5", "10", "15", "30"};
static const std::string EnumAPIServiceName[]{
    "NONE",
    "ADMIN",
    "ACTIVES_NASDAQ",
    "ACTIVES_NYSE",
    "ACTIVES_OTCBB",
    "ACTIVES_OPTIONS",
    "CHART_EQUITY",
    "CHART_FOREX",
    "CHART_FUTURES",
    "CHART_OPTIONS",
    "QUOTE",
    "LEVELONE_FUTURES",
    "LEVELONE_FOREX",
    "LEVELONE_FUTURES_OPTIONS",
    "OPTION",
    "NEWS_HEADLINE",
    "TIMESALE_EQUITY",
    "TIMESALE_FUTURES",
    "TIMESALE_FOREX",
    "TIMESALE_OPTIONS",
    "ACCT_ACTIVITY",
    "CHART_HISTORY_FUTURES",
    "FOREX_BOOK",
    "FUTURES_BOOK",
    "LISTED_BOOK",
    "NASDAQ_BOOK",
    "OPTIONS_BOOK",
    "FUTURES_OPTIONS_BOOK",
    "NEWS_STORY",
    "NEWS_HEADLINE_LIST",
    "UNKNOWN",
};

using CURLHeader = struct curl_slist *;

class Client {
 public:
  Client();
  ~Client();

  absl::Status GetAccount(const absl::string_view account_id);

  void api_login();

  // WebSocket Controls
  void start_session(const std::string &ticker);
  void send_logout_request();
  void fetch_access_token();

  // Accounts
  std::string get_account(const std::string &account_id);
  std::string get_all_accounts();
  std::vector<std::string> get_all_account_ids();

  // Quotes
  std::string get_quote(const std::string &symbol) const;

  // Watchlists
  std::string get_watchlist_by_account(const std::string &account_id) const;

  // Price History
  std::string get_price_history(const std::string &symbol, PeriodType ptype,
                                int period_amt, FrequencyType ftype,
                                int freq_amt, bool ext) const;

  // Option Chain
  std::string get_option_chain(const std::string &ticker,
                               const std::string &contractType,
                               const std::string &strikeCount,
                               bool includeQuotes, const std::string &strategy,
                               const std::string &range,
                               const std::string &expMonth,
                               const std::string &optionType) const;

  // Orders
  std::string get_order(const std::string &account_id,
                        const std::string &order_id) const;
  std::string get_orders_by_query(const std::string &account_id, int maxResults,
                                  double fromEnteredTime, double toEnteredTime,
                                  OrderStatus status) const;
  void place_order(const std::string &account_id, const Order &order) const;

  void addAuth(const std::string &key, const std::string &token);

 private:
  bool request_fields[53];
  const char *quote_fields[53] = {"Symbol",
                                  "Bid Price",
                                  "Ask Price",
                                  "Last Price",
                                  "Bid Size",
                                  "Ask Size",
                                  "Ask ID",
                                  "Bid ID",
                                  "Total Volume",
                                  "Last Size",
                                  "Trade Time",
                                  "Quote Time",
                                  "High Price",
                                  "Low Price",
                                  "Bid Tick",
                                  "Close Price",
                                  "Exchange ID",
                                  "Marginable",
                                  "Shortable",
                                  "Island Bid",
                                  "Island Ask",
                                  "Island Volume",
                                  "Quote Day",
                                  "Trade Day",
                                  "Volatility",
                                  "Description",
                                  "Last ID",
                                  "Digits",
                                  "Open Price",
                                  "Net Change",
                                  "52 Week High",
                                  "52 Week Low",
                                  "PE Ratio",
                                  "Dividend Amount",
                                  "Dividend Yield",
                                  "Island Bid Size",
                                  "Island Ask Size",
                                  "NAV",
                                  "Fund Price",
                                  "Exchange Name",
                                  "Dividend Date",
                                  "Regular Market Quote",
                                  "Regular Market Trade",
                                  "Regular Market Last Price",
                                  "Regular Market Last Size",
                                  "Regular Market Trade Time",
                                  "Regular Market Trade Day",
                                  "Regular Market Net Change",
                                  "Security Status",
                                  "Mark",
                                  "Quote Time in Long",
                                  "Trade Time in Long",
                                  "Regular Market Trade Time in Long"};

  // Flags
  bool has_access_token = false;
  bool has_user_principals = false;

  // API std::strings
  std::string api_key = "";
  std::string refresh_token = "";
  std::string access_token = "";

  // API Data
  Parser parser;
  std::unordered_map<std::string, std::string> account_data;
  UserPrincipals user_principals;
  json::ptree _user_principals;

  // WebSocket session variables
  net::io_context ioc;
  boost::asio::thread_pool ioc_pool;
  std::shared_ptr<tda::Socket> websocket_session;
  std::shared_ptr<std::vector<std::string>> websocket_buffer;
  ssl::context context{ssl::context::tlsv12_client};
  std::vector<std::shared_ptr<std::string const>> request_queue;
  std::vector<std::thread> ws_threads;

  // std::string Manipulation
  std::string get_api_interval_value(int value) const;
  std::string get_api_frequency_type(int value) const;
  std::string get_api_period_amount(int value) const;
  std::string get_api_frequency_amount(int value) const;

  // API Functions
  std::string send_request(const std::string &endpoint) const;
  std::string send_authorized_request(const std::string &endpoint) const;
  void post_authorized_request(const std::string &endpoint,
                               const std::string &data) const;
  std::string post_access_token() const;
  void get_user_principals();
  void check_user_principals();

  // WebSocket functions
  json::ptree create_login_request();
  json::ptree create_logout_request();
  json::ptree create_service_request(ServiceType serv_type,
                                     const std::string &keys,
                                     const std::string &fields);
};

}  // namespace tda
}  // namespace premia

#endif