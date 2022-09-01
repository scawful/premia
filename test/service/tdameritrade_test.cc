#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

#include "absl/status/status.h"
#include "app/core/TDA.hpp"

namespace premiatests {
namespace ServiceTestSuite {
namespace TDATests {

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
    std::ifstream keyfile("../assets/apikey.txt");
    if (keyfile.good()) {
      std::stringstream buffer;
      buffer << keyfile.rdbuf();
      keyfile.close();
      buffer >> consumer_key;
      buffer >> refresh_token;
    } else {
      std::cout << "no file" << std::endl;
    }
    client_.addAuth(consumer_key, refresh_token);
    client_.fetch_access_token();
    client_.CreateChannel();
  }
};

TEST_F(TDAFixture, PostAccessTokenOk) {
  EXPECT_THAT(client().PostAccessToken(), absl::OkStatus());
}

TEST_F(TDAFixture, GetUserPrincipalsOK) {
  EXPECT_THAT(client().GetUserPrincipals(), absl::OkStatus());
}

TEST_F(TDAFixture, GetAccountOk) {
  std::string account_id = "some_account_id";
  EXPECT_THAT(client().GetAccount(account_id), absl::OkStatus());
}

TEST_F(TDAFixture, GetPriceHistoryOk) {
  EXPECT_THAT(client().GetPriceHistory("AAPL", premia::tda::PeriodType(1), 1,
                                       premia::tda::FrequencyType(1), 1, true),
              absl::OkStatus());
}

}  // namespace TDATests
}  // namespace ServiceTestSuite
}  // namespace premiatests