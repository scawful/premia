#include <gtest/gtest.h>

#include <chrono>
#include <cstdio>
#include <fstream>
#include <string>

#include "service/Plaid/client.h"
#include "service/Plaid/parser.h"

namespace premiatests {
namespace ServiceTestSuite {
namespace PlaidTests {

// ---------------------------------------------------------------------------
// Helper: write a temporary file and return its path
// ---------------------------------------------------------------------------

static std::string write_tmp(const std::string& name, const std::string& body) {
  std::string path = "/tmp/" + name;
  std::ofstream f(path);
  f << body;
  return path;
}

// ---------------------------------------------------------------------------
// Fixture
// ---------------------------------------------------------------------------

class PlaidClientFixture : public ::testing::Test {
 protected:
  premia::plaid::Client client;

  std::string valid_config_path;
  std::string token_path;

  void SetUp() override {
    valid_config_path = write_tmp(
        "plaid_test_config.json",
        R"({"client_id":"test_client_id","secret":"test_secret","env_url":"https://sandbox.plaid.com"})");
    token_path = write_tmp("plaid_test_tokens.json", "{}");
  }

  void TearDown() override {
    std::remove(valid_config_path.c_str());
    std::remove(token_path.c_str());
  }
};

// ---------------------------------------------------------------------------
// IsConfigured
// ---------------------------------------------------------------------------

TEST_F(PlaidClientFixture, NotConfiguredByDefault) {
  EXPECT_FALSE(client.IsConfigured());
}

TEST_F(PlaidClientFixture, ConfiguredAfterLoadConfig) {
  EXPECT_TRUE(client.LoadConfig(valid_config_path));
  EXPECT_TRUE(client.IsConfigured());
}

TEST_F(PlaidClientFixture, LoadConfigMissingFile) {
  EXPECT_FALSE(client.LoadConfig("/tmp/does_not_exist_plaid.json"));
  EXPECT_FALSE(client.IsConfigured());
}

TEST_F(PlaidClientFixture, LoadConfigMalformed) {
  std::string bad = write_tmp("plaid_bad.json", "not json {{{");
  EXPECT_FALSE(client.LoadConfig(bad));
  std::remove(bad.c_str());
}

// ---------------------------------------------------------------------------
// Token persistence
// ---------------------------------------------------------------------------

TEST_F(PlaidClientFixture, NoAccessTokenByDefault) {
  EXPECT_FALSE(client.HasAccessToken());
}

TEST_F(PlaidClientFixture, LoadTokensMissingFile) {
  EXPECT_FALSE(client.LoadTokens("/tmp/no_such_plaid_tokens.json"));
}

TEST_F(PlaidClientFixture, LoadTokensEmptyFile) {
  EXPECT_FALSE(client.LoadTokens(token_path));
  EXPECT_FALSE(client.HasAccessToken());
}

TEST_F(PlaidClientFixture, LoadAndSaveTokensRoundTrip) {
  std::string token_json =
      R"({"access_token":"access-sandbox-abc123","item_id":"item-xyz789"})";
  std::string path = write_tmp("plaid_roundtrip.json", token_json);

  EXPECT_TRUE(client.LoadTokens(path));
  EXPECT_TRUE(client.HasAccessToken());

  // Save and reload — should still have a token.
  std::string save_path = "/tmp/plaid_saved.json";
  client.SaveTokens(save_path);

  premia::plaid::Client client2;
  EXPECT_TRUE(client2.LoadTokens(save_path));
  EXPECT_TRUE(client2.HasAccessToken());

  std::remove(path.c_str());
  std::remove(save_path.c_str());
}

// ---------------------------------------------------------------------------
// SaveConfig round-trip
// ---------------------------------------------------------------------------

TEST_F(PlaidClientFixture, SaveConfigRoundTrip) {
  ASSERT_TRUE(client.LoadConfig(valid_config_path));

  std::string save_path = "/tmp/plaid_config_saved.json";
  EXPECT_TRUE(client.SaveConfig(save_path));

  premia::plaid::Client client2;
  EXPECT_TRUE(client2.LoadConfig(save_path));
  EXPECT_TRUE(client2.IsConfigured());

  std::remove(save_path.c_str());
}

// ---------------------------------------------------------------------------
// ExchangePublicToken — no network, empty token path
// ---------------------------------------------------------------------------

TEST_F(PlaidClientFixture, ExchangeEmptyPublicTokenFails) {
  EXPECT_FALSE(client.ExchangePublicToken(""));
  EXPECT_FALSE(client.HasAccessToken());
}

// ---------------------------------------------------------------------------
// Parser: ParseAccounts
// ---------------------------------------------------------------------------

TEST(PlaidParserTest, ParseAccountsEmpty) {
  auto accounts = premia::plaid::ParseAccounts("");
  EXPECT_TRUE(accounts.empty());
}

TEST(PlaidParserTest, ParseAccountsInvalidJson) {
  auto accounts = premia::plaid::ParseAccounts("not json {{{");
  EXPECT_TRUE(accounts.empty());
}

TEST(PlaidParserTest, ParseAccountsValid) {
  const std::string json = R"({
    "accounts": [
      {
        "account_id": "acc-001",
        "name": "Plaid Checking",
        "official_name": "Plaid Gold Standard 0% Interest Checking",
        "type": "depository",
        "subtype": "checking",
        "mask": "0000",
        "balances": {
          "current": 110.0,
          "available": 100.0,
          "limit": 0.0,
          "iso_currency_code": "USD"
        }
      }
    ]
  })";

