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
    std::string consumer_key = "OPG0OQDNBFHHAFGM6YMMFJI4HY0WNTLZ";
    std::string refresh_token =
        "6LBJJIfdadHFUV6ibwex2LBluS7UuJO3dq2K6+pahpWSYiOV7EnvmXPpLR47Zyx/"
        "Ow9s1Fiho4trpY4TNc6M4rmTlP58kdg8XZPyBhZaCuv0GyWnarfh6RTbIvLX/"
        "wiIW7mv8eW+ZlGYy/"
        "uGF5hGLc9uF4RnaYDfjkD3uIVOLk3Ry5m8NmYZmF8Ff4MmFGwBp5Gvk6nKHwsrnlmGKss2"
        "CpOSYMckcl5iZN9n3me2NbIKuOCN6HpwYvGRfn9BkhahxyjHuExSvsgf3tNJ/"
        "9YfED85P+P35nP3Kbe/"
        "JeD4S1im14CAccyA9U2m8RQGHdbeFuAyoj51drb3cWHd9VhShfwfhbyNg8PbHwF+ouh+"
        "drSnK1gSR4wlWEBtKMuhX6kUqeNcHz9B6fEz8qZ9DFJP6Tcf9h8n6CVdM/"
        "W+qUHbOhFQCK64iQYXrJgpZrG100MQuG4LYrgoVi/"
        "JHHvlxuyBCkVBQfqeKJVdlVuYxbPQXTRpXiD1w0Iz/"
        "Hyu98WfF7HREcKoHUvz8YteEamzanby+voSi1bQE1M4jAkff8TbD3D+"
        "m7AoBaxfRcgfiJNT/"
        "RS1zmfv4aTh6eIuZLM8Wtj7DTKttK5XLBdcz4VIVsWHAn34M0ITdzyzNfl5Je3jZY2DWHb"
        "mavt06VnjzxRZ2hB/rZ/EBY/t3QQ5PNkkzww/"
        "+9qNp6OFN74iuTih0FJ4FoubigqA+Cc1Rf/"
        "SKPpteDp0EjX3yx69aaHtRYqXdcB0WerQTUKCLaLRxJtHWbrMLvgQDXX4WbV5DX7QsSf01"
        "SDTbOavNRyH4Zd2iwQM3VbjfGNxD7S+"
        "zod0KxySkqRcgyFua3GTXfYcZcaXklI1KkyORCdZXfMjmukdWKPtHj5B6Z2L0jrsVU8EsC"
        "pAVqGHlPt43hHJIsbfZNQ=212FD3x19z9sWBHDJACbC00B75E";
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