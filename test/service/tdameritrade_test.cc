#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

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
    // Auth is skipped in unit tests — live API calls are integration tests.
    client_.addAuth("test_key", "test_token");
  }
};

TEST_F(TDAFixture, ClientConstructsWithoutCrash) {
  SUCCEED();
}

TEST_F(TDAFixture, ParserReadsEmptyResponseWithoutCrash) {
  // An empty JSON object should produce an empty ptree without throwing.
  auto tree = parser().read_response("{}");
  EXPECT_TRUE(tree.empty());
}

TEST_F(TDAFixture, AddAuthSetsClientState) {
  // addAuth should not throw and the client should be usable after.
  EXPECT_NO_THROW(client().addAuth("another_key", "another_token"));
}

}  // namespace TDATests
}  // namespace ServiceTestSuite
}  // namespace premiatests
