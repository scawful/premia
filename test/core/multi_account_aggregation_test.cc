#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>

#include <gtest/gtest.h>

#include "provider_service_components.hpp"

namespace {

namespace app = premia::core::application;
namespace detail = premia::core::application::detail;
namespace domain = premia::core::domain;

class ScopedCurrentPath {
 public:
  explicit ScopedCurrentPath(const std::filesystem::path& next)
      : original_(std::filesystem::current_path()) {
    std::filesystem::current_path(next);
  }
  ~ScopedCurrentPath() { std::filesystem::current_path(original_); }

 private:
  std::filesystem::path original_;
};

class ScopedEnvVar {
 public:
  ScopedEnvVar(const std::string& name, const std::string& value)
      : name_(name) {
    if (const char* existing = std::getenv(name.c_str())) {
      original_ = existing;
    }
    Set(value);
  }
  ~ScopedEnvVar() {
    if (original_.has_value()) {
      Set(*original_);
    } else {
      Unset();
    }
  }

 private:
  void Set(const std::string& value) {
#if defined(_WIN32)
    _putenv_s(name_.c_str(), value.c_str());
#else
    setenv(name_.c_str(), value.c_str(), 1);
#endif
  }
  void Unset() {
#if defined(_WIN32)
    _putenv_s(name_.c_str(), "");
#else
    unsetenv(name_.c_str());
#endif
  }
  std::string name_;
  std::optional<std::string> original_;
};

auto MakeWorkspace(const std::string& name) -> std::filesystem::path {
  const auto dir = std::filesystem::temp_directory_path() / name;
  std::filesystem::remove_all(dir);
  std::filesystem::create_directories(dir / "assets");
  std::filesystem::create_directories(dir / ".runtime");
  return dir;
}

void WriteFile(const std::filesystem::path& path, const std::string& content) {
  std::ofstream output(path);
  output << content;
}

void WriteFallbackAssets(const std::filesystem::path& workspace) {
  WriteFile(workspace / "assets" / "tda.json",
            "{\"consumer_key\":\"YOUR_TDA_CONSUMER_KEY\","
            "\"refresh_token\":\"YOUR_TDA_REFRESH_TOKEN\"}");
  WriteFile(workspace / "assets" / "schwab.json",
            "{\"app_key\":\"YOUR_SCHWAB_APP_KEY\","
            "\"app_secret\":\"YOUR_SCHWAB_APP_SECRET\"}");
  WriteFile(workspace / "assets" / "account.json",
            "{\"account\":{\"accountId\":\"local_acc\","
            "\"cash\":\"14320.00\","
            "\"netLiquidation\":\"128345.22\","
            "\"availableFunds\":\"28640.00\","
            "\"longMarketValue\":\"113024.22\","
            "\"shortMarketValue\":\"0.00\","
            "\"buyingPower\":\"28640.00\","
            "\"equity\":\"128345.22\","
            "\"equityPercentage\":\"100.00\","
            "\"marginBalance\":\"0.00\","
            "\"positions\":[{\"symbol\":\"AAPL\","
            "\"name\":\"Apple Inc.\","
            "\"dayProfitLoss\":\"54.60\","
            "\"dayProfitLossPercent\":\"0.60\","
            "\"averagePrice\":\"200.50\","
            "\"marketValue\":\"9114.00\","
            "\"quantity\":\"42\"}]}}");
  WriteFile(workspace / "assets" / "portfolio.json",
            "{\"portfolio\":{\"totalValue\":\"128345.22\","
            "\"dayChangeAbsolute\":\"842.13\","
            "\"dayChangePercent\":\"0.66\","
            "\"cash\":\"14320.00\","
            "\"buyingPower\":\"28640.00\","
            "\"holdingsCount\":1},"
            "\"holdings\":[{\"id\":\"holding_aapl\","
            "\"symbol\":\"AAPL\","
            "\"name\":\"Apple Inc.\","
            "\"quantity\":\"42\","
            "\"marketValue\":\"9114.00\","
            "\"dayChangeAbsolute\":\"54.60\","
            "\"dayChangePercent\":\"0.60\"}]}");
}

}  // namespace

namespace premiatests::MultiAccountAggregationTests {

TEST(MultiAccountAggregation, FallsBackToLocalWhenNoProviderConnected) {
  const auto workspace =
      MakeWorkspace("premia-multi-account-aggregation-test");
  WriteFallbackAssets(workspace);

  ScopedCurrentPath cwd(workspace);
  ScopedEnvVar runtime_dir("PREMIA_RUNTIME_DIR",
                           (workspace / ".runtime").string());
  ScopedEnvVar disable_keychain("PREMIA_DISABLE_KEYCHAIN", "1");

  detail::PortfolioAccountService service;
  const auto screen = service.GetMultiAccountHomeScreen();

  ASSERT_EQ(screen.accounts.size(), 1u);
  EXPECT_EQ(screen.accounts.front().provider, domain::Provider::kInternal);
  EXPECT_EQ(screen.accounts.front().account_id, "local_acc");
  EXPECT_EQ(screen.accounts.front().display_name, "Local Preview");
  EXPECT_EQ(screen.accounts.front().balance.amount, "128345.22");
  EXPECT_EQ(screen.accounts.front().holdings_count, 1);

  EXPECT_EQ(screen.aggregate_net_worth.amount, "128345.22");
  EXPECT_FALSE(screen.top_holdings.empty());
}

TEST(MultiAccountAggregation,
     AggregateNetWorthSumsAllAccountBalances) {
  // Build a minimal AccountDetail to exercise ComputeDayChangeFromPositions
  // via MakeHoldingsFromAccount; verify that the aggregate sums correctly
  // using two local accounts (we drive the local provider twice by tweaking
  // the asset file between calls, but here we just verify the math on the
  // already-returned screen from the single fallback path).
  const auto workspace =
      MakeWorkspace("premia-multi-account-aggregation-sum-test");
  WriteFallbackAssets(workspace);

  ScopedCurrentPath cwd(workspace);
  ScopedEnvVar runtime_dir("PREMIA_RUNTIME_DIR",
                           (workspace / ".runtime").string());
  ScopedEnvVar disable_keychain("PREMIA_DISABLE_KEYCHAIN", "1");

  detail::PortfolioAccountService service;
  const auto screen = service.GetMultiAccountHomeScreen();

  // aggregate_net_worth must equal the sum of account balances
  double expected = 0.0;
  for (const auto& account : screen.accounts) {
    expected += std::stod(account.balance.amount);
  }
  EXPECT_DOUBLE_EQ(std::stod(screen.aggregate_net_worth.amount), expected);
}

TEST(MultiAccountAggregation,
     PerAccountDayChangeComputedFromPositionProfitLoss) {
  const auto workspace =
      MakeWorkspace("premia-multi-account-aggregation-daychange-test");
  WriteFallbackAssets(workspace);

  ScopedCurrentPath cwd(workspace);
  ScopedEnvVar runtime_dir("PREMIA_RUNTIME_DIR",
                           (workspace / ".runtime").string());
  ScopedEnvVar disable_keychain("PREMIA_DISABLE_KEYCHAIN", "1");

  detail::PortfolioAccountService service;
  const auto screen = service.GetMultiAccountHomeScreen();

  // The local account has one position with dayProfitLoss = 54.60
  ASSERT_EQ(screen.accounts.size(), 1u);
  EXPECT_EQ(screen.accounts.front().day_change.absolute.amount, "54.60");
  // aggregate_day_change.absolute must match since there is only one account
  EXPECT_EQ(screen.aggregate_day_change.absolute.amount, "54.60");
}

TEST(MultiAccountAggregation, HoldingIdsUseProviderScopedPrefix) {
  app::AccountDetail detail;
  app::AccountPositionRow row;
  row.symbol = "MSFT";
  row.name = "Microsoft Corp.";
  row.day_profit_loss = {"12.50", "USD"};
  row.day_profit_loss_percent = "0.50";
  row.average_price = {"380.00", "USD"};
  row.market_value = {"38000.00", "USD"};
  row.quantity = "100";
  detail.positions.push_back(row);

  const auto schwab_holdings =
      detail::MakeHoldingsFromAccount(detail, domain::Provider::kSchwab);
  ASSERT_EQ(schwab_holdings.size(), 1u);
  EXPECT_EQ(schwab_holdings.front().id, "schwab:MSFT");

  const auto ibkr_holdings =
      detail::MakeHoldingsFromAccount(detail, domain::Provider::kIBKR);
  ASSERT_EQ(ibkr_holdings.size(), 1u);
  EXPECT_EQ(ibkr_holdings.front().id, "ibkr:MSFT");
}

}  // namespace premiatests::MultiAccountAggregationTests
