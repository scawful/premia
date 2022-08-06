#include "Client.hpp"

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

#include "Data/Order.hpp"
#include "Data/UserPrincipals.hpp"
#include "Parser.hpp"
#include "Socket.hpp"
#include "metatypes.h"
#include "util.h"

namespace premia {

static size_t json_write(const char *contents, size_t size, size_t nmemb,
                         std::string *s) {
  size_t new_length = size * nmemb;
  try {
    s->append(contents, new_length);
  } catch (const std::bad_alloc &e) {
    // @todo attach a logger
    return EXIT_FAILURE;
  }
  return new_length;
}

using namespace tda;

std::string Client::get_api_interval_value(int value) const {
  return EnumAPIValues[value];
}
std::string Client::get_api_frequency_type(int value) const {
  return EnumAPIFreq[value];
}
std::string Client::get_api_period_amount(int value) const {
  return EnumAPIPeriod[value];
}
std::string Client::get_api_frequency_amount(int value) const {
  return EnumAPIFreqAmt[value];
}

/**
 * @brief Send a request for data from the API using the json callback
 *
 * @param endpoint
 * @return std::string
 */
std::string Client::send_request(const std::string &endpoint) const {
  CURL *curl;
  CURLcode res;
  std::string response;

  curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, json_write);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "premia-agent/1.0");
  curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
  res = curl_easy_perform(curl);

  if (res != CURLE_OK)
    throw premia::TDAClientException("send_request() failed",
                                     curl_easy_strerror(res));

  curl_easy_cleanup(curl);
  return response;
}

/**
 * @brief Send an authorized request for data from the API using the json
 * callback
 * @author @scawful
 *
 * @param endpoint
 * @return std::string
 */
std::string Client::send_authorized_request(const std::string &endpoint) const {
  CURL *curl;
  CURLcode res;
  CURLHeader headers = nullptr;
  std::string response;
  std::string auth_bearer = "Authorization: Bearer " + access_token;

  curl = curl_easy_init();
  headers = curl_slist_append(headers, auth_bearer.c_str());

  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, json_write);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "premia-agent/1.0");
  curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

  res = curl_easy_perform(curl);
  if (res != CURLE_OK)
    throw premia::TDAClientException("send_authorized_request() failed",
                                     curl_easy_strerror(res));

  curl_easy_cleanup(curl);
  return response;
}

/**
 * @brief POST Request using access token
 *
 * @param endpoint
 * @param data
 */
void Client::post_authorized_request(const std::string &endpoint,
                                     const std::string &data) const {
  CURL *curl;
  CURLcode res;
  CURLHeader headers = nullptr;
  std::string response;

  curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPPOST, true);

  std::string auth_bearer = "Authorization: Bearer " + access_token;
  curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
  curl_slist_append(headers, auth_bearer.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "premia-agent/1.0");

  std::string easy_escape = curl_easy_escape(
      curl, refresh_token.c_str(), static_cast<int>(refresh_token.length()));
  std::string data_post =
      "grant_type=refresh_token&refresh_token=" + easy_escape +
      "&client_id=" + api_key;
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data_post.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data_post.length());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, json_write);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

  res = curl_easy_perform(curl);

  if (res != CURLE_OK)
    throw premia::TDAClientException("post_authorized_request() failed",
                                     curl_easy_strerror(res));

  curl_easy_cleanup(curl);
}

/**
 * @brief Send a POST request using the consumer key and refresh token to get
 * the access token
 * @author @scawful
 *
 * @return std::string
 */
std::string Client::post_access_token() const {
  CURL *curl;
  CURLcode res;
  CURLHeader headers = nullptr;
  std::string response;

  curl = curl_easy_init();

  curl_easy_setopt(curl, CURLOPT_URL,
                   "https://api.tdameritrade.com/v1/oauth2/token");
  curl_easy_setopt(curl, CURLOPT_HTTPPOST, true);
  headers = curl_slist_append(
      headers, "Content-Type: application/x-www-form-urlencoded");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  CURLHeader chunk = nullptr;  // chunked request for http1.1/200 ok
  chunk = curl_slist_append(chunk, "Transfer-Encoding: chunked");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

  // specify post data, have to url encode the refresh token
  std::string easy_escape = curl_easy_escape(
      curl, refresh_token.c_str(), static_cast<int>(refresh_token.length()));
  std::string data_post =
      "grant_type=refresh_token&refresh_token=" + easy_escape +
      "&client_id=" + api_key;

  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data_post.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data_post.length());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, json_write);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "premia-agent/1.0");
  curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

  // run the operations
  res = curl_easy_perform(curl);

  if (res != CURLE_OK)
    throw premia::TDAClientException("post_access_token() failed",
                                     curl_easy_strerror(res));

  curl_easy_cleanup(curl);
  return response;
}

