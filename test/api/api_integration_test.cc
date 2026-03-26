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
#include <fcntl.h>
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
  } else if (method != "GET") {
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
  }
  if (!payload.empty()) {
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, payload.size());
  }
  if (!payload.empty() || method == "POST" || method == "PATCH") {
    headers = curl_slist_append(headers, "Content-Type: application/json");
  }
  if (headers != nullptr) {
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

auto FindConnectionReauthRequired(const pt::ptree& tree,
                                  const std::string& provider) -> bool {
  for (const auto& item : tree.get_child("data.connections")) {
    if (item.second.get<std::string>("provider") == provider) {
      return item.second.get<bool>("reauthRequired");
    }
  }
  return false;
}

void WriteWorkspaceAsset(const fs::path& workspace, const std::string& name,
                        const std::string& content) {
  std::ofstream output(workspace / "assets" / name);
  output << content;
}

void WriteRuntimeProviderFile(const fs::path& workspace, const std::string& provider,
                              const std::string& name,
                              const std::string& content) {
  const auto dir = workspace / ".runtime" / "providers" / provider;
  fs::create_directories(dir);
  std::ofstream output(dir / name);
  output << content;
}

auto MakeTempWorkspace(const std::string& name) -> fs::path {
  const auto path = fs::temp_directory_path() / name;
  fs::remove_all(path);
  fs::create_directories(path / "assets");
  fs::create_directories(path / ".runtime");

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
      : workspace_(std::move(workspace)), port_(NextPort()) {}

  void Start() {
    ASSERT_EQ(pipe(control_pipe_), 0);
    pid_ = fork();
    ASSERT_GE(pid_, 0);
    if (pid_ == 0) {
      close(control_pipe_[1]);
      setpgid(0, 0);
      const pid_t api_pid = fork();
      if (api_pid == 0) {
        chdir(workspace_.c_str());
        const auto runtime_dir = (workspace_ / ".runtime").string();
        setenv("PREMIA_RUNTIME_DIR", runtime_dir.c_str(), 1);
        setenv("PREMIA_DISABLE_KEYCHAIN", "1", 1);
        const auto log_path = (workspace_ / "premia_api_test.log").string();
        const int log_fd = open(log_path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (log_fd >= 0) {
          dup2(log_fd, STDOUT_FILENO);
          dup2(log_fd, STDERR_FILENO);
          close(log_fd);
        }
        const auto port_string = std::to_string(port_);
        execl(PREMIA_API_BIN, PREMIA_API_BIN, "--host", kHost, "--port",
              port_string.c_str(), nullptr);
        _exit(127);
      }

      char buffer = 0;
      while (read(control_pipe_[0], &buffer, 1) > 0) {
      }
      kill(api_pid, SIGTERM);
      for (int attempt = 0; attempt < 20; ++attempt) {
        int status = 0;
        const auto result = waitpid(api_pid, &status, WNOHANG);
        if (result == api_pid) {
          _exit(0);
        }
        usleep(50000);
      }
      kill(api_pid, SIGKILL);
      int status = 0;
      waitpid(api_pid, &status, 0);
      _exit(0);
    }

    close(control_pipe_[0]);

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
      Stop();
    }
    fs::remove_all(workspace_);
  }

 private:
  static auto NextPort() -> int {
    static int next_port = 8300;
    return next_port++;
  }

  void Stop() {
    if (control_pipe_[1] >= 0) {
      close(control_pipe_[1]);
      control_pipe_[1] = -1;
    }
    for (int attempt = 0; attempt < 20; ++attempt) {
      int status = 0;
      const auto result = waitpid(pid_, &status, WNOHANG);
      if (result == pid_) {
        pid_ = -1;
        return;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    kill(-pid_, SIGKILL);
    int status = 0;
    waitpid(pid_, &status, 0);
    pid_ = -1;
  }

  fs::path workspace_;
  int port_ = 0;
  pid_t pid_ = -1;
  int control_pipe_[2] = {-1, -1};
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

TEST_F(ApiIntegrationFixture, UnreachableIbkrShowsDegradedBootstrapAndFallbackScreens) {
#if defined(_WIN32)
  GTEST_SKIP() << "POSIX-only process launch helper";
#endif
  WriteWorkspaceAsset(workspace_, "ibkr.json",
                      R"({"host":"127.0.0.1","port":65535,"client_id":2901,"account_id":""})");

  const auto bootstrap = HttpRequest("GET", Url("/v1/bootstrap"));
  ASSERT_EQ(bootstrap.status_code, 200);
  const auto bootstrap_tree = ParseJson(bootstrap.body);
  EXPECT_EQ(FindConnectionStatus(bootstrap_tree, "ibkr"), "degraded");

  const auto account = HttpRequest("GET", Url("/v1/screens/account"));
  ASSERT_EQ(account.status_code, 200);
  const auto account_tree = ParseJson(account.body);
  EXPECT_EQ(account_tree.get<std::string>("data.accountId"), "local_acc");

  const auto home = HttpRequest("GET", Url("/v1/screens/home"));
  ASSERT_EQ(home.status_code, 200);
  const auto home_tree = ParseJson(home.body);
  EXPECT_EQ(home_tree.get<std::string>("data.portfolio.totalValue.amount"),
            "128345.22");
  EXPECT_EQ(home_tree.get<std::string>("data.topHoldings..id"), "holding_aapl");
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

TEST_F(ApiIntegrationFixture, WatchlistPinAndMoveRoutesPersistRowState) {
#if defined(_WIN32)
  GTEST_SKIP() << "POSIX-only process launch helper";
#endif
  const auto created = HttpRequest("POST", Url("/v1/watchlists"),
                                   R"({"name":"Pinned Flow"})");
  ASSERT_EQ(created.status_code, 201);
  const auto created_tree = ParseJson(created.body);
  const auto watchlist_id = created_tree.get<std::string>("data.id");

  ASSERT_EQ(HttpRequest("POST", Url("/v1/watchlists/" + watchlist_id + "/symbols"),
                        R"({"symbol":"AAPL"})")
                .status_code,
            200);
  ASSERT_EQ(HttpRequest("POST", Url("/v1/watchlists/" + watchlist_id + "/symbols"),
                        R"({"symbol":"MSFT"})")
                .status_code,
            200);
  ASSERT_EQ(HttpRequest("PATCH",
                        Url("/v1/watchlists/" + watchlist_id + "/symbols/AAPL"),
                        R"({"pinned":true})")
                .status_code,
            200);
  ASSERT_EQ(HttpRequest("POST", Url("/v1/watchlists/" + watchlist_id + "/move"),
                        R"({"symbol":"MSFT","beforeSymbol":"AAPL"})")
                .status_code,
            200);

  const auto screen =
      HttpRequest("GET", Url("/v1/screens/watchlists/" + watchlist_id));
  ASSERT_EQ(screen.status_code, 200);
  const auto tree = ParseJson(screen.body);
  const auto& rows = tree.get_child("data.rows");
  ASSERT_EQ(std::distance(rows.begin(), rows.end()), 2);
  auto it = rows.begin();
  EXPECT_EQ(it->second.get<std::string>("symbol"), "MSFT");
  EXPECT_FALSE(it->second.get<bool>("isPinned"));
  ++it;
  EXPECT_EQ(it->second.get<std::string>("symbol"), "AAPL");
  EXPECT_TRUE(it->second.get<bool>("isPinned"));
}

TEST_F(ApiIntegrationFixture, WatchlistArchiveDeleteAndTransferRoutesWork) {
#if defined(_WIN32)
  GTEST_SKIP() << "POSIX-only process launch helper";
#endif
  const auto source = HttpRequest("POST", Url("/v1/watchlists"),
                                  R"({"name":"Source List"})");
  const auto destination = HttpRequest("POST", Url("/v1/watchlists"),
                                       R"({"name":"Destination List"})");
  ASSERT_EQ(source.status_code, 201);
  ASSERT_EQ(destination.status_code, 201);
  const auto source_id = ParseJson(source.body).get<std::string>("data.id");
  const auto destination_id =
      ParseJson(destination.body).get<std::string>("data.id");

  ASSERT_EQ(HttpRequest("POST", Url("/v1/watchlists/" + source_id + "/symbols"),
                        R"({"symbol":"NVDA"})")
                .status_code,
            200);
  ASSERT_EQ(HttpRequest("POST", Url("/v1/watchlists/" + source_id + "/transfer"),
                        std::string("{\"symbol\":\"NVDA\",\"destinationWatchlistId\":\"") +
                            destination_id + "\"}")
                .status_code,
            200);

  const auto destination_screen =
      HttpRequest("GET", Url("/v1/screens/watchlists/" + destination_id));
  ASSERT_EQ(destination_screen.status_code, 200);
  const auto destination_tree = ParseJson(destination_screen.body);
  ASSERT_FALSE(destination_tree.get_child("data.rows").empty());
  EXPECT_EQ(destination_tree.get_child("data.rows").front().second.get<std::string>("symbol"),
            "NVDA");

  ASSERT_EQ(HttpRequest("PATCH", Url("/v1/watchlists/" + destination_id + "/archive"),
                        R"({"archived":true})")
                .status_code,
            200);
  const auto watchlists = HttpRequest("GET", Url("/v1/watchlists"));
  ASSERT_EQ(watchlists.status_code, 200);
  const auto watchlists_tree = ParseJson(watchlists.body);
  bool found_archived = false;
  for (const auto& item : watchlists_tree.get_child("data.watchlists")) {
    if (item.second.get<std::string>("id") == destination_id) {
      found_archived = item.second.get<bool>("isArchived");
    }
  }
  EXPECT_TRUE(found_archived);

  ASSERT_EQ(HttpRequest("DELETE", Url("/v1/watchlists/" + destination_id)).status_code,
            200);
}

TEST_F(ApiIntegrationFixture, ChartScreenSerializesAnnotations) {
#if defined(_WIN32)
  GTEST_SKIP() << "POSIX-only process launch helper";
#endif
  const auto update = HttpRequest(
      "PUT", Url("/v1/screens/charts/AAPL/annotations"),
      R"({"annotations":[{"id":"test-note","label":"Test Note","price":"215.50","kind":"annotation"},{"id":"test-entry","label":"Entry","price":"216.00","kind":"entry"}]})");
  ASSERT_EQ(update.status_code, 200);

  const auto patch = HttpRequest(
      "PATCH", Url("/v1/screens/charts/AAPL/annotations/test-note"),
      R"({"label":"Patched Note","price":"217.25","kind":"annotation"})");
  ASSERT_EQ(patch.status_code, 200);

  const auto erase = HttpRequest(
      "DELETE", Url("/v1/screens/charts/AAPL/annotations/test-entry?accountId=local_acc"));
  ASSERT_EQ(erase.status_code, 200);

  const auto response =
      HttpRequest("GET", Url("/v1/screens/charts/AAPL?range=1M&interval=1D"));
  ASSERT_EQ(response.status_code, 200);
  const auto tree = ParseJson(response.body);
  EXPECT_FALSE(tree.get_child("data.annotations").empty());
  const auto& versions = tree.get_child("data.annotationVersions");
  EXPECT_FALSE(versions.empty());
  std::string rollback_version_id;
  for (const auto& version : versions) {
    if (version.second.get<int>("annotationCount", 0) >= 2) {
      rollback_version_id = version.second.get<std::string>("id");
      break;
    }
  }
  ASSERT_FALSE(rollback_version_id.empty());
  bool found_test_note = false;
  bool found_test_entry = false;
  for (const auto& item : tree.get_child("data.annotations")) {
    if (item.second.get<std::string>("id") == "test-note") {
      found_test_note = true;
      EXPECT_EQ(item.second.get<std::string>("label"), "Patched Note");
      EXPECT_EQ(item.second.get<std::string>("price"), "217.25");
    }
    if (item.second.get<std::string>("id") == "test-entry") {
      found_test_entry = true;
    }
  }
  EXPECT_TRUE(found_test_note);
  EXPECT_FALSE(found_test_entry);

  const auto rollback = HttpRequest(
      "POST", Url("/v1/screens/charts/AAPL/annotations/rollback"),
      std::string("{\"accountId\":\"local_acc\",\"versionId\":\"") +
          rollback_version_id + "\"}");
  ASSERT_EQ(rollback.status_code, 200);
  const auto rollback_tree = ParseJson(rollback.body);
  bool restored_entry = false;
  for (const auto& item : rollback_tree.get_child("data.annotations")) {
    if (item.second.get<std::string>("id") == "test-entry") {
      restored_entry = true;
    }
  }
  EXPECT_TRUE(restored_entry);
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

TEST_F(ApiIntegrationFixture, SeededProviderTokensDriveBootstrapStates) {
#if defined(_WIN32)
  GTEST_SKIP() << "POSIX-only process launch helper";
#endif
  WriteRuntimeProviderFile(
      workspace_, "schwab", "config.json",
      R"({"app_key":"real-looking-key","app_secret":"real-looking-secret"})");
  WriteRuntimeProviderFile(
      workspace_, "schwab", "tokens.json",
      R"({"refresh_token":"expired-refresh","refresh_token_expires_at":1})");
  WriteRuntimeProviderFile(
      workspace_, "plaid", "config.json",
      R"({"client_id":"real-looking-client-id","secret":"real-looking-secret"})");
  WriteRuntimeProviderFile(
      workspace_, "plaid", "tokens.json",
      R"({"access_token":"access-sandbox-demo"})");

  const auto bootstrap = HttpRequest("GET", Url("/v1/bootstrap"));
  ASSERT_EQ(bootstrap.status_code, 200);
  const auto bootstrap_tree = ParseJson(bootstrap.body);

  EXPECT_EQ(FindConnectionStatus(bootstrap_tree, "schwab"), "reauth_required");
  EXPECT_TRUE(FindConnectionReauthRequired(bootstrap_tree, "schwab"));
  EXPECT_EQ(FindConnectionStatus(bootstrap_tree, "plaid"), "connected");
  EXPECT_FALSE(FindConnectionReauthRequired(bootstrap_tree, "plaid"));
}

TEST_F(ApiIntegrationFixture, MultiAccountHomeReturnsFallbackAggregate) {
#if defined(_WIN32)
  GTEST_SKIP() << "POSIX-only process launch helper";
#endif
  const auto response =
      HttpRequest("GET", Url("/v1/screens/home/multi-account"));
  ASSERT_EQ(response.status_code, 200);
  const auto tree = ParseJson(response.body);

  // Must have the envelope meta fields
  EXPECT_FALSE(tree.get<std::string>("meta.requestId").empty());
  EXPECT_FALSE(tree.get<std::string>("meta.asOf").empty());

  // Aggregate fields must be present
  EXPECT_FALSE(
      tree.get<std::string>("data.aggregateNetWorth.amount").empty());
  EXPECT_FALSE(
      tree.get<std::string>("data.aggregateDayChange.absolute.amount").empty());

  // Local fallback account must be present
  const auto& accounts = tree.get_child("data.accounts");
  EXPECT_FALSE(accounts.empty());
  const auto& first = accounts.begin()->second;
  EXPECT_EQ(first.get<std::string>("provider"), "internal");
  EXPECT_EQ(first.get<std::string>("accountId"), "local_acc");
  EXPECT_EQ(first.get<std::string>("balance.amount"), "128345.22");

  // Aggregate net worth must equal the sum of account balances
  double aggregate = 0.0;
  for (const auto& item : accounts) {
    aggregate +=
        std::stod(item.second.get<std::string>("balance.amount"));
  }
  EXPECT_DOUBLE_EQ(
      std::stod(tree.get<std::string>("data.aggregateNetWorth.amount")),
      aggregate);

  // Top holdings from the local provider must be present
  EXPECT_FALSE(tree.get_child("data.topHoldings").empty());

  // Connections array must be present
  EXPECT_FALSE(tree.get_child("data.connections").empty());
}

TEST_F(ApiIntegrationFixture, OrderTemplatesCrudFlow) {
#if defined(_WIN32)
  GTEST_SKIP() << "POSIX-only process launch helper";
#endif
  // Seed an empty template store.
  WriteWorkspaceAsset(workspace_, "order_templates.json", "{\"templates\": []}");

  // List — should be empty initially.
  const auto list0 = HttpRequest("GET", Url("/v1/order-templates"));
  ASSERT_EQ(list0.status_code, 200);
  EXPECT_TRUE(ParseJson(list0.body).get_child("data.templates").empty());

  // Create a template.
  const auto create_resp = HttpRequest(
      "POST", Url("/v1/order-templates"),
      R"({"name":"Buy $500 VTI","action":"BUY","quantity":"500.00","isDollarAmount":true,"orderType":"MARKET","assetType":"EQUITY"})");
  ASSERT_EQ(create_resp.status_code, 201);
  const auto created = ParseJson(create_resp.body);
  const auto tmpl_id = created.get<std::string>("data.id");
  EXPECT_FALSE(tmpl_id.empty());
  EXPECT_EQ(created.get<std::string>("data.name"), "Buy $500 VTI");
  EXPECT_EQ(created.get<bool>("data.isDollarAmount"), true);

  // List — should have one template.
  const auto list1 = HttpRequest("GET", Url("/v1/order-templates"));
  ASSERT_EQ(list1.status_code, 200);
  EXPECT_NE(list1.body.find("Buy $500 VTI"), std::string::npos);

  // Update the template.
  const auto update_resp = HttpRequest(
      "PUT", Url("/v1/order-templates/" + tmpl_id),
      R"({"name":"Buy $1000 VTI","action":"BUY","quantity":"1000.00","isDollarAmount":true,"orderType":"MARKET","assetType":"EQUITY"})");
  ASSERT_EQ(update_resp.status_code, 200);
  const auto updated = ParseJson(update_resp.body);
  EXPECT_EQ(updated.get<std::string>("data.name"), "Buy $1000 VTI");
  EXPECT_EQ(updated.get<std::string>("data.id"), tmpl_id);

  // Delete the template.
  const auto del_resp = HttpRequest("DELETE", Url("/v1/order-templates/" + tmpl_id));
  ASSERT_EQ(del_resp.status_code, 200);
  EXPECT_EQ(ParseJson(del_resp.body).get<std::string>("data.id"), tmpl_id);

  // List — empty again.
  const auto list2 = HttpRequest("GET", Url("/v1/order-templates"));
  ASSERT_EQ(list2.status_code, 200);
  EXPECT_TRUE(ParseJson(list2.body).get_child("data.templates").empty());
}

TEST_F(ApiIntegrationFixture, QuickTradePreviewHydratesTemplateWithSymbol) {
#if defined(_WIN32)
  GTEST_SKIP() << "POSIX-only process launch helper";
#endif
  WriteWorkspaceAsset(workspace_, "order_templates.json", "{\"templates\": []}");

  // Create a dollar-amount template with no symbol binding.
  const auto create_resp = HttpRequest(
      "POST", Url("/v1/order-templates"),
      R"({"name":"Market buy $200","action":"BUY","quantity":"200.00","isDollarAmount":true,"orderType":"MARKET","assetType":"EQUITY"})");
  ASSERT_EQ(create_resp.status_code, 201);
  const auto tmpl_id = ParseJson(create_resp.body).get<std::string>("data.id");

  // Quick-trade preview against AAPL.
  const auto preview_resp = HttpRequest(
      "POST", Url("/v1/quick-trade/preview"),
      R"({"symbol":"AAPL","templateId":")" + tmpl_id + R"("})");
  ASSERT_EQ(preview_resp.status_code, 200);
  const auto preview = ParseJson(preview_resp.body);
  EXPECT_EQ(preview.get<std::string>("data.symbol"), "AAPL");
  EXPECT_EQ(preview.get<std::string>("data.instruction"), "BUY");
  EXPECT_EQ(preview.get<std::string>("data.assetType"), "EQUITY");
  EXPECT_EQ(preview.get<std::string>("data.status"), "preview");
  // Quantity should be a computed integer (200/price) — just ensure it's non-zero.
  const auto quantity_str = preview.get<std::string>("data.quantity");
  EXPECT_FALSE(quantity_str.empty());
  EXPECT_NE(quantity_str, "0");
}

TEST_F(ApiIntegrationFixture, QuickTradePreviewReturnsErrorForUnknownTemplate) {
#if defined(_WIN32)
  GTEST_SKIP() << "POSIX-only process launch helper";
#endif
  WriteWorkspaceAsset(workspace_, "order_templates.json", "{\"templates\": []}");

  const auto preview_resp = HttpRequest(
      "POST", Url("/v1/quick-trade/preview"),
      R"({"symbol":"AAPL","templateId":"tmpl_nonexistent"})");
  ASSERT_EQ(preview_resp.status_code, 400);
  const auto tree = ParseJson(preview_resp.body);
  EXPECT_EQ(tree.get<std::string>("error.code"), "INVALID_REQUEST");
}

}  // namespace premiatests::ApiIntegrationTests
