#ifndef Schwab_Client_hpp
#define Schwab_Client_hpp

#include <curl/curl.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <chrono>
#include <string>
#include <vector>

namespace premia {
namespace schwab {

struct AccountHash {
  std::string account_number;
  std::string hash_value;
};

// OAuth 2.0 confidential client for Schwab API.
//
// Auth flow:
//   1. LoadConfig("assets/schwab.json")       — loads app_key/app_secret
//   2. HasValidRefreshToken() → false first time
//   3. OpenBrowser()                          — opens consent URL
//   4. ExchangeAuthCode(callback_url_or_code) — exchanges code for tokens
//   5. SaveTokens("assets/schwab_tokens.json")
//   6. GetAccountNumbers()                    — bootstrap account hashes
//
// Subsequent runs:
//   1. LoadConfig / LoadTokens
//   2. HasValidAccessToken() → false  →  RefreshAccessToken()
//   3. GetAccountNumbers()
class Client {
 public:
  Client();
  ~Client();

  // Configuration & token persistence
  bool LoadConfig(const std::string& config_path);
  bool LoadTokens(const std::string& token_path);
  void SaveTokens(const std::string& token_path) const;

  // OAuth 2.0 Authorization Code flow
  std::string BuildAuthUrl() const;
  void OpenBrowser();
  // Accepts the full redirect callback URL or just the raw authorization code.
  bool ExchangeAuthCode(const std::string& callback_url_or_code);
  bool RefreshAccessToken();

  // Account number bootstrap — must be called after obtaining a valid access
  // token. Populates the account_number → hash mapping needed for all
  // account-scoped endpoints.
  bool GetAccountNumbers();
  std::string GetHashForAccount(const std::string& account_number) const;
  const std::vector<AccountHash>& GetAllAccountHashes() const;

  // Status
  bool IsConfigured() const;
  bool HasValidAccessToken() const;
  bool HasValidRefreshToken() const;

  // Trader v1 endpoints (https://api.schwabapi.com/trader/v1)
  std::string GetAccount(const std::string& account_hash) const;
  std::string GetAllAccounts() const;

  // MarketData v1 endpoints (https://api.schwabapi.com/marketdata/v1)
  std::string GetQuote(const std::string& symbol) const;
  std::string GetPriceHistory(const std::string& symbol,
                              const std::string& period_type, int period_amt,
                              const std::string& frequency_type,
                              int frequency_amt, bool extended_hours) const;
  std::string GetOptionChain(const std::string& symbol,
                             const std::string& contract_type, int strike_count,
                             const std::string& strategy,
                             const std::string& range,
                             const std::string& exp_month,
                             const std::string& option_type,
                             bool include_quotes) const;

 private:
  // Config (loaded from schwab.json)
  std::string app_key_;
  std::string app_secret_;
  std::string redirect_uri_ = "https://127.0.0.1";

  // Tokens
  std::string access_token_;
  std::string refresh_token_;
  long long access_token_expires_at_ = 0;   // unix epoch seconds
  long long refresh_token_expires_at_ = 0;  // unix epoch seconds

  // Account hashes populated by GetAccountNumbers()
  std::vector<AccountHash> account_hashes_;

  // HTTP helpers
  std::string SendAuthorizedGet(const std::string& url) const;
  std::string PostWithBasicAuth(const std::string& url,
                                const std::string& body) const;
  bool ParseTokenResponse(const std::string& response);

  // Extracts the `code` query parameter from a redirect callback URL, or
  // returns the input unchanged if it does not look like a URL.
  static std::string ExtractCodeFromUrl(const std::string& url_or_code);

  static const std::string kTraderBaseUrl;
  static const std::string kMarketDataBaseUrl;
  static const std::string kTokenEndpoint;
  static const std::string kAuthEndpoint;
};

}  // namespace schwab
}  // namespace premia

#endif  // Schwab_Client_hpp
