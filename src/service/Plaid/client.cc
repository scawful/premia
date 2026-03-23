#include "client.h"

#include <curl/curl.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "premia/infrastructure/secrets/runtime_paths.hpp"

namespace premia {
namespace plaid {

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

const std::string Client::kSandboxUrl    = "https://sandbox.plaid.com";
const std::string Client::kProductionUrl = "https://production.plaid.com";

// ---------------------------------------------------------------------------
// libcurl write callback
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
    client_id_ = pt.get<std::string>("client_id", "");
    secret_    = pt.get<std::string>("secret", "");
    env_url_   = pt.get<std::string>("env_url", kSandboxUrl);
  } catch (const std::exception& e) {
    std::cerr << "[Plaid] LoadConfig error: " << e.what() << "\n";
    return false;
  }

  return IsConfigured();
}

bool Client::SaveConfig(const std::string& config_path) const {
  boost::property_tree::ptree pt;
  pt.put("client_id", client_id_);
  pt.put("secret", secret_);
  pt.put("env_url", env_url_);

  try {
    std::ostringstream buffer;
    boost::property_tree::write_json(buffer, pt);
    if (!premia::infrastructure::secrets::WriteSecureText(config_path,
                                                          buffer.str())) {
      std::cerr << "[Plaid] SaveConfig error: unable to write secure config file\n";
      return false;
    }
  } catch (const std::exception& e) {
    std::cerr << "[Plaid] SaveConfig error: " << e.what() << "\n";
    return false;
  }
  return true;
}

bool Client::LoadTokens(const std::string& token_path) {
  std::ifstream file(token_path);
  if (!file.good()) return false;

  try {
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(file, pt);
    access_token_ = pt.get<std::string>("access_token", "");
    item_id_      = pt.get<std::string>("item_id", "");
  } catch (const std::exception& e) {
    std::cerr << "[Plaid] LoadTokens error: " << e.what() << "\n";
    return false;
  }

  return !access_token_.empty();
}

void Client::SaveTokens(const std::string& token_path) const {
  boost::property_tree::ptree pt;
  pt.put("access_token", access_token_);
  pt.put("item_id", item_id_);

  try {
    std::ostringstream buffer;
    boost::property_tree::write_json(buffer, pt);
    if (!premia::infrastructure::secrets::WriteSecureText(token_path,
                                                          buffer.str())) {
      std::cerr << "[Plaid] SaveTokens error: unable to write secure token file\n";
    }
  } catch (const std::exception& e) {
    std::cerr << "[Plaid] SaveTokens error: " << e.what() << "\n";
  }
}

// ---------------------------------------------------------------------------
// Status helpers
// ---------------------------------------------------------------------------

bool Client::IsConfigured() const {
  return !client_id_.empty() && !secret_.empty();
}

bool Client::HasAccessToken() const {
  return !access_token_.empty();
}

// ---------------------------------------------------------------------------
// Internal HTTP helper
// ---------------------------------------------------------------------------

// Injects client_id and secret into body, serialises to JSON, POSTs to
// env_url + endpoint, and returns the response body string.
std::string Client::Post(const std::string& endpoint,
                         boost::property_tree::ptree body) const {
  // Inject auth credentials into every request body per Plaid spec.
  body.put("client_id", client_id_);
  body.put("secret", secret_);

  std::ostringstream oss;
  boost::property_tree::write_json(oss, body, false);
  std::string json_body = oss.str();

  std::string url = env_url_ + endpoint;

  CURL* curl = curl_easy_init();
  if (!curl) return "";

  std::string response;

  struct curl_slist* headers = nullptr;
  headers = curl_slist_append(headers, "Content-Type: application/json");

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_body.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE,
                   static_cast<long>(json_body.size()));
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
// Plaid Link flow
// ---------------------------------------------------------------------------

std::string Client::CreateLinkToken(const std::string& user_id) const {
  boost::property_tree::ptree body;

  // user object
  boost::property_tree::ptree user_node;
  user_node.put("client_user_id", user_id);
  body.add_child("user", user_node);

  body.put("client_name", "Premia");

  // products array
  boost::property_tree::ptree products;
  boost::property_tree::ptree product_node;
  product_node.put("", "transactions");
  products.push_back({"", product_node});
  body.add_child("products", products);

  // country_codes array
  boost::property_tree::ptree countries;
  boost::property_tree::ptree country_node;
  country_node.put("", "US");
  countries.push_back({"", country_node});
  body.add_child("country_codes", countries);

  body.put("language", "en");

  return Post("/link/token/create", body);
}

bool Client::ExchangePublicToken(const std::string& public_token) {
  if (public_token.empty()) {
    std::cerr << "[Plaid] ExchangePublicToken: empty public_token\n";
    return false;
  }

  boost::property_tree::ptree body;
  body.put("public_token", public_token);

  std::string response = Post("/item/public_token/exchange", body);
  if (response.empty()) {
    std::cerr << "[Plaid] ExchangePublicToken: empty response\n";
    return false;
  }

  try {
    std::istringstream ss(response);
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);

    auto error = pt.get_optional<std::string>("error_code");
    if (error) {
      std::cerr << "[Plaid] ExchangePublicToken error: " << *error << " — "
                << pt.get<std::string>("error_message", "") << "\n";
      return false;
    }

    access_token_ = pt.get<std::string>("access_token", "");
    item_id_      = pt.get<std::string>("item_id", "");
    return !access_token_.empty();
  } catch (const std::exception& e) {
    std::cerr << "[Plaid] ExchangePublicToken parse error: " << e.what() << "\n";
    return false;
  }
}

// ---------------------------------------------------------------------------
// API endpoints
// ---------------------------------------------------------------------------

std::string Client::GetAccounts() const {
  boost::property_tree::ptree body;
  body.put("access_token", access_token_);
  return Post("/accounts/get", body);
}

std::string Client::GetBalances() const {
  boost::property_tree::ptree body;
  body.put("access_token", access_token_);
  return Post("/accounts/balance/get", body);
}

std::string Client::GetTransactions(const std::string& start_date,
                                    const std::string& end_date) const {
  boost::property_tree::ptree body;
  body.put("access_token", access_token_);
  body.put("start_date", start_date);
  body.put("end_date", end_date);
  return Post("/transactions/get", body);
}

std::string Client::GetInstitution(
    const std::string& institution_id) const {
  boost::property_tree::ptree body;
  body.put("institution_id", institution_id);

  // country_codes array required by Plaid
  boost::property_tree::ptree countries;
  boost::property_tree::ptree country_node;
  country_node.put("", "US");
  countries.push_back({"", country_node});
  body.add_child("country_codes", countries);

  return Post("/institutions/get_by_id", body);
}

std::string Client::GetCategories() const {
  boost::property_tree::ptree body;
  return Post("/categories/get", body);
}

}  // namespace plaid
}  // namespace premia