  auto accounts = premia::plaid::ParseAccounts(json);
  ASSERT_EQ(accounts.size(), 1u);
  EXPECT_EQ(accounts[0].account_id, "acc-001");
  EXPECT_EQ(accounts[0].name, "Plaid Checking");
  EXPECT_EQ(accounts[0].type, "depository");
  EXPECT_EQ(accounts[0].subtype, "checking");
  EXPECT_EQ(accounts[0].mask, "0000");
  EXPECT_DOUBLE_EQ(accounts[0].balances.current, 110.0);
  EXPECT_DOUBLE_EQ(accounts[0].balances.available, 100.0);
  EXPECT_EQ(accounts[0].balances.iso_currency_code, "USD");
}

// ---------------------------------------------------------------------------
// Parser: ParseTransactions
// ---------------------------------------------------------------------------

TEST(PlaidParserTest, ParseTransactionsEmpty) {
  auto txns = premia::plaid::ParseTransactions("");
  EXPECT_TRUE(txns.empty());
}

TEST(PlaidParserTest, ParseTransactionsValid) {
  const std::string json = R"({
    "transactions": [
      {
        "transaction_id": "tx-001",
        "account_id": "acc-001",
        "amount": 12.50,
        "date": "2024-01-15",
        "name": "Coffee Shop",
        "merchant_name": "Blue Bottle Coffee",
        "category": ["Food and Drink", "Coffee Shop"],
        "category_id": "13005043",
        "iso_currency_code": "USD",
        "pending": false
      }
    ]
  })";

  auto txns = premia::plaid::ParseTransactions(json);
  ASSERT_EQ(txns.size(), 1u);
  EXPECT_EQ(txns[0].transaction_id, "tx-001");
  EXPECT_EQ(txns[0].account_id, "acc-001");
  EXPECT_DOUBLE_EQ(txns[0].amount, 12.50);
  EXPECT_EQ(txns[0].date, "2024-01-15");
  EXPECT_EQ(txns[0].name, "Coffee Shop");
  EXPECT_EQ(txns[0].merchant_name, "Blue Bottle Coffee");
  EXPECT_EQ(txns[0].category.size(), 2u);
  EXPECT_EQ(txns[0].category[0], "Food and Drink");
  EXPECT_FALSE(txns[0].pending);
}

// ---------------------------------------------------------------------------
// Parser: ParseInstitution
// ---------------------------------------------------------------------------

TEST(PlaidParserTest, ParseInstitutionEmpty) {
  auto inst = premia::plaid::ParseInstitution("");
  EXPECT_TRUE(inst.institution_id.empty());
}

TEST(PlaidParserTest, ParseInstitutionValid) {
  const std::string json = R"({
    "institution": {
      "institution_id": "ins_109508",
      "name": "First Platypus Bank",
      "products": ["balance", "auth", "transactions"],
      "logo": "",
      "primary_color": "#bd1622",
      "url": "https://plaid.com"
    }
  })";

  auto inst = premia::plaid::ParseInstitution(json);
  EXPECT_EQ(inst.institution_id, "ins_109508");
  EXPECT_EQ(inst.name, "First Platypus Bank");
  EXPECT_EQ(inst.products.size(), 3u);
  EXPECT_EQ(inst.products[0], "balance");
  EXPECT_EQ(inst.primary_color, "#bd1622");
}

// ---------------------------------------------------------------------------
// Parser: ParseCategories
// ---------------------------------------------------------------------------

TEST(PlaidParserTest, ParseCategoriesEmpty) {
  auto cats = premia::plaid::ParseCategories("");
  EXPECT_TRUE(cats.empty());
}

TEST(PlaidParserTest, ParseCategoriesValid) {
  const std::string json = R"({
    "categories": [
      {
        "category_id": "10000000",
        "group": "special",
        "hierarchy": ["Bank Fees"]
      },
      {
        "category_id": "10001000",
        "group": "special",
        "hierarchy": ["Bank Fees", "ATM Fees"]
      }
    ]
  })";

  auto cats = premia::plaid::ParseCategories(json);
  ASSERT_EQ(cats.size(), 2u);
  EXPECT_EQ(cats[0], "Bank Fees");
  EXPECT_EQ(cats[1], "Bank Fees > ATM Fees");
}

// ---------------------------------------------------------------------------
// Parser: ParseBalances (same as ParseAccounts, via balance endpoint)
// ---------------------------------------------------------------------------

TEST(PlaidParserTest, ParseBalancesValid) {
  const std::string json = R"({
    "accounts": [
      {
        "account_id": "acc-002",
        "name": "Plaid Saving",
        "type": "depository",
        "subtype": "savings",
        "mask": "1111",
        "balances": {
          "current": 500.0,
          "available": 500.0,
          "limit": 0.0,
          "iso_currency_code": "USD"
        }
      }
    ]
  })";

  auto accounts = premia::plaid::ParseBalances(json);
  ASSERT_EQ(accounts.size(), 1u);
  EXPECT_EQ(accounts[0].account_id, "acc-002");
  EXPECT_DOUBLE_EQ(accounts[0].balances.current, 500.0);
}

}  // namespace PlaidTests
}  // namespace ServiceTestSuite
}  // namespace premiatests