/**
 * @brief Get User Principals from API endpoint
 *        Parse and store in UserPrincipals object for local use
 */
void Client::get_user_principals() {
  std::string endpoint =
      "https://api.tdameritrade.com/v1/"
      "userprincipals?fields=streamerSubscriptionKeys,streamerConnectionInfo";
  std::string response = send_authorized_request(endpoint);
  _user_principals = parser.read_response(response);
  user_principals = parser.parse_user_principals(_user_principals);
  has_user_principals = true;
}

void Client::check_user_principals() {
  if (!has_user_principals) get_user_principals();
}

json::ptree Client::create_login_request() {
  json::ptree credentials;
  json::ptree requests;
  json::ptree parameters;

  check_user_principals();
  BOOST_FOREACH (json::ptree::value_type &v,
                 _user_principals.get_child("accounts.")) {
    for (auto const &acct_it : v.second) {
      account_data[acct_it.first] = acct_it.second.get_value<std::string>();
    }
    break;
  }

  requests.put("service", "ADMIN");
  requests.put("command", "LOGIN");
  requests.put("requestid", 1);
  requests.put("account", account_data["accountId"]);
  requests.put("source", _user_principals.get<std::string>(
                             json::ptree::path_type("streamerInfo.appId")));

  credentials.put("userid", account_data["accountId"]);
  credentials.put("company", account_data["company"]);
  credentials.put("segment", account_data["segment"]);
  credentials.put("cddomain", account_data["accountCdDomainId"]);
  credentials.put("userid", account_data["accountId"]);
  credentials.put("usergroup",
                  _user_principals.get<std::string>(
                      json::ptree::path_type("streamerInfo.userGroup")));
  credentials.put("accesslevel",
                  _user_principals.get<std::string>(
                      json::ptree::path_type("streamerInfo.accessLevel")));
  credentials.put("authorized", "Y");
  credentials.put("acl", _user_principals.get<std::string>(
                             json::ptree::path_type("streamerInfo.acl")));

  std::tm token_timestamp =
      {};  // token timestamp format :: 2021-08-10T14:57:11+0000
  std::string original_token_timestamp =
      _user_principals.get<std::string>("streamerInfo.tokenTimestamp");

  // remove 'T' character
  std::size_t found = original_token_timestamp.find('T');
  std::string reformatted_token_timestamp =
      original_token_timestamp.replace(found, 1, " ");

  // remove the UTC +0000 portion, will adjust for this manually
  found = reformatted_token_timestamp.find('+');
  reformatted_token_timestamp =
      reformatted_token_timestamp.replace(found, 5, " ");

  // convert string timestamp into time_t
  std::istringstream ss(reformatted_token_timestamp);
  ss >> std::get_time(&token_timestamp, "%Y-%m-%d %H:%M:%S");
  if (ss.fail()) {
    std::cout << "Token timestamp parse failed!" << std::endl;
  } else {
    // this is disgusting i'm sorry
    std::time_t token_timestamp_as_sec = std::mktime(&token_timestamp);
    std::chrono::time_point token_timestamp_point =
        std::chrono::system_clock::from_time_t(token_timestamp_as_sec);
    auto duration = token_timestamp_point.time_since_epoch();
    auto millis =
        std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    millis -= 18000000;
    millis -= 3600000;
    credentials.put("timestamp", millis);
  }

  credentials.put("appid", _user_principals.get<std::string>(
                               json::ptree::path_type("streamerInfo.appId")));

  std::string credential_str;  // format parameters
  for (const auto &[key, value] : (json::ptree)credentials) {
    credential_str += key + "%3D" + value.get_value<std::string>() + "%26";
  }
  std::size_t end = credential_str.size();
  credential_str.replace(end - 3, 3, "");

  parameters.put("token", _user_principals.get<std::string>(
                              json::ptree::path_type("streamerInfo.token")));
  parameters.put("version", "1.0");
  parameters.put("credential", credential_str);

  requests.add_child("parameters", parameters);  // include in requests
  return requests;
}

json::ptree Client::create_logout_request() {
  json::ptree requests;
  json::ptree parameters;
  requests.put("service", "ADMIN");
  requests.put("requestid", 3);
  requests.put("command", "LOGOUT");
  requests.put("account", account_data["accountId"]);
  requests.put("source", _user_principals.get<std::string>(
                             json::ptree::path_type("streamerInfo.appId")));
  requests.add_child("parameters", parameters);
  return requests;
}

json::ptree Client::create_service_request(ServiceType type,
                                           const std::string &keys,
                                           const std::string &fields) {
  json::ptree requests;
  json::ptree parameters;
  requests.put("service", EnumAPIServiceName[type]);
  requests.put("requestid", 2);
  requests.put("command", "SUBS");
  requests.put("account", account_data["accountId"]);
  requests.put("source", _user_principals.get<std::string>(
                             json::ptree::path_type("streamerInfo.appId")));
  parameters.put("keys", keys);
  parameters.put("fields", fields);
  requests.add_child("parameters", parameters);
  return requests;
}

