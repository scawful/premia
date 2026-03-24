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
  ScopedEnvVar(const std::string& name, const std::string& value) : name_(name) {
    if (const char* existing = std::getenv(name.c_str())) {
      original_ = existing;
    }
    Set(value);
  }

  ~ScopedEnvVar() {
    if (original_.has_value()) {
      Set(*original_);
      return;
    }
    Unset();
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
            "{\"consumer_key\":\"YOUR_TDA_CONSUMER_KEY\",\"refresh_token\":\"YOUR_TDA_REFRESH_TOKEN\"}");
  WriteFile(workspace / "assets" / "account.json",
            "{\"account\":{\"accountId\":\"local_acc\",\"cash\":\"14320.00\",\"netLiquidation\":\"128345.22\",\"availableFunds\":\"28640.00\",\"longMarketValue\":\"113024.22\",\"shortMarketValue\":\"0.00\",\"buyingPower\":\"28640.00\",\"equity\":\"128345.22\",\"equityPercentage\":\"100.00\",\"marginBalance\":\"0.00\",\"positions\":[{\"symbol\":\"AAPL\",\"name\":\"Apple Inc.\",\"dayProfitLoss\":\"54.60\",\"dayProfitLossPercent\":\"0.60\",\"averagePrice\":\"200.50\",\"marketValue\":\"9114.00\",\"quantity\":\"42\"}]}}");
  WriteFile(workspace / "assets" / "portfolio.json",
            "{\"portfolio\":{\"totalValue\":\"128345.22\",\"dayChangeAbsolute\":\"842.13\",\"dayChangePercent\":\"0.66\",\"cash\":\"14320.00\",\"buyingPower\":\"28640.00\",\"holdingsCount\":1},\"holdings\":[{\"id\":\"holding_aapl\",\"symbol\":\"AAPL\",\"name\":\"Apple Inc.\",\"quantity\":\"42\",\"marketValue\":\"9114.00\",\"dayChangeAbsolute\":\"54.60\",\"dayChangePercent\":\"0.60\"}]}");
}

void WriteUnreachableIbkrConfig(const std::filesystem::path& workspace) {
  WriteFile(workspace / "assets" / "ibkr.json",
            "{\"host\":\"127.0.0.1\",\"port\":65535,\"client_id\":2901,\"account_id\":\"\"}");
}

auto MakePositionRow() -> app::AccountPositionRow {
  app::AccountPositionRow row;
  row.symbol = "AAPL";
  row.name = "Apple Inc.";
  row.day_profit_loss = {"54.60", "USD"};
  row.day_profit_loss_percent = "0.60";
  row.average_price = {"200.50", "USD"};
  row.market_value = {"9114.00", "USD"};
  row.quantity = "42";
  return row;
}

}  // namespace

namespace premiatests::PortfolioAccountServiceTests {

TEST(PortfolioAccountService, AccountHoldingsUseProviderScopedIds) {
  app::AccountDetail detail_snapshot;
  detail_snapshot.positions.push_back(MakePositionRow());

  const auto ibkr_holdings =
      detail::MakeHoldingsFromAccount(detail_snapshot, domain::Provider::kIBKR);
  ASSERT_EQ(ibkr_holdings.size(), 1u);
  EXPECT_EQ(ibkr_holdings.front().id, "ibkr:AAPL");

  const auto schwab_holdings = detail::MakeHoldingsFromAccount(
      detail_snapshot, domain::Provider::kSchwab);
  ASSERT_EQ(schwab_holdings.size(), 1u);
  EXPECT_EQ(schwab_holdings.front().id, "schwab:AAPL");
}

TEST(PortfolioAccountService, FallsBackToLocalDataWhenIbkrIsUnavailable) {
  const auto workspace = MakeWorkspace("premia-portfolio-account-service");
  WriteFallbackAssets(workspace);
  WriteUnreachableIbkrConfig(workspace);

  ScopedCurrentPath cwd(workspace);
  ScopedEnvVar runtime_dir("PREMIA_RUNTIME_DIR", (workspace / ".runtime").string());
  ScopedEnvVar disable_keychain("PREMIA_DISABLE_KEYCHAIN", "1");

  detail::ConnectionService connection_service;
  const auto ibkr_connection = connection_service.GetConnection("ibkr");
  EXPECT_EQ(ibkr_connection.status, domain::ConnectionStatus::kDegraded);

  detail::PortfolioAccountService service;
  const auto account = service.GetAccountDetail();
  EXPECT_EQ(account.account_id, "local_acc");
  ASSERT_EQ(account.positions.size(), 1u);
  EXPECT_EQ(account.positions.front().symbol, "AAPL");

  const auto portfolio = service.GetPortfolioSummary();
  EXPECT_EQ(portfolio.total_value.amount, "128345.22");
  EXPECT_EQ(portfolio.holdings_count, 1);

  const auto holdings = service.GetTopHoldings();
  ASSERT_EQ(holdings.size(), 1u);
  EXPECT_EQ(holdings.front().id, "holding_aapl");
}

}  // namespace premiatests::PortfolioAccountServiceTests
