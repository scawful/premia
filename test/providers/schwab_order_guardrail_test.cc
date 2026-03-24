#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include "premia/providers/schwab/order_provider.hpp"

namespace {

auto MakePlaceholderSchwabConfig() -> std::filesystem::path {
  const auto dir = std::filesystem::temp_directory_path() /
                   "premia-schwab-order-guardrail";
  std::filesystem::remove_all(dir);
  std::filesystem::create_directories(dir);
  const auto config = dir / "schwab.json";
  std::ofstream output(config);
  output << "{\n"
            "  \"app_key\": \"YOUR_SCHWAB_APP_KEY\",\n"
            "  \"app_secret\": \"YOUR_SCHWAB_APP_SECRET\",\n"
            "  \"redirect_uri\": \"https://127.0.0.1\"\n"
            "}\n";
  return config;
}

auto MakeIntent() -> premia::core::application::OrderIntentRequest {
  premia::core::application::OrderIntentRequest request;
  request.symbol = "AAPL";
  request.asset_type = "EQUITY";
  request.instruction = "BUY";
  request.quantity = "1";
  request.order_type = "LIMIT";
  request.limit_price = "180.00";
  return request;
}

}  // namespace

namespace premiatests::SchwabOrderGuardrailTests {

TEST(SchwabProviders, PlaceholderOrderConfigFailsFast) {
  const auto config = MakePlaceholderSchwabConfig();
  const auto tokens = config.parent_path() / "schwab_tokens.json";

  premia::providers::schwab::OrderProvider provider(config.string(),
                                                    tokens.string());

  EXPECT_THROW(provider.PreviewOrder(MakeIntent()), std::runtime_error);
  EXPECT_THROW(provider.GetOpenOrders(""), std::runtime_error);
}

}  // namespace premiatests::SchwabOrderGuardrailTests