Client::Client() { curl_global_init(CURL_GLOBAL_SSL); }
Client::~Client() { curl_global_cleanup(); }
void Client::api_login() {
  fetch_access_token();
  get_user_principals();
}

/**
 * @brief Start a WebSocket session quickly with a ticker and fields
 * @author @scawful
 *
 * @param ticker
 * @param fields
 */
void Client::start_session(Logger logger, const std::string &ticker) {
  std::string host;
  std::string port = "443";
  std::string all_requests;
  json::ptree requests;
  std::stringstream requests_text_stream;
  std::vector<json::ptree> requests_array;

  Try {
    host = _user_principals.get<std::string>("streamerInfo.streamerSocketUrl");
  }
  catch (const json::ptree_error &) {
    logger(
        "Client::start_session::ptree_bad_path@[streamerInfo."
        "streamerSocketUrl]");
  }
  finally {}
  Proceed;

  requests_array.push_back(create_login_request());
  requests_array.push_back(
      create_service_request(QUOTE, ticker, "0,1,2,3,4,5,6,7,8"));
  requests = bind_requests(requests_array);
  write_json(requests_text_stream, requests);
  all_requests = requests_text_stream.str();
  websocket_session = std::make_shared<tda::Socket>(
      ioc_pool.get_executor(), context, logger, all_requests);
  websocket_session->open(host.c_str(), port.c_str());
}

/**
 * @brief WebSocket session logout request
 *
 */
void Client::send_logout_request() {
  json::ptree logout_request = create_logout_request();
  std::stringstream logout_text_stream;
  json::write_json(logout_text_stream, logout_request);
  std::string logout_text = logout_text_stream.str();
  websocket_session->write(logout_text);
  websocket_session->close();
}

/**
 * @brief API Access Token retrieval
 *
 */
void Client::fetch_access_token() {
  access_token = parser.parse_access_token(post_access_token());
  has_access_token = true;
}

/**
 * @brief Request account data by the account id
 *        Return the API response after authorization
 *
 * @param account_id
 * @return std::string
 */
std::string Client::get_account(const std::string &account_id) {
  check_user_principals();
  std::string account_url =
      "https://api.tdameritrade.com/v1/accounts/"
      "{accountNum}?fields=positions,orders";
  Utils::string_replace(account_url, "{accountNum}", account_id);
  return send_authorized_request(account_url);
}

/**
 * @brief Get all account data as a response
 *
 * @return std::string
 */
std::string Client::get_all_accounts() {
  check_user_principals();
  std::string account_url =
      "https://api.tdameritrade.com/v1/accounts/?fields=positions,orders";
  return send_authorized_request(account_url);
}

/**
 * @brief Create a vector of all the account ids present on the API key
 *
 * @return std::vector<std::string>
 */
std::vector<std::string> Client::get_all_account_ids() {
  std::vector<std::string> accounts;
  api_login();

  for (const auto &[key, value] : _user_principals) {
    if (key == "accounts") {
      for (const auto &[key2, val2] : value) {
        for (const auto &[elementKey, elementValue] : val2) {
          if (elementKey == "accountId") {
            accounts.push_back(elementValue.get_value<std::string>());
          }
        }
      }
    }

    if (key == "primaryAccountId") {
      std::cout << "Here" << value.get_value<std::string>() << std::endl
                << std::flush;
      accounts.push_back(value.get_value<std::string>());
    }
  }

  return accounts;
}

/**
 * @brief Request quote data by the instrument symbol
 *        Return the API response
 *
 * @param symbol
 * @return std::string
 */
std::string Client::get_quote(const std::string &symbol) const {
  std::string url =
      "https://api.tdameritrade.com/v1/marketdata/{ticker}/quotes?apikey=" +
      api_key;
  Utils::string_replace(url, "{ticker}", symbol);
  return send_request(url);
}

/**
 * @brief Prepare a request for watchlist data by an account number
 *        Return the API response
 *
 * @param account_id
 * @return std::string
 */
std::string Client::get_watchlist_by_account(
    const std::string &account_id) const {
  std::string url =
      "https://api.tdameritrade.com/v1/accounts/{accountNum}/watchlists";
  Utils::string_replace(url, "{accountNum}", account_id);
  return send_authorized_request(url);
}

/**
 * @brief Prepare a request from the API for price history information
 *        Return the API response
 *
 * @param symbol
 * @param ptype
 * @param period_amt
 * @param ftype
 * @param freq_amt
 * @param ext
 * @return std::string
 */
std::string Client::get_price_history(const std::string &symbol,
                                      PeriodType ptype, int period_amt,
                                      FrequencyType ftype, int freq_amt,
                                      bool ext) const {
  std::string url =
      "https://api.tdameritrade.com/v1/marketdata/{ticker}/"
      "pricehistory?apikey=" +
      api_key +
      "&periodType={periodType}&period={period}&frequencyType={frequencyType}&"
      "frequency={frequency}&needExtendedHoursData={ext}";

  Utils::string_replace(url, "{ticker}", symbol);
  Utils::string_replace(url, "{periodType}", get_api_interval_value(ptype));
  Utils::string_replace(url, "{period}", get_api_period_amount(period_amt));
  Utils::string_replace(url, "{frequencyType}", get_api_frequency_type(ftype));
  Utils::string_replace(url, "{frequency}", get_api_frequency_amount(freq_amt));

  if (!ext)
    Utils::string_replace(url, "{ext}", "false");
  else
    Utils::string_replace(url, "{ext}", "true");

  return send_request(url);
}

/**
 * @brief Prepare a request from the API for option chain data
 *        Return the API response
 *
 * @param ticker
 * @param contractType
 * @param strikeCount
 * @param includeQuotes
 * @param strategy
 * @param range
 * @param expMonth
 * @param optionType
 * @return std::string
 */
std::string Client::get_option_chain(
    const std::string &ticker, const std::string &contractType,
    const std::string &strikeCount, bool includeQuotes,
    const std::string &strategy, const std::string &range,
    const std::string &expMonth, const std::string &optionType) const {
  OptionChain option_chain;
  std::string url =
      "https://api.tdameritrade.com/v1/marketdata/chains?apikey=" + api_key +
      "&symbol={ticker}&contractType={contractType}&strikeCount={strikeCount}&"
      "includeQuotes={includeQuotes}&strategy={strategy}&range={range}&"
      "expMonth={expMonth}&optionType={optionType}";

  Utils::string_replace(url, "{ticker}", ticker);
  Utils::string_replace(url, "{contractType}", contractType);
  Utils::string_replace(url, "{strikeCount}", strikeCount);
  Utils::string_replace(url, "{strategy}", strategy);
  Utils::string_replace(url, "{range}", range);
  Utils::string_replace(url, "{expMonth}", expMonth);
  Utils::string_replace(url, "{optionType}", optionType);

  if (!includeQuotes)
    Utils::string_replace(url, "{includeQuotes}", "FALSE");
  else
    Utils::string_replace(url, "{includeQuotes}", "TRUE");

  return send_request(url);
}

/**
 * @brief Retrieve order by the account and the order id
 *
 * @param account_id
 * @param order_id
 * @return std::string
 */
std::string Client::get_order(const std::string &account_id,
                              const std::string &order_id) const {
  std::string endpoint =
      "https://api.tdameritrade.com/v1/accounts/{accountId}/orders/{orderId}";
  Utils::string_replace(endpoint, "{accountId}", account_id);
  Utils::string_replace(endpoint, "{orderId}", order_id);
  return send_authorized_request(endpoint);
}

/**
 * @brief Retrieve Order using query parameters
 *
 * @param account_id
 * @param maxResults
 * @param fromEnteredTime
 * @param toEnteredTime
 * @param status
 * @return std::string
 */
std::string Client::get_orders_by_query(const std::string &account_id,
                                        int maxResults, double fromEnteredTime,
                                        double toEnteredTime,
                                        OrderStatus status) const {
  std::string endpoint =
      "https://api.tdameritrade.com/v1/accounts/{accountId}/"
      "orders?maxResults={maxResults}&fromEnteredTime={from}&toEnteredTime={to}"
      "&status={status}";
  Utils::string_replace(endpoint, "{accountId}", account_id);
  Utils::string_replace(endpoint, "{maxResults}", std::to_string(maxResults));
  Utils::string_replace(endpoint, "{from}", std::to_string(fromEnteredTime));
  Utils::string_replace(endpoint, "{to}", std::to_string(toEnteredTime));
  Utils::string_replace(endpoint, "{status}", "status");
  return send_authorized_request(endpoint);
}

/**
 * @brief Place an Order for the account by id
 *
 * @param account_id
 * @param order
 */
void Client::place_order(const std::string &account_id,
                         const Order &order) const {
  std::string endpoint =
      "https://api.tdameritrade.com/v1/accounts/{accountId}/orders";
  Utils::string_replace(endpoint, "{accountId}", account_id);
  post_authorized_request(endpoint, order.getString());
}

/**
 * @brief temp function for passing key to client
 *
 * @param key
 * @param token
 */
void Client::addAuth(const std::string &key, const std::string &token) {
  api_key = key;
  refresh_token = token;
}

}  // namespace premia