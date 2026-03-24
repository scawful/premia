#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>

#include <gtest/gtest.h>

#include "provider_service_components.hpp"

namespace {

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
#if defined(_WIN32)
    _putenv_s(name_.c_str(), value.c_str());
#else
    setenv(name_.c_str(), value.c_str(), 1);
#endif
  }

  ~ScopedEnvVar() {
    if (original_.has_value()) {
#if defined(_WIN32)
      _putenv_s(name_.c_str(), original_->c_str());
#else
      setenv(name_.c_str(), original_->c_str(), 1);
#endif
      return;
    }
#if defined(_WIN32)
    _putenv_s(name_.c_str(), "");
#else
    unsetenv(name_.c_str());
#endif
  }

 private:
  std::string name_;
  std::optional<std::string> original_;
};

auto MakeWorkspace() -> std::filesystem::path {
  const auto dir = std::filesystem::temp_directory_path() / "premia-order-service";
  std::filesystem::remove_all(dir);
  std::filesystem::create_directories(dir / "assets");
  std::filesystem::create_directories(dir / ".runtime");
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
  request.quantity = "1";
  request.order_type = "LIMIT";
  return request;
}

}  // namespace

namespace premiatests::CoreOrderServiceTests {

TEST(OrderService, FallsBackToLocalProviderWhenBrokerOrderConfigsAreUnavailable) {
  const auto workspace = MakeWorkspace();
  WriteFile(workspace / "assets/schwab.json",
            "{\"app_key\":\"YOUR_SCHWAB_APP_KEY\",\"app_secret\":\"YOUR_SCHWAB_APP_SECRET\",\"redirect_uri\":\"https://127.0.0.1\"}");
  WriteFile(workspace / "assets/tda.json",
            "{\"consumer_key\":\"YOUR_TDA_CONSUMER_KEY\",\"refresh_token\":\"YOUR_TDA_REFRESH_TOKEN\"}");
  WriteFile(workspace / "assets/orders.json", "{\"orders\": []}");

  ScopedCurrentPath cwd(workspace);
  ScopedEnvVar runtime_dir("PREMIA_RUNTIME_DIR", (workspace / ".runtime").string());
  ScopedEnvVar disable_keychain("PREMIA_DISABLE_KEYCHAIN", "1");
  premia::core::application::detail::OrderService service;

  const auto preview = service.PreviewOrder(MakeIntent());
  const auto submission = service.SubmitOrder(MakeIntent());
  const auto open_orders = service.GetOpenOrders("");
  const auto history = service.GetOrderHistory("");

  EXPECT_EQ(preview.mode, "simulated");
  EXPECT_EQ(submission.status, "accepted");
  EXPECT_EQ(open_orders.size(), 1u);
  EXPECT_EQ(history.size(), 1u);
}

}  // namespace premiatests::CoreOrderServiceTests
