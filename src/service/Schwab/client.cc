#include "client.h"

#include <curl/curl.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <chrono>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "premia/infrastructure/secrets/runtime_paths.hpp"
#include "premia/infrastructure/secrets/secret_store.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

namespace premia {
namespace schwab {

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

const std::string Client::kTraderBaseUrl =
    "https://api.schwabapi.com/trader/v1";
const std::string Client::kMarketDataBaseUrl =
    "https://api.schwabapi.com/marketdata/v1";
const std::string Client::kTokenEndpoint =
    "https://api.schwabapi.com/v1/oauth/token";
const std::string Client::kAuthEndpoint =
    "https://api.schwabapi.com/v1/oauth/authorize";

// ---------------------------------------------------------------------------
// libcurl write callback (shared with TDA client pattern)
// ---------------------------------------------------------------------------

static size_t curl_write(const char* ptr, size_t size, size_t nmemb,
                         std::string* out) {
  size_t total = size * nmemb;
  try {
    out->append(ptr, total);
  } catch (const std::bad_alloc&) {
    return 0;
  }
  return total;
}

static size_t curl_header(const char* ptr, size_t size, size_t nmemb,
                          std::string* out) {
  size_t total = size * nmemb;
  if (out != nullptr) {
    out->append(ptr, total);
  }
  return total;
}

auto HeaderValue(const std::string& headers, const std::string& name)
    -> std::string {
  std::istringstream stream(headers);
  std::string line;
  const std::string needle = name + ":";
  while (std::getline(stream, line)) {
    if (line.size() < needle.size()) {
      continue;
    }
    bool matches = true;
    for (size_t index = 0; index < needle.size(); ++index) {
      if (std::tolower(static_cast<unsigned char>(line[index])) !=
          std::tolower(static_cast<unsigned char>(needle[index]))) {
        matches = false;
        break;
      }
    }
    if (!matches) {
      continue;
    }
    auto value = line.substr(needle.size());
    while (!value.empty() && (value.front() == ' ' || value.front() == '\t')) {
      value.erase(value.begin());
    }
    while (!value.empty() &&
           (value.back() == '\r' || value.back() == '\n' || value.back() == ' ')) {
      value.pop_back();
    }
    return value;
  }
  return "";
}

// ---------------------------------------------------------------------------
// Ctor / Dtor
// ---------------------------------------------------------------------------

Client::Client() { curl_global_init(CURL_GLOBAL_SSL); }
Client::~Client() { curl_global_cleanup(); }

// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------

bool Client::LoadConfig(const std::string& config_path) {
  std::ifstream file(config_path);
  if (!file.good()) return false;

  try {
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(file, pt);
    app_key_ = pt.get<std::string>("app_key", "");
    app_secret_ = pt.get<std::string>("app_secret", "");
    redirect_uri_ =
        pt.get<std::string>("redirect_uri", "https://127.0.0.1");
  } catch (const std::exception& e) {
    std::cerr << "[Schwab] LoadConfig error: " << e.what() << "\n";
    return false;
  }

  return IsConfigured();
}

bool Client::LoadTokens(const std::string& token_path) {
  std::ifstream file(token_path);
  if (!file.good()) return false;

  try {
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(file, pt);
    access_token_ = pt.get<std::string>("access_token", "");
    refresh_token_ = pt.get<std::string>("refresh_token", "");
    access_token_expires_at_ =
        pt.get<long long>("access_token_expires_at", 0LL);
    refresh_token_expires_at_ =
        pt.get<long long>("refresh_token_expires_at", 0LL);
  } catch (const std::exception& e) {
    std::cerr << "[Schwab] LoadTokens error: " << e.what() << "\n";
    return false;
  }

  return !refresh_token_.empty();
}

void Client::SaveTokens(const std::string& token_path) const {
  boost::property_tree::ptree pt;
  pt.put("access_token", access_token_);
  pt.put("refresh_token", refresh_token_);
  pt.put("access_token_expires_at", access_token_expires_at_);
  pt.put("refresh_token_expires_at", refresh_token_expires_at_);

  try {
    std::ostringstream buffer;
    boost::property_tree::write_json(buffer, pt);
    if (!premia::infrastructure::secrets::WriteSecureText(token_path,
                                                          buffer.str())) {
      std::cerr << "[Schwab] SaveTokens error: unable to write secure token file\n";
    }
    premia::infrastructure::secrets::SaveSecret(
        premia::infrastructure::secrets::ProviderKind::kSchwab,
        premia::infrastructure::secrets::SecretKind::kTokens, buffer.str());
  } catch (const std::exception& e) {
    std::cerr << "[Schwab] SaveTokens error: " << e.what() << "\n";
  }
}

// ---------------------------------------------------------------------------
// Status helpers
// ---------------------------------------------------------------------------

bool Client::IsConfigured() const {
  return !app_key_.empty() && !app_secret_.empty();
}

bool Client::HasValidAccessToken() const {
  if (access_token_.empty()) return false;
  auto now = std::chrono::system_clock::now();
  long long now_secs = std::chrono::duration_cast<std::chrono::seconds>(
                           now.time_since_epoch())
                           .count();
  return now_secs < access_token_expires_at_;
}

bool Client::HasValidRefreshToken() const {
  if (refresh_token_.empty()) return false;
  auto now = std::chrono::system_clock::now();
  long long now_secs = std::chrono::duration_cast<std::chrono::seconds>(
                           now.time_since_epoch())
                           .count();
  return now_secs < refresh_token_expires_at_;
}

// ---------------------------------------------------------------------------
// OAuth 2.0 Authorization Code flow
// ---------------------------------------------------------------------------

std::string Client::BuildAuthUrl() const {
  return kAuthEndpoint + "?response_type=code" + "&client_id=" + app_key_ +
         "&redirect_uri=" + redirect_uri_;
}

void Client::OpenBrowser() {
  std::string url = BuildAuthUrl();
#ifdef _WIN32
  ::ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWDEFAULT);
#else
#if __APPLE__
  const char* open_exec = "open";
#else
  const char* open_exec = "xdg-open";
#endif
  char command[2048];
  snprintf(command, sizeof(command), "%s \"%s\"", open_exec, url.c_str());
  system(command);
#endif
}

// static
std::string Client::ExtractCodeFromUrl(const std::string& url_or_code) {
  // If the input doesn't look like a URL, treat it as a raw code.
  if (url_or_code.find("://") == std::string::npos) {
    return url_or_code;
  }

  auto q = url_or_code.find('?');
  if (q == std::string::npos) return "";
  std::string query = url_or_code.substr(q + 1);

  const std::string search = "code=";
  auto start = query.find(search);
  if (start == std::string::npos) return "";
  start += search.size();

  auto end = query.find('&', start);
  if (end == std::string::npos) end = query.size();

  // The code value is URL-encoded in the query string; return it as-is so it
  // can be embedded directly in the form-encoded POST body.
  return query.substr(start, end - start);
}

bool Client::ExchangeAuthCode(const std::string& callback_url_or_code) {
  std::string code = ExtractCodeFromUrl(callback_url_or_code);
  if (code.empty()) {
    std::cerr << "[Schwab] ExchangeAuthCode: empty code\n";
    return false;
  }

  // redirect_uri must be form-encoded; it only contains safe chars here.
  std::string body = "grant_type=authorization_code"
                     "&code=" +
                     code + "&redirect_uri=" + redirect_uri_;

  std::string response = PostWithBasicAuth(kTokenEndpoint, body);
  if (response.empty()) {
    std::cerr << "[Schwab] ExchangeAuthCode: empty response from token "
                 "endpoint\n";
    return false;
  }

  return ParseTokenResponse(response);
}

bool Client::RefreshAccessToken() {
  if (refresh_token_.empty()) {
    std::cerr << "[Schwab] RefreshAccessToken: no refresh token\n";
    return false;
  }

  // URL-encode the refresh token for the POST body.
  CURL* curl = curl_easy_init();
  if (!curl) return false;
  char* escaped = curl_easy_escape(
      curl, refresh_token_.c_str(), static_cast<int>(refresh_token_.size()));
  std::string encoded_token = escaped ? escaped : refresh_token_;
  if (escaped) curl_free(escaped);
  curl_easy_cleanup(curl);

  std::string body = "grant_type=refresh_token&refresh_token=" + encoded_token;

  std::string response = PostWithBasicAuth(kTokenEndpoint, body);
  if (response.empty()) {
    std::cerr << "[Schwab] RefreshAccessToken: empty response\n";
    return false;
  }

  return ParseTokenResponse(response);
}

// ---------------------------------------------------------------------------
// Token response parsing
// ---------------------------------------------------------------------------

bool Client::ParseTokenResponse(const std::string& response) {
  try {
    std::istringstream ss(response);
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);

    // Check for error field
    auto error = pt.get_optional<std::string>("error");
    if (error) {
      std::cerr << "[Schwab] Token error: " << *error << " — "
                << pt.get<std::string>("error_description", "") << "\n";
      return false;
    }

    access_token_ = pt.get<std::string>("access_token", "");
    refresh_token_ = pt.get<std::string>("refresh_token", refresh_token_);

    long long expires_in = pt.get<long long>("expires_in", 1800LL);

    auto now = std::chrono::system_clock::now();
    long long now_secs = std::chrono::duration_cast<std::chrono::seconds>(
                             now.time_since_epoch())
                             .count();

    // Subtract 60-second buffer so we refresh slightly before actual expiry.
    access_token_expires_at_ = now_secs + expires_in - 60;

    // Schwab refresh tokens expire in 7 days but the value isn't returned in
    // the response; reset the expiry window on every successful token exchange.
    refresh_token_expires_at_ = now_secs + (7LL * 24 * 3600) - 300;

    return !access_token_.empty();
  } catch (const std::exception& e) {
    std::cerr << "[Schwab] ParseTokenResponse error: " << e.what() << "\n";
    return false;
  }
}

// ---------------------------------------------------------------------------
// Account number bootstrap
// ---------------------------------------------------------------------------

bool Client::GetAccountNumbers() {
  std::string url = kTraderBaseUrl + "/accounts/accountNumbers";
  std::string response = SendAuthorizedGet(url);
  if (response.empty()) return false;

  try {
    std::istringstream ss(response);
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);

    account_hashes_.clear();
    // Response is a JSON array; boost::property_tree represents arrays as
    // children with empty-string keys.
    for (const auto& [key, node] : pt) {
      AccountHash ah;
      ah.account_number = node.get<std::string>("accountNumber", "");
      ah.hash_value = node.get<std::string>("hashValue", "");
      if (!ah.account_number.empty() && !ah.hash_value.empty()) {
        account_hashes_.push_back(std::move(ah));
      }
    }

    return !account_hashes_.empty();
  } catch (const std::exception& e) {
    std::cerr << "[Schwab] GetAccountNumbers parse error: " << e.what() << "\n";
    return false;
  }
}

std::string Client::GetHashForAccount(
    const std::string& account_number) const {
  for (const auto& ah : account_hashes_) {
    if (ah.account_number == account_number) return ah.hash_value;
  }
  return "";
}

const std::vector<AccountHash>& Client::GetAllAccountHashes() const {
  return account_hashes_;
}

// ---------------------------------------------------------------------------
// Internal HTTP helpers
// ---------------------------------------------------------------------------

auto Client::SendAuthorizedRequest(const std::string& method,
                                   const std::string& url,
                                   const std::string* body) const
    -> AuthorizedResponse {
  CURL* curl = curl_easy_init();
  if (!curl) return {};

  AuthorizedResponse response;
  std::string response_headers;
  std::string auth_header = "Authorization: Bearer " + access_token_;

  struct curl_slist* headers = nullptr;
  headers = curl_slist_append(headers, auth_header.c_str());
  headers = curl_slist_append(headers, "Accept: application/json");
  if (body != nullptr) {
    headers = curl_slist_append(headers, "Content-Type: application/json");
  }

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.body);
  curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, curl_header);
  curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response_headers);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "premia-agent/1.0");
  curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

  if (method == "POST") {
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
  } else if (method != "GET") {
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
  }

  if (body != nullptr) {
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body->c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE,
                     static_cast<long>(body->size()));
  }

  CURLcode code = curl_easy_perform(curl);
  if (code == CURLE_OK) {
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);
  }
  response.location = HeaderValue(response_headers, "Location");

  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  return response;
}

std::string Client::SendAuthorizedGet(const std::string& url) const {
  return SendAuthorizedRequest("GET", url).body;
}

// POST to Schwab token endpoint using HTTP Basic Auth (app_key:app_secret).
std::string Client::PostWithBasicAuth(const std::string& url,
                                      const std::string& body) const {
  CURL* curl = curl_easy_init();
  if (!curl) return "";

  std::string response;
  std::string userpwd = app_key_ + ":" + app_secret_;

  struct curl_slist* headers = nullptr;
  headers = curl_slist_append(
      headers, "Content-Type: application/x-www-form-urlencoded");

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(body.size()));
  curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "premia-agent/1.0");
  curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

  curl_easy_perform(curl);
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  return response;
}

// ---------------------------------------------------------------------------
// Trader v1 endpoints
// ---------------------------------------------------------------------------

std::string Client::GetAccount(const std::string& account_hash) const {
  return SendAuthorizedGet(kTraderBaseUrl + "/accounts/" + account_hash +
                           "?fields=positions");
}

std::string Client::GetAllAccounts() const {
  return SendAuthorizedGet(kTraderBaseUrl + "/accounts?fields=positions");
}

std::string Client::PreviewOrder(const std::string& account_hash,
                                 const std::string& order_payload,
                                 long* status_code) const {
  const auto response = SendAuthorizedRequest(
      "POST", kTraderBaseUrl + "/accounts/" + account_hash + "/previewOrder",
      &order_payload);
  if (status_code != nullptr) {
    *status_code = response.status_code;
  }
  return response.body;
}

bool Client::PlaceOrder(const std::string& account_hash,
                        const std::string& order_payload,
                        std::string* order_location,
                        long* status_code) const {
  const auto response = SendAuthorizedRequest(
      "POST", kTraderBaseUrl + "/accounts/" + account_hash + "/orders",
      &order_payload);
  if (status_code != nullptr) {
    *status_code = response.status_code;
  }
  if (order_location != nullptr) {
    *order_location = response.location;
  }
  return response.status_code >= 200 && response.status_code < 300;
}

std::string Client::GetOrdersForAccount(const std::string& account_hash,
                                        const std::string& from_entered_time,
                                        const std::string& to_entered_time,
                                        int max_results,
                                        const std::string& status,
                                        long* status_code) const {
  std::string url = kTraderBaseUrl + "/accounts/" + account_hash +
                    "/orders?maxResults=" + std::to_string(max_results) +
                    "&fromEnteredTime=" + from_entered_time +
                    "&toEnteredTime=" + to_entered_time;
  if (!status.empty()) {
    url += "&status=" + status;
  }
  const auto response = SendAuthorizedRequest("GET", url);
  if (status_code != nullptr) {
    *status_code = response.status_code;
  }
  return response.body;
}

std::string Client::GetOrder(const std::string& account_hash,
                             const std::string& order_id,
                             long* status_code) const {
  const auto response = SendAuthorizedRequest(
      "GET", kTraderBaseUrl + "/accounts/" + account_hash + "/orders/" +
                 order_id);
  if (status_code != nullptr) {
    *status_code = response.status_code;
  }
  return response.body;
}

bool Client::CancelOrder(const std::string& account_hash,
                         const std::string& order_id,
                         long* status_code) const {
  const auto response = SendAuthorizedRequest(
      "DELETE", kTraderBaseUrl + "/accounts/" + account_hash + "/orders/" +
                    order_id);
  if (status_code != nullptr) {
    *status_code = response.status_code;
  }
  return response.status_code >= 200 && response.status_code < 300;
}

bool Client::ReplaceOrder(const std::string& account_hash,
                          const std::string& order_id,
                          const std::string& order_payload,
                          std::string* order_location,
                          long* status_code) const {
  const auto response = SendAuthorizedRequest(
      "PUT", kTraderBaseUrl + "/accounts/" + account_hash + "/orders/" +
                 order_id,
      &order_payload);
  if (status_code != nullptr) {
    *status_code = response.status_code;
  }
  if (order_location != nullptr) {
    *order_location = response.location;
  }
  return response.status_code >= 200 && response.status_code < 300;
}

// ---------------------------------------------------------------------------
// MarketData v1 endpoints
// ---------------------------------------------------------------------------

std::string Client::GetQuote(const std::string& symbol) const {
  return SendAuthorizedGet(kMarketDataBaseUrl + "/quotes?symbols=" + symbol);
}

std::string Client::GetPriceHistory(const std::string& symbol,
                                    const std::string& period_type,
                                    int period_amt,
                                    const std::string& frequency_type,
                                    int frequency_amt,
                                    bool extended_hours) const {
  std::string url = kMarketDataBaseUrl + "/pricehistory?symbol=" + symbol +
                    "&periodType=" + period_type +
                    "&period=" + std::to_string(period_amt) +
                    "&frequencyType=" + frequency_type +
                    "&frequency=" + std::to_string(frequency_amt) +
                    "&needExtendedHoursData=" +
                    (extended_hours ? "true" : "false");
  return SendAuthorizedGet(url);
}

std::string Client::GetOptionChain(const std::string& symbol,
                                   const std::string& contract_type,
                                   int strike_count,
                                   const std::string& strategy,
                                   const std::string& range,
                                   const std::string& exp_month,
                                   const std::string& option_type,
                                   bool include_quotes) const {
  std::string url =
      kMarketDataBaseUrl + "/chains?symbol=" + symbol +
      "&contractType=" + contract_type +
      "&strikeCount=" + std::to_string(strike_count) +
      "&includeUnderlyingQuote=" + (include_quotes ? "true" : "false") +
      "&strategy=" + strategy + "&range=" + range + "&expMonth=" + exp_month +
      "&optionType=" + option_type;
  return SendAuthorizedGet(url);
}

}  // namespace schwab
}  // namespace premia
