#include <ctime>
#include <filesystem>
#include <fstream>
#include <optional>

#include <gtest/gtest.h>

#include "provider_service_components.hpp"

namespace {

namespace app = premia::core::application;
namespace detail = premia::core::application::detail;

// Build a std::time_t from a calendar date (local time, midnight).
auto MakeTime(int year, int month, int day) -> std::time_t {
  std::tm tm{};
  tm.tm_year = year - 1900;
  tm.tm_mon = month - 1;
  tm.tm_mday = day;
  tm.tm_hour = 0;
  tm.tm_min = 0;
  tm.tm_sec = 0;
  tm.tm_isdst = -1;
  return std::mktime(&tm);
}

auto MakeGrant(int total_units = 100, int cliff_months = 12) -> detail::GrantConfig {
  detail::GrantConfig config;
  config.id = "test-grant";
  config.symbol = "GOOGL";
  config.grant_date = "2022-01-15";
  config.total_units = total_units;
  config.cliff_months = cliff_months;
  return config;
}

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

auto MakeWorkspace(const std::string& name) -> std::filesystem::path {
  const auto dir = std::filesystem::temp_directory_path() / name;
  std::filesystem::remove_all(dir);
  std::filesystem::create_directories(dir / "assets");
  return dir;
}

void WriteFile(const std::filesystem::path& path, const std::string& content) {
  std::ofstream output(path);
  output << content;
}

}  // namespace

namespace premiatests::RSUOverlayServiceTests {

// ── ComputeGrantVesting unit tests ──────────────────────────────────────────

TEST(ComputeGrantVesting, BeforeCliffNoUnitsVested) {
  const auto config = MakeGrant(100, 12);
  // 2022-12-31: 11.5 months after grant date, before cliff (2023-01-15)
  const auto result = detail::ComputeGrantVesting(config, MakeTime(2022, 12, 31));

  EXPECT_EQ(result.vested_units, 0);
  EXPECT_EQ(result.unvested_units, 100);
  EXPECT_DOUBLE_EQ(result.vest_progress_percent, 0.0);
  EXPECT_FALSE(result.next_vest_date.empty());
  EXPECT_EQ(result.next_vest_date, "2023-01-15");
  EXPECT_EQ(result.next_vest_units, 25);  // cliff = 25% of 100
}

TEST(ComputeGrantVesting, AtCliffExactlyCliffUnitsVested) {
  const auto config = MakeGrant(100, 12);
  // 2023-01-15: exactly at cliff
  const auto result = detail::ComputeGrantVesting(config, MakeTime(2023, 1, 15));

  EXPECT_EQ(result.vested_units, 25);
  EXPECT_EQ(result.unvested_units, 75);
  EXPECT_DOUBLE_EQ(result.vest_progress_percent, 25.0);
  // Next vest is first quarterly: 2023-04-15
  EXPECT_EQ(result.next_vest_date, "2023-04-15");
}

TEST(ComputeGrantVesting, AfterFirstQuarterlyVest) {
  const auto config = MakeGrant(100, 12);
  // 2023-04-15: cliff + 3 months
  const auto result = detail::ComputeGrantVesting(config, MakeTime(2023, 4, 15));

  // cliff(25) + 1 quarterly(6) = 31
  EXPECT_EQ(result.vested_units, 31);
  EXPECT_EQ(result.unvested_units, 69);
  EXPECT_NEAR(result.vest_progress_percent, 31.0, 0.001);
  EXPECT_EQ(result.next_vest_date, "2023-07-15");
}

TEST(ComputeGrantVesting, MidVestingTwoYearsIn) {
  const auto config = MakeGrant(100, 12);
  // 2024-01-15: cliff + 12 months = 4 quarterly events after cliff vested
  // cliff(25) + 4 quarterly(6*4=24) = 49
  const auto result = detail::ComputeGrantVesting(config, MakeTime(2024, 1, 15));

  EXPECT_EQ(result.vested_units, 49);
  EXPECT_EQ(result.unvested_units, 51);
}

TEST(ComputeGrantVesting, FullyVestedAfterFourYears) {
  const auto config = MakeGrant(100, 12);
  // 2026-01-15: grant date + 4 years, all 13 vest events passed
  const auto result = detail::ComputeGrantVesting(config, MakeTime(2026, 1, 15));

  EXPECT_EQ(result.vested_units, 100);
  EXPECT_EQ(result.unvested_units, 0);
  EXPECT_DOUBLE_EQ(result.vest_progress_percent, 100.0);
  EXPECT_TRUE(result.next_vest_date.empty());
  EXPECT_EQ(result.next_vest_units, 0);
}

TEST(ComputeGrantVesting, ScheduleHasThirteenEvents) {
  const auto config = MakeGrant(100, 12);
  const auto result = detail::ComputeGrantVesting(config, MakeTime(2022, 1, 15));

  // 1 cliff + 12 quarterly = 13 total vest events
  EXPECT_EQ(result.vest_schedule.size(), 13u);
}

TEST(ComputeGrantVesting, TotalUnitsPreservedAcrossSchedule) {
  const auto config = MakeGrant(100, 12);
  const auto result = detail::ComputeGrantVesting(config, MakeTime(2022, 1, 15));

  int total = 0;
  for (const auto& event : result.vest_schedule) {
    total += event.units;
  }
  EXPECT_EQ(total, 100);
}

TEST(ComputeGrantVesting, GrantMetadataPreserved) {
  const auto config = MakeGrant(100, 12);
  const auto result = detail::ComputeGrantVesting(config, MakeTime(2022, 1, 15));

  EXPECT_EQ(result.id, "test-grant");
  EXPECT_EQ(result.symbol, "GOOGL");
  EXPECT_EQ(result.grant_date, "2022-01-15");
  EXPECT_EQ(result.total_units, 100);
}

TEST(ComputeGrantVesting, ZeroTotalUnitsProducesZeroProgress) {
  detail::GrantConfig config;
  config.id = "empty";
  config.symbol = "GOOGL";
  config.grant_date = "2022-01-15";
  config.total_units = 0;
  config.cliff_months = 12;

  const auto result = detail::ComputeGrantVesting(config, MakeTime(2026, 1, 15));
  EXPECT_DOUBLE_EQ(result.vest_progress_percent, 0.0);
}

// ── RsuOverlayService integration test ──────────────────────────────────────

TEST(RsuOverlayService, ReturnsEmptyWhenNoGrantsFile) {
  const auto workspace = MakeWorkspace("premia-rsu-no-file");
  ScopedCurrentPath cwd(workspace);

  detail::RsuOverlayService service;
  const auto grants = service.GetGrantsWithVesting(MakeTime(2025, 1, 15));
  EXPECT_TRUE(grants.empty());
}

TEST(RsuOverlayService, ParsesGrantsFromJsonFile) {
  const auto workspace = MakeWorkspace("premia-rsu-service");
  WriteFile(workspace / "assets" / "rsu_grants.json",
            R"({
  "grants": [
    {
      "id": "googl-gsu-2022-01",
      "symbol": "GOOGL",
      "grant_date": "2022-01-15",
      "total_units": 100,
      "cliff_months": 12
    }
  ]
})");

