#ifndef Plaid_hpp
#define Plaid_hpp

#include "service/Plaid/client.h"
#include "service/Plaid/parser.h"

namespace premia::plaid {

// Singleton facade over the Plaid API client.
//
// Typical startup sequence:
//
//   auto& p = Plaid::getInstance();
//   if (p.loadConfig()) {
//       p.loadTokens();             // no-op if file missing
//       if (!p.hasAccessToken()) {
//           std::string link_json = p.createLinkToken("user-123");
//           // … surface link_token to frontend via Plaid Link …
//           // … user completes Link, you receive a public_token …
//           p.exchangePublicToken(public_token);
//           p.saveTokens();
//       }
//   }
class Plaid {
 private:
  Plaid() = default;
  Client client_;

  static constexpr const char* kDefaultConfigPath = "assets/plaid.json";
  static constexpr const char* kDefaultTokenPath  = "assets/plaid_tokens.json";

 public:
  Plaid(Plaid const&) = delete;
  void operator=(Plaid const&) = delete;

  static Plaid& getInstance() {
    static Plaid instance;
    return instance;
  }

  // Load client_id / secret / env_url from a JSON config file.
  bool loadConfig(const std::string& path = kDefaultConfigPath) {
    return client_.LoadConfig(path);
  }

  // Load previously persisted access_token from disk.
  bool loadTokens(const std::string& path = kDefaultTokenPath) {
    return client_.LoadTokens(path);
  }

  // Persist current access_token to disk.
  void saveTokens(const std::string& path = kDefaultTokenPath) {
    client_.SaveTokens(path);
  }

  // Status
  bool isConfigured() const  { return client_.IsConfigured(); }
  bool hasAccessToken() const { return client_.HasAccessToken(); }

  // Plaid Link flow
  std::string createLinkToken(const std::string& user_id) const {
    return client_.CreateLinkToken(user_id);
  }

  bool exchangePublicToken(const std::string& public_token) {
    return client_.ExchangePublicToken(public_token);
  }

  // Raw JSON API
  std::string getRawAccounts() const      { return client_.GetAccounts(); }
  std::string getRawBalances() const      { return client_.GetBalances(); }
  std::string getRawTransactions(const std::string& start_date,
                                  const std::string& end_date) const {
    return client_.GetTransactions(start_date, end_date);
  }
  std::string getRawInstitution(const std::string& institution_id) const {
    return client_.GetInstitution(institution_id);
  }
  std::string getRawCategories() const    { return client_.GetCategories(); }

  // Parsed API
  std::vector<Account> getAccounts() const {
    return ParseAccounts(client_.GetAccounts());
  }
  std::vector<Account> getBalances() const {
    return ParseBalances(client_.GetBalances());
  }
  std::vector<Transaction> getTransactions(const std::string& start_date,
                                            const std::string& end_date) const {
    return ParseTransactions(client_.GetTransactions(start_date, end_date));
  }
  Institution getInstitution(const std::string& institution_id) const {
    return ParseInstitution(client_.GetInstitution(institution_id));
  }
  std::vector<std::string> getCategories() const {
    return ParseCategories(client_.GetCategories());
  }
};

}  // namespace premia::plaid

#endif  // Plaid_hpp
