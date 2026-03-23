#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include "premia/providers/local/order_provider.hpp"

namespace {

auto MakeTempDir(const std::string& name) -> std::filesystem::path {
  const auto dir = std::filesystem::temp_directory_path() / name;
  std::filesystem::remove_all(dir);
  std::filesystem::create_directories(dir);
  return dir;
}

void WriteFile(const std::filesystem::path& path, const std::string& content) {
  std::ofstream output(path);
  output << content;
}

auto MakeIntent() -> premia::core::application::OrderIntentRequest {
  premia::core::application::OrderIntentRequest request;
  request.symbol = "AAPL";
  request.asset_type = "EQUITY";
  request.instruction = "BUY";
  request.quantity = "2";
  request.order_type = "LIMIT";
  return request;
}

}  // namespace

namespace premiatests::ProviderOrderTests {

TEST(LocalOrderProvider, PreviewUsesFallbackPriceAndDefaults) {
  const auto dir = MakeTempDir("premia-local-order-preview");
  premia::providers::local::OrderProvider provider((dir / "orders.json").string());

  auto preview = provider.PreviewOrder(MakeIntent());

  EXPECT_EQ(preview.account_id, "local_acc");
  EXPECT_EQ(preview.limit_price, "217.00");
  EXPECT_EQ(preview.estimated_total, "434.00");
  EXPECT_EQ(preview.mode, "simulated");
  EXPECT_EQ(preview.status, "preview");
  ASSERT_FALSE(preview.warnings.empty());
  EXPECT_EQ(preview.warnings.front(), "Using fallback local price estimate.");
}

TEST(LocalOrderProvider, SubmitPersistsAcceptedOrderAndShowsInOpenOrders) {
  const auto dir = MakeTempDir("premia-local-order-submit");
  const auto orders_path = dir / "orders.json";
  WriteFile(orders_path, "{\"orders\": []}");
  premia::providers::local::OrderProvider provider(orders_path.string());

  auto submission = provider.SubmitOrder(MakeIntent());
  auto open_orders = provider.GetOpenOrders(submission.account_id);

  EXPECT_EQ(submission.status, "accepted");
  EXPECT_EQ(submission.mode, "simulated");
  EXPECT_EQ(submission.message, "Preview only submission stored locally.");
  ASSERT_EQ(open_orders.size(), 1u);
  EXPECT_EQ(open_orders.front().order_id, submission.submission_id);
  EXPECT_EQ(open_orders.front().symbol, submission.symbol);
}

TEST(LocalOrderProvider, CancelRemovesOpenOrderButKeepsHistory) {
  const auto dir = MakeTempDir("premia-local-order-cancel");
  const auto orders_path = dir / "orders.json";
  WriteFile(orders_path, "{\"orders\": []}");
  premia::providers::local::OrderProvider provider(orders_path.string());

  const auto submission = provider.SubmitOrder(MakeIntent());
  const auto cancellation = provider.CancelOrder({submission.account_id, submission.submission_id, false});

  auto open_orders = provider.GetOpenOrders(submission.account_id);
  auto history = provider.GetOrderHistory(submission.account_id);

  EXPECT_EQ(cancellation.status, "cancelled");
  EXPECT_EQ(cancellation.mode, "simulated");
  EXPECT_TRUE(open_orders.empty());
  ASSERT_EQ(history.size(), 1u);
  EXPECT_EQ(history.front().status, "cancelled");
}

TEST(LocalOrderProvider, ReplaceCreatesNewAcceptedOrder) {
  const auto dir = MakeTempDir("premia-local-order-replace");
  const auto orders_path = dir / "orders.json";
  WriteFile(orders_path, "{\"orders\": []}");
  premia::providers::local::OrderProvider provider(orders_path.string());

  const auto original = provider.SubmitOrder(MakeIntent());
  auto replacement_intent = MakeIntent();
  replacement_intent.quantity = "5";
  replacement_intent.limit_price = "216.50";
  const auto replacement = provider.ReplaceOrder({original.submission_id, replacement_intent});

  auto open_orders = provider.GetOpenOrders(original.account_id);
  auto history = provider.GetOrderHistory(original.account_id);

  EXPECT_EQ(replacement.replaced_order_id, original.submission_id);
  EXPECT_NE(replacement.replacement_id, original.submission_id);
  EXPECT_EQ(open_orders.size(), 1u);
  EXPECT_EQ(open_orders.front().order_id, replacement.replacement_id);
  EXPECT_EQ(history.size(), 2u);
}

}  // namespace premiatests::ProviderOrderTests
