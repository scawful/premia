#include "premia/providers/local/watchlist_provider.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace premia::providers::local {

namespace application = premia::core::application;
namespace pt = boost::property_tree;

namespace {

auto MakeMoney(const std::string& amount) -> core::domain::Money {
  return core::domain::Money{amount, "USD"};
}

auto MakeChange(const std::string& absolute, const std::string& percent)
    -> core::domain::AbsolutePercentChange {
  return core::domain::AbsolutePercentChange{MakeMoney(absolute), percent};
}

auto BuildSummary(const application::WatchlistScreenData& screen)
    -> application::WatchlistSummary {
  application::WatchlistSummary summary;
  summary.id = screen.watchlist.id;
  summary.name = screen.watchlist.name;
  summary.instrument_count = static_cast<int>(screen.rows.size());
  return summary;
}

}  // namespace

WatchlistProvider::WatchlistProvider(std::string path) : path_(std::move(path)) {}

auto WatchlistProvider::ListWatchlists() const
    -> std::vector<application::WatchlistSummary> {
  const auto screens = LoadData();
  std::vector<application::WatchlistSummary> summaries;
  summaries.reserve(screens.size());
  for (const auto& screen : screens) {
    summaries.push_back(BuildSummary(screen));
  }
  return summaries;
}

auto WatchlistProvider::GetWatchlistScreen(const std::string& watchlist_id) const
    -> application::WatchlistScreenData {
  const auto screens = LoadData();
  if (screens.empty()) {
    return {};
  }

  for (const auto& screen : screens) {
    if (screen.watchlist.id == watchlist_id) {
      return screen;
    }
  }

  return screens.front();
}

auto WatchlistProvider::BuildFallbackData() const
    -> std::vector<application::WatchlistScreenData> {
  std::vector<application::WatchlistScreenData> screens;

  application::WatchlistScreenData core;
  core.watchlist = {"core", "Core", 3};
  core.rows = {
      {"row_aapl", "AAPL", "Apple Inc.", MakeMoney("217.00"),
       MakeChange("1.30", "0.60"), MakeMoney("216.95"),
       MakeMoney("217.02"), "2026-03-22T18:44:58Z"},
      {"row_msft", "MSFT", "Microsoft Corp.", MakeMoney("420.10"),
       MakeChange("2.15", "0.51"), MakeMoney("420.00"),
       MakeMoney("420.15"), "2026-03-22T18:44:59Z"},
      {"row_spy", "SPY", "SPDR S&P 500 ETF", MakeMoney("519.50"),
       MakeChange("3.70", "0.72"), MakeMoney("519.45"),
       MakeMoney("519.54"), "2026-03-22T18:45:00Z"},
  };
  screens.push_back(core);

  application::WatchlistScreenData earnings;
  earnings.watchlist = {"earnings", "Earnings", 2};
  earnings.rows = {
      {"row_nvda", "NVDA", "NVIDIA Corp.", MakeMoney("915.20"),
       MakeChange("12.40", "1.37"), MakeMoney("915.10"),
       MakeMoney("915.30"), "2026-03-22T18:45:00Z"},
      {"row_amzn", "AMZN", "Amazon.com Inc.", MakeMoney("204.80"),
       MakeChange("1.12", "0.55"), MakeMoney("204.75"),
       MakeMoney("204.84"), "2026-03-22T18:44:55Z"},
  };
  screens.push_back(earnings);

  std::vector<application::WatchlistSummary> summaries;
  summaries.reserve(screens.size());
  for (const auto& screen : screens) {
    summaries.push_back(BuildSummary(screen));
  }
  for (auto& screen : screens) {
    screen.available_watchlists = summaries;
  }

  return screens;
}

auto WatchlistProvider::LoadData() const
    -> std::vector<application::WatchlistScreenData> {
  std::ifstream file(path_);
  if (!file.good()) {
    return BuildFallbackData();
  }

  try {
    pt::ptree root;
    pt::read_json(file, root);

    std::vector<application::WatchlistScreenData> screens;
    for (const auto& watchlist_node : root.get_child("watchlists")) {
      application::WatchlistScreenData screen;
      const auto& tree = watchlist_node.second;
      screen.watchlist.id = tree.get<std::string>("id", "watchlist");
      screen.watchlist.name = tree.get<std::string>("name", "Watchlist");
      screen.rows.clear();

      for (const auto& row_node : tree.get_child("rows")) {
        const auto& row_tree = row_node.second;
        screen.rows.push_back(
            {row_tree.get<std::string>("id", "row"),
             row_tree.get<std::string>("symbol", ""),
             row_tree.get<std::string>("name", ""),
             MakeMoney(row_tree.get<std::string>("lastPrice", "0.00")),
             MakeChange(row_tree.get<std::string>("dayChangeAbsolute", "0.00"),
                        row_tree.get<std::string>("dayChangePercent", "0.00")),
             MakeMoney(row_tree.get<std::string>("bid", "0.00")),
             MakeMoney(row_tree.get<std::string>("ask", "0.00")),
             row_tree.get<std::string>("updatedAt", "")});
      }

      screen.watchlist.instrument_count = static_cast<int>(screen.rows.size());
      screens.push_back(screen);
    }

    if (screens.empty()) {
      return BuildFallbackData();
    }

    std::vector<application::WatchlistSummary> summaries;
    summaries.reserve(screens.size());
    for (const auto& screen : screens) {
      summaries.push_back(BuildSummary(screen));
    }
    for (auto& screen : screens) {
      screen.available_watchlists = summaries;
    }
    return screens;
  } catch (const std::exception&) {
    return BuildFallbackData();
  }
}

}  // namespace premia::providers::local
