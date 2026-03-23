#include <chrono>
#include <csignal>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>

#include <curl/curl.h>
#include <gtest/gtest.h>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#if !defined(_WIN32)
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace {

namespace fs = std::filesystem;
namespace pt = boost::property_tree;

constexpr const char* kHost = "127.0.0.1";

auto WriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata)
    -> size_t {
  auto* output = static_cast<std::string*>(userdata);
  output->append(ptr, size * nmemb);
  return size * nmemb;
}

struct HttpResponse {
  long status_code = 0;
  std::string body;
};

auto HttpRequest(const std::string& method, const std::string& url,
                 const std::string& payload = "") -> HttpResponse {
  CURL* curl = curl_easy_init();
  EXPECT_NE(curl, nullptr);

  HttpResponse response;
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 3000L);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.body);

  struct curl_slist* headers = nullptr;
  if (method == "POST") {
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, payload.size());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  }

  const auto result = curl_easy_perform(curl);
  EXPECT_EQ(result, CURLE_OK);
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

  if (headers != nullptr) {
    curl_slist_free_all(headers);
  }
  curl_easy_cleanup(curl);
  return response;
}

auto ParseJson(const std::string& body) -> pt::ptree {
  std::istringstream input(body);
  pt::ptree tree;
  pt::read_json(input, tree);
  return tree;
}

auto FindConnectionStatus(const pt::ptree& tree, const std::string& provider)
    -> std::string {
  for (const auto& item : tree.get_child("data.connections")) {
    if (item.second.get<std::string>("provider") == provider) {
      return item.second.get<std::string>("status");
    }
  }
  return "";
}

auto MakeTempWorkspace(const std::string& name) -> fs::path {
  const auto path = fs::temp_directory_path() / name;
  fs::remove_all(path);
  fs::create_directories(path / "assets");

  for (const auto* asset : {"account.json", "orders.json", "options.json",
                            "portfolio.json", "plaid.json", "schwab.json",
                            "tda.json", "watchlists.json"}) {
    fs::copy_file(fs::path(PREMIA_SOURCE_DIR) / "assets" / asset,
                  path / "assets" / asset,
                  fs::copy_options::overwrite_existing);
  }
  return path;
}

#if !defined(_WIN32)
class PremiaApiProcess {
 public:
  explicit PremiaApiProcess(fs::path workspace)
      : workspace_(std::move(workspace)),
        port_(8300 + (std::rand() % 400)) {}

  void Start() {
    pid_ = fork();
    ASSERT_GE(pid_, 0);
    if (pid_ == 0) {
      chdir(workspace_.c_str());
      const auto port_string = std::to_string(port_);
      execl(PREMIA_API_BIN, PREMIA_API_BIN, "--host", kHost, "--port",
            port_string.c_str(), nullptr);
      _exit(127);
    }

    const auto health_url = BaseUrl() + "/health";
    for (int attempt = 0; attempt < 40; ++attempt) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      CURL* curl = curl_easy_init();
      if (curl == nullptr) {
        continue;
      }
      curl_easy_setopt(curl, CURLOPT_URL, health_url.c_str());
      curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 500L);
      const auto result = curl_easy_perform(curl);
      curl_easy_cleanup(curl);
      if (result == CURLE_OK) {
        return;
      }
    }
    FAIL() << "premia_api failed to start for integration test";
  }

  auto BaseUrl() const -> std::string {
    return std::string("http://") + kHost + ":" + std::to_string(port_);
  }

  ~PremiaApiProcess() {
    if (pid_ > 0) {
      kill(pid_, SIGTERM);
      int status = 0;
      waitpid(pid_, &status, 0);
    }
    fs::remove_all(workspace_);
  }

 private:
  fs::path workspace_;
  int port_ = 0;
  pid_t pid_ = -1;
};
#endif

class ApiIntegrationFixture : public ::testing::Test {
 protected:
  void SetUp() override {
    workspace_ = MakeTempWorkspace("premia-api-integration");
#if !defined(_WIN32)
    process_ = std::make_unique<PremiaApiProcess>(workspace_);
    process_->Start();
#endif
  }

  auto Url(const std::string& path) const -> std::string {
#if !defined(_WIN32)
    return process_->BaseUrl() + path;
#else
    return path;
#endif
  }

  fs::path workspace_;
#if !defined(_WIN32)
  std::unique_ptr<PremiaApiProcess> process_;
#endif
};

}  // namespace

namespace premiatests::ApiIntegrationTests {

TEST_F(ApiIntegrationFixture, AccountScreenReturnsFallbackData) {
#if defined(_WIN32)
  GTEST_SKIP() << "POSIX-only process launch helper";
#endif
  const auto response = HttpRequest("GET", Url("/v1/screens/account"));
  ASSERT_EQ(response.status_code, 200);
  const auto tree = ParseJson(response.body);
  EXPECT_EQ(tree.get<std::string>("data.accountId"), "local_acc");
  EXPECT_EQ(tree.get<std::string>("data.cash.amount"), "14320.00");
  EXPECT_FALSE(tree.get_child("data.positions").empty());
}

TEST_F(ApiIntegrationFixture, OptionsScreenReturnsFallbackChain) {
#if defined(_WIN32)
  GTEST_SKIP() << "POSIX-only process launch helper";
#endif
  const auto response = HttpRequest(
      "GET", Url("/v1/screens/options/SPY?strikeCount=8&strategy=SINGLE&range=ALL&expMonth=ALL&optionType=ALL"));
  ASSERT_EQ(response.status_code, 200);
  const auto tree = ParseJson(response.body);
  EXPECT_EQ(tree.get<std::string>("data.symbol"), "SPY");
  EXPECT_EQ(tree.get<std::string>("data.description"), "SPDR S&P 500 ETF");
  EXPECT_FALSE(tree.get_child("data.expirations").empty());
}

TEST_F(ApiIntegrationFixture, OrderLifecyclePersistsAcrossOpenAndHistoryRoutes) {
#if defined(_WIN32)
  GTEST_SKIP() << "POSIX-only process launch helper";
#endif
  const auto submit = HttpRequest(
      "POST", Url("/v1/orders/submit"),
      R"({"symbol":"AAPL","assetType":"EQUITY","instruction":"BUY","quantity":"10","orderType":"LIMIT","limitPrice":"217.00"})");
  ASSERT_EQ(submit.status_code, 200);
  const auto submit_tree = ParseJson(submit.body);
  const auto order_id = submit_tree.get<std::string>("data.submissionId");

  const auto open_before = HttpRequest("GET", Url("/v1/orders/open"));
  ASSERT_EQ(open_before.status_code, 200);
  auto open_tree = ParseJson(open_before.body);
  ASSERT_EQ(open_tree.get_child("data.orders").size(), 1u);

  const auto cancel = HttpRequest(
      "POST", Url("/v1/orders/" + order_id + "/cancel"), "{}");
  ASSERT_EQ(cancel.status_code, 200);

  const auto open_after = HttpRequest("GET", Url("/v1/orders/open"));
  ASSERT_EQ(open_after.status_code, 200);
  open_tree = ParseJson(open_after.body);
  EXPECT_EQ(open_tree.get_child("data.orders").size(), 0u);

  const auto history = HttpRequest("GET", Url("/v1/orders/history"));
  ASSERT_EQ(history.status_code, 200);
  const auto history_tree = ParseJson(history.body);
  ASSERT_EQ(history_tree.get_child("data.orders").size(), 1u);
  EXPECT_EQ(history_tree.get<std::string>("data.orders..status"), "cancelled");
}

TEST_F(ApiIntegrationFixture, ReplaceOrderCreatesNewOpenOrderAndHistoryEntries) {
#if defined(_WIN32)
  GTEST_SKIP() << "POSIX-only process launch helper";
#endif
  const auto submit = HttpRequest(
      "POST", Url("/v1/orders/submit"),
      R"({"symbol":"MSFT","assetType":"EQUITY","instruction":"BUY","quantity":"3","orderType":"LIMIT","limitPrice":"420.10"})");
  ASSERT_EQ(submit.status_code, 200);
  const auto submit_tree = ParseJson(submit.body);
  const auto order_id = submit_tree.get<std::string>("data.submissionId");

  const auto replace = HttpRequest(
      "POST", Url("/v1/orders/" + order_id + "/replace"),
      R"({"symbol":"MSFT","assetType":"EQUITY","instruction":"BUY","quantity":"1","orderType":"LIMIT","limitPrice":"419.75"})");
  ASSERT_EQ(replace.status_code, 200);
  const auto replace_tree = ParseJson(replace.body);
  EXPECT_EQ(replace_tree.get<std::string>("data.replacedOrderId"), order_id);

  const auto open_orders = HttpRequest("GET", Url("/v1/orders/open"));
  ASSERT_EQ(open_orders.status_code, 200);
  const auto open_tree = ParseJson(open_orders.body);
  ASSERT_EQ(open_tree.get_child("data.orders").size(), 1u);
  EXPECT_EQ(open_tree.get<std::string>("data.orders..quantity"), "1");

  const auto history = HttpRequest("GET", Url("/v1/orders/history"));
  ASSERT_EQ(history.status_code, 200);
  const auto history_tree = ParseJson(history.body);
  EXPECT_EQ(history_tree.get_child("data.orders").size(), 2u);
}

TEST_F(ApiIntegrationFixture, SchwabWorkflowUpdatesBootstrapState) {
#if defined(_WIN32)
  GTEST_SKIP() << "POSIX-only process launch helper";
#endif
  const auto start = HttpRequest(
      "POST", Url("/v1/connections/schwab/oauth/start"),
      R"({"redirectUri":"premia://schwab/callback","clientPlatform":"desktop"})");
  ASSERT_EQ(start.status_code, 200);
  const auto start_tree = ParseJson(start.body);
  EXPECT_NE(start_tree.get<std::string>("data.state"), "");
  EXPECT_NE(start_tree.get<std::string>("data.authUrl").find("oauth/authorize"),
            std::string::npos);

  const auto bootstrap_after_start = HttpRequest("GET", Url("/v1/bootstrap"));
  ASSERT_EQ(bootstrap_after_start.status_code, 200);
  auto bootstrap_tree = ParseJson(bootstrap_after_start.body);
  EXPECT_EQ(FindConnectionStatus(bootstrap_tree, "schwab"), "connecting");

  const auto complete = HttpRequest(
      "POST", Url("/v1/connections/schwab/oauth/complete"),
      R"({"callback":"demo-auth-code","state":"ignored"})");
  ASSERT_EQ(complete.status_code, 200);
  const auto complete_tree = ParseJson(complete.body);
  EXPECT_EQ(complete_tree.get<std::string>("data.provider"), "schwab");
  EXPECT_EQ(complete_tree.get<std::string>("data.status"), "connected");

  const auto bootstrap_after_complete = HttpRequest("GET", Url("/v1/bootstrap"));
  ASSERT_EQ(bootstrap_after_complete.status_code, 200);
  bootstrap_tree = ParseJson(bootstrap_after_complete.body);
  EXPECT_EQ(FindConnectionStatus(bootstrap_tree, "schwab"), "connected");
}

TEST_F(ApiIntegrationFixture, PlaidWorkflowUpdatesBootstrapState) {
#if defined(_WIN32)
  GTEST_SKIP() << "POSIX-only process launch helper";
#endif
  const auto start = HttpRequest(
      "POST", Url("/v1/connections/plaid/link-token"),
      R"({"userId":"integration-user"})");
  ASSERT_EQ(start.status_code, 200);
  const auto start_tree = ParseJson(start.body);
  EXPECT_NE(start_tree.get<std::string>("data.linkToken").find("link-sandbox-"),
            std::string::npos);

  const auto bootstrap_after_start = HttpRequest("GET", Url("/v1/bootstrap"));
  ASSERT_EQ(bootstrap_after_start.status_code, 200);
  auto bootstrap_tree = ParseJson(bootstrap_after_start.body);
  EXPECT_EQ(FindConnectionStatus(bootstrap_tree, "plaid"), "connecting");

  const auto complete = HttpRequest(
      "POST", Url("/v1/connections/plaid/link-complete"),
      R"({"publicToken":"public-sandbox-token","institutionId":"ins_123"})");
  ASSERT_EQ(complete.status_code, 200);
  const auto complete_tree = ParseJson(complete.body);
  EXPECT_EQ(complete_tree.get<std::string>("data.provider"), "plaid");
  EXPECT_EQ(complete_tree.get<std::string>("data.status"), "connected");

  const auto bootstrap_after_complete = HttpRequest("GET", Url("/v1/bootstrap"));
  ASSERT_EQ(bootstrap_after_complete.status_code, 200);
  bootstrap_tree = ParseJson(bootstrap_after_complete.body);
  EXPECT_EQ(FindConnectionStatus(bootstrap_tree, "plaid"), "connected");
}

}  // namespace premiatests::ApiIntegrationTests