  ScopedCurrentPath cwd(workspace);

  detail::RsuOverlayService service;
  // At 2023-01-15 the cliff should have vested (25 units)
  const auto grants = service.GetGrantsWithVesting(MakeTime(2023, 1, 15));

  ASSERT_EQ(grants.size(), 1u);
  EXPECT_EQ(grants.front().id, "googl-gsu-2022-01");
  EXPECT_EQ(grants.front().vested_units, 25);
}

TEST(RsuOverlayService, SkipsInvalidGrantEntries) {
  const auto workspace = MakeWorkspace("premia-rsu-invalid");
  WriteFile(workspace / "assets" / "rsu_grants.json",
            R"({
  "grants": [
    {
      "id": "",
      "symbol": "GOOGL",
      "grant_date": "2022-01-15",
      "total_units": 100,
      "cliff_months": 12
    },
    {
      "id": "good-grant",
      "symbol": "GOOGL",
      "grant_date": "2022-01-15",
      "total_units": 50,
      "cliff_months": 12
    }
  ]
})");

  ScopedCurrentPath cwd(workspace);

  detail::RsuOverlayService service;
  const auto grants = service.GetGrantsWithVesting(MakeTime(2025, 1, 15));

  // Only the grant with a non-empty id should be included
  ASSERT_EQ(grants.size(), 1u);
  EXPECT_EQ(grants.front().id, "good-grant");
}

}  // namespace premiatests::RSUOverlayServiceTests
