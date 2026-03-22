#ifndef Schwab_hpp
#define Schwab_hpp

#include "service/Schwab/client.h"

namespace premia::schwab {

// Singleton facade over the Schwab API client.
//
// Typical startup sequence:
//
//   auto& s = Schwab::getInstance();
//   if (s.loadConfig()) {
//       s.loadTokens();
//       if (!s.isAuthenticated()) {
//           if (s.hasRefreshToken()) {
//               s.refreshAuth();
//           } else {
//               s.openAuthBrowser();
//               // … user authorizes, pastes callback URL …
//               s.exchangeAuthCode(callback_url);
//           }
//           s.saveTokens();
//       }
//       s.bootstrapAccounts();
//   }
class Schwab {
 private:
  Schwab() = default;
  Client client_;
  bool bootstrapped_ = false;

  static constexpr const char* kDefaultConfigPath = "assets/schwab.json";
  static constexpr const char* kDefaultTokenPath = "assets/schwab_tokens.json";

 public:
  Schwab(Schwab const&) = delete;
  void operator=(Schwab const&) = delete;

  static Schwab& getInstance() {
    static Schwab instance;
    return instance;
  }

  // Load app_key / app_secret / redirect_uri from a JSON config file.
  // Returns true on success.
  bool loadConfig(const std::string& path = kDefaultConfigPath) {
    return client_.LoadConfig(path);
  }

  // Load previously persisted tokens.  Returns true if a refresh token was
  // found (access token may still be expired).
  bool loadTokens(const std::string& path = kDefaultTokenPath) {
    return client_.LoadTokens(path);
  }

  // Persist current tokens to disk so they survive across process restarts.
  void saveTokens(const std::string& path = kDefaultTokenPath) {
    client_.SaveTokens(path);
  }

  // Open the browser to the Schwab consent URL. The user will be redirected to
  // redirect_uri with ?code=… after authorizing.
  void openAuthBrowser() { client_.OpenBrowser(); }

  // Exchange the authorization code (or full callback URL) for tokens.
  // Returns true on success; call saveTokens() afterwards.
  bool exchangeAuthCode(const std::string& callback_url_or_code) {
    return client_.ExchangeAuthCode(callback_url_or_code);
  }

  // Use the stored refresh token to obtain a new access token.
  // Returns true on success; call saveTokens() afterwards.
  bool refreshAuth() { return client_.RefreshAccessToken(); }

  // Fetch the encrypted account hashes required for account-scoped endpoints.
  // Must be called after obtaining a valid access token.
  bool bootstrapAccounts() {
    bootstrapped_ = client_.GetAccountNumbers();
    return bootstrapped_;
  }

  // Status
  bool isConfigured() const { return client_.IsConfigured(); }
  bool isAuthenticated() const { return client_.HasValidAccessToken(); }
  bool hasRefreshToken() const { return client_.HasValidRefreshToken(); }
  bool isBootstrapped() const { return bootstrapped_; }

  // Account access
  const std::vector<AccountHash>& getAccountHashes() const {
    return client_.GetAllAccountHashes();
  }

  std::string getHashForAccount(const std::string& account_number) const {
    return client_.GetHashForAccount(account_number);
  }

  // Returns the hash for the first account, or empty string if none.
  std::string getDefaultAccountHash() const {
    const auto& hashes = client_.GetAllAccountHashes();
    if (hashes.empty()) return "";
    return hashes.front().hash_value;
  }

  // Returns the plain account number for the first account, or empty string.
  std::string getDefaultAccountNumber() const {
    const auto& hashes = client_.GetAllAccountHashes();
    if (hashes.empty()) return "";
    return hashes.front().account_number;
  }

  // Raw API access (returns JSON strings; parsing is Phase 2).
  std::string getRawAccount(const std::string& account_hash) const {
    return client_.GetAccount(account_hash);
  }
  std::string getRawAllAccounts() const { return client_.GetAllAccounts(); }
  std::string getRawQuote(const std::string& symbol) const {
    return client_.GetQuote(symbol);
  }
  std::string getRawPriceHistory(const std::string& symbol,
                                 const std::string& period_type, int period_amt,
                                 const std::string& frequency_type,
                                 int frequency_amt,
                                 bool extended_hours) const {
    return client_.GetPriceHistory(symbol, period_type, period_amt,
                                   frequency_type, frequency_amt,
                                   extended_hours);
  }
  std::string getRawOptionChain(const std::string& symbol,
                                const std::string& contract_type,
                                int strike_count, const std::string& strategy,
                                const std::string& range,
                                const std::string& exp_month,
                                const std::string& option_type,
                                bool include_quotes) const {
    return client_.GetOptionChain(symbol, contract_type, strike_count, strategy,
                                  range, exp_month, option_type, include_quotes);
  }
};

}  // namespace premia::schwab

#endif  // Schwab_hpp
