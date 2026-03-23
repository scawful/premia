#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include "premia/providers/ibkr/account_detail_provider.hpp"

namespace {

auto MakeInvalidIbkrConfig() -> std::filesystem::path {
  const auto dir = std::filesystem::temp_directory_path() / "premia-ibkr-guardrail";
  std::filesystem::remove_all(dir);
  std::filesystem::create_directories(dir);
  const auto config = dir / "ibkr.json";
  std::ofstream output(config);
  output << "{\n"
            "  \"host\": \"\",\n"
            "  \"port\": 0,\n"
            "  \"client_id\": 2901,\n"
            "  \"account_id\": \"\"\n"
            "}\n";
  return config;
}

}  // namespace

namespace premiatests::IbkrGuardrailTests {

TEST(IbkrProviders, InvalidConfigFailsFast) {
  const auto config = MakeInvalidIbkrConfig();
  premia::providers::ibkr::AccountDetailProvider provider(config.string());

  EXPECT_THROW(provider.GetAccountDetail(), std::runtime_error);
}

}  // namespace premiatests::IbkrGuardrailTests
