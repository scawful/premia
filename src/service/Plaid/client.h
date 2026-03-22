#ifndef Plaid_Client_hpp
#define Plaid_Client_hpp

#include <curl/curl.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace premia {
namespace plaid {

// Plaid API client.
//
// Unlike OAuth2 services (Schwab), Plaid uses a two-step institution linking
// flow via Plaid Link:
//   1. CreateLinkToken(user_id)     — server-side: get a link_token for the
//                                     frontend Link component
//   2. [user completes Plaid Link in browser, gets a public_token]
//   3. ExchangePublicToken(token)   — server-side: exchange public_token for
//                                     a permanent access_token
//   4. SaveTokens(path)             — persist access_token for reuse
//
// Auth: every request includes client_id + secret in the JSON body.
// All endpoints are POST.
class Client {
 public:
  Client();
  ~Client();

  // Configuration & token persistence
  bool LoadConfig(const std::string& config_path);
  bool SaveConfig(const std::string& config_path) const;
  bool LoadTokens(const std::string& token_path);
  void SaveTokens(const std::string& token_path) const;

  // Status
  bool IsConfigured() const;
  bool HasAccessToken() const;

  // Plaid Link flow (server-side)
  // Returns the link_token JSON string on success, empty on failure.
  std::string CreateLinkToken(const std::string& user_id) const;

  // Exchange the public_token returned by Plaid Link for a permanent
  // access_token + item_id.  Stores the access_token internally and returns
  // true on success.
  bool ExchangePublicToken(const std::string& public_token);

  // API endpoints — each returns raw JSON string.
  std::string GetAccounts() const;
  std::string GetBalances() const;
  std::string GetTransactions(const std::string& start_date,
                              const std::string& end_date) const;
  std::string GetInstitution(const std::string& institution_id) const;
  std::string GetCategories() const;

 private:
  // Config (loaded from plaid.json)
  std::string client_id_;
  std::string secret_;
  std::string env_url_;  // e.g. https://sandbox.plaid.com

  // Access token obtained after ExchangePublicToken()
  std::string access_token_;
  std::string item_id_;

  // HTTP helper: POST JSON body to endpoint, returns response body.
  // Plaid auth is always in the JSON body (client_id + secret), not headers.
  std::string Post(const std::string& endpoint,
                   boost::property_tree::ptree body) const;

  static const std::string kSandboxUrl;
  static const std::string kProductionUrl;
};

}  // namespace plaid
}  // namespace premia

#endif  // Plaid_Client_hpp
