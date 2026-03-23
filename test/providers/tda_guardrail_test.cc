#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include "premia/providers/tda/account_detail_provider.hpp"
#include "premia/providers/tda/options_provider.hpp"
#include "premia/providers/tda/order_provider.hpp"
#include "premia/providers/tda/portfolio_provider.hpp"
#include "premia/providers/tda/watchlist_provider.hpp"

namespace {

auto MakePlaceholderConfig() -> std::filesystem::path {
  const auto dir = std::filesystem::temp_directory_path() / "premia-tda-guardrail";
  std::filesystem::remove_all(dir);
  std::filesystem::create_directories(dir);
  const auto config = dir / "tda.json";
  std::ofstream output(config);
  output << "{\n"
            "  \"consumer_key\": \"YOUR_TDA_CONSUMER_KEY\",\n"
            "  \"refresh_token\": \"YOUR_TDA_REFRESH_TOKEN\"\n"
            "}\n";
  return config;
}

auto MakeIntent() -> premia::core::application::OrderIntentRequest {
  premia::core::application::OrderIntentRequest request;
  request.symbol = "SPY";
  request.asset_type = "EQUITY";
  request.instruction = "BUY";
  request.quantity = "1";
  request.order_type = "LIMIT";
  request.limit_price = "519.50";
  return request;
}

}  // namespace

namespace premiatests::TdaGuardrailTests {

TEST(TdaProviders, PlaceholderConfigFailsFast) {
  const auto config = MakePlaceholderConfig();

  premia::providers::tda::WatchlistProvider watchlists(config.string());
  premia::providers::tda::PortfolioProvider portfolio(config.string());
  premia::providers::tda::AccountDetailProvider account(config.string());
  premia::providers::tda::OptionsProvider options(config.string());
  premia::providers::tda::OrderProvider orders(config.string());

  EXPECT_THROW(watchlists.ListWatchlists(), std::runtime_error);
  EXPECT_THROW(portfolio.GetPortfolioSummary(), std::runtime_error);
  EXPECT_THROW(account.GetAccountDetail(), std::runtime_error);
  EXPECT_THROW(options.GetOptionChainSnapshot("SPY", "8", "SINGLE", "ALL", "ALL", "ALL"), std::runtime_error);
  EXPECT_THROW(orders.PreviewOrder(MakeIntent()), std::runtime_error);
  EXPECT_THROW(orders.GetOpenOrders(""), std::runtime_error);
  EXPECT_THROW(orders.GetOrderHistory(""), std::runtime_error);
}

}  // namespace premiatests::TdaGuardrailTests
