#ifndef Client_hpp
#define Client_hpp

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

#include "service/TDAmeritrade/Data/Order.hpp"
#include "service/TDAmeritrade/Data/UserPrincipals.hpp"
#include "service/TDAmeritrade/Parser.hpp"
#include "service/TDAmeritrade/Socket.hpp"
#include "util.h"
#include "metatypes.h"

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

static const String EnumAPIValues[]{"day", "month", "year", "ytd"};
static const String EnumAPIFreq[]{"minute", "daily", "weekly", "monthly"};
static const String EnumAPIPeriod[]{"1", "2",  "3",  "4", "5",
                                    "6", "10", "15", "20"};
static const String EnumAPIFreqAmt[]{"1", "5", "10", "15", "30"};
static const String EnumAPIServiceName[]{
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

  // API Strings
  String api_key = "";
  String refresh_token = "";
  String access_token = "";

  // API Data
  Parser parser;
  StringMap account_data;
  UserPrincipals user_principals;
  json::ptree _user_principals;

  // WebSocket session variables
  net::io_context ioc;
  boost::asio::thread_pool ioc_pool;
  std::shared_ptr<tda::Socket> websocket_session;
  std::shared_ptr<ArrayList<String>> websocket_buffer;
  ssl::context context{ssl::context::tlsv12_client};
  ArrayList<std::shared_ptr<String const>> request_queue;
  ArrayList<std::thread> ws_threads;

  // String Manipulation
  String get_api_interval_value(int value) const;
  String get_api_frequency_type(int value) const;
  String get_api_period_amount(int value) const;
  String get_api_frequency_amount(int value) const;

  // API Functions
  String send_request(CRString endpoint) const;
  String send_authorized_request(CRString endpoint) const;
  void post_authorized_request(CRString endpoint, CRString data) const;
  String post_access_token() const;
  void get_user_principals();
  void check_user_principals();

  // WebSocket functions
  json::ptree create_login_request();
  json::ptree create_logout_request();
  json::ptree create_service_request(ServiceType serv_type, CRString keys,
                                     CRString fields);

 public:
  Client();
  ~Client();
  void api_login();

  // WebSocket Controls
  void start_session(Logger logger, CRString ticker);
  void send_logout_request();
  void fetch_access_token();

  // Accounts
  String get_account(CRString account_id);
  String get_all_accounts();
  StringList get_all_account_ids();

  // Quotes
  String get_quote(CRString symbol) const;

  // Watchlists
  String get_watchlist_by_account(CRString account_id) const;

  // Price History
  String get_price_history(CRString symbol, PeriodType ptype, int period_amt,
                           FrequencyType ftype, int freq_amt, bool ext) const;

  // Option Chain
  String get_option_chain(CRString ticker, CRString contractType,
                          CRString strikeCount, bool includeQuotes,
                          CRString strategy, CRString range, CRString expMonth,
                          CRString optionType) const;

  // Orders
  String get_order(CRString account_id, CRString order_id) const;
  String get_orders_by_query(CRString account_id, int maxResults,
                             double fromEnteredTime, double toEnteredTime,
                             OrderStatus status) const;
  void place_order(CRString account_id, const Order &order) const;

  void addAuth(CRString, CRString);
};

}  // namespace tda
}  // namespace premia

#endif