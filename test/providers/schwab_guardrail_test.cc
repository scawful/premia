#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include "premia/providers/schwab/account_detail_provider.hpp"
#include "premia/providers/schwab/options_provider.hpp"

namespace {

auto MakePlaceholderSchwabConfig() -> std::filesystem::path {
  const auto dir = std::filesystem::temp_directory_path() / "premia-schwab-guardrail";
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

}  // namespace

namespace premiatests::SchwabGuardrailTests {

TEST(SchwabProviders, PlaceholderConfigFailsFast) {
  const auto config = MakePlaceholderSchwabConfig();
  const auto tokens = config.parent_path() / "schwab_tokens.json";

  premia::providers::schwab::AccountDetailProvider account(config.string(),
                                                           tokens.string());
  premia::providers::schwab::OptionsProvider options(config.string(),
                                                     tokens.string());

  EXPECT_THROW(account.GetAccountDetail(), std::runtime_error);
  EXPECT_THROW(options.GetOptionChainSnapshot("SPY", "8", "SINGLE", "ALL", "ALL",
                                              "ALL"),
               std::runtime_error);
}

}  // namespace premiatests::SchwabGuardrailTests
