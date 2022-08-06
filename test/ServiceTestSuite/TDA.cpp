#include "singletons/TDA.hpp"

#include <gtest/gtest.h>

#include <string>

namespace premiatests::ServiceTestSuite::TDATests {

class TDAFixture : public ::testing::Test {
 private:
  premia::tda::Client client_;
  premia::tda::Parser parser_;

 public:
  inline premia::tda::Client& client() { return client_; }
  inline premia::tda::Parser& parser() { return parser_; }

  void SetUp() override {
    std::string consumer_key;
    std::string refresh_token;
    Logger logger;
    std::ifstream keyfile("../assets/apikey.txt");
    if (keyfile.good()) {
      std::stringstream buffer;
      buffer << keyfile.rdbuf();
      keyfile.close();
      buffer >> consumer_key;
      buffer >> refresh_token;
    }
    client_.addAuth(consumer_key, refresh_token);
    client_.fetch_access_token();
  }
};

TEST_F(TDAFixture, get_and_parse_all_account_test) {
  std::string response = client().get_all_accounts();
  auto data = parser().read_response(response);
  parser().parse_all_accounts(data);
}

TEST_F(TDAFixture, get_and_parse_account_test) {
  auto all_ids = client().get_all_account_ids();
  std::string response = client().get_account(all_ids.at(0));
  auto data = parser().read_response(response);
  parser().parse_all_accounts(data);
}

TEST_F(TDAFixture, get_and_parse_option_chain_test) {
  std::string response = client().get_option_chain(
      "SPY", "ALL", "50", true, "ALL", "{range}", "{expMonth}", "{optionType}");
  auto data = parser().read_response(response);
  parser().parse_option_chain(data);
}

}  // namespace premiatests::ServiceTestSuite::TDATests