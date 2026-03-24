#include "premia/providers/local/watchlist_provider.hpp"

#include <algorithm>
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

auto BuildSummaryList(
    const std::vector<application::WatchlistScreenData>& screens)
    -> std::vector<application::WatchlistSummary> {
  std::vector<application::WatchlistSummary> summaries;
  summaries.reserve(screens.size());
  for (const auto& screen : screens) {
    summaries.push_back(BuildSummary(screen));
  }
  return summaries;
}

auto MakeSlug(std::string value) -> std::string {
  std::string slug;
  slug.reserve(value.size());
  for (const char ch : value) {
    if ((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')) {
      slug.push_back(ch);
    } else if (ch >= 'A' && ch <= 'Z') {
      slug.push_back(static_cast<char>(ch - 'A' + 'a'));
    } else if (ch == ' ' || ch == '_' || ch == '-') {
      if (!slug.empty() && slug.back() != '-') {
        slug.push_back('-');
      }
    }
  }
  while (!slug.empty() && slug.back() == '-') {
    slug.pop_back();
  }
  return slug.empty() ? "watchlist" : slug;
}

auto MakePlaceholderRow(const std::string& watchlist_id,
                        const std::string& symbol) -> application::WatchlistRow {
  return {watchlist_id + ":" + symbol,
          symbol,
          symbol,
          MakeMoney("0.00"),
          MakeChange("0.00", "0.00"),
          MakeMoney("0.00"),
          MakeMoney("0.00"),
          "",
          false};
}

auto FindScreen(std::vector<application::WatchlistScreenData>& screens,
                const std::string& watchlist_id)
    -> std::vector<application::WatchlistScreenData>::iterator {
  return std::find_if(screens.begin(), screens.end(),
                      [&watchlist_id](const application::WatchlistScreenData& screen) {
                        return screen.watchlist.id == watchlist_id;
                      });
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

auto WatchlistProvider::CreateWatchlist(const std::string& name)
    -> application::WatchlistSummary {
  auto screens = LoadData();
  application::WatchlistScreenData screen;
  screen.watchlist.id = MakeSlug(name);
  screen.watchlist.name = name.empty() ? "Watchlist" : name;
  screen.watchlist.instrument_count = 0;
  screens.push_back(screen);

  const auto summaries = BuildSummaryList(screens);
  for (auto& item : screens) {
    item.available_watchlists = summaries;
  }
  SaveData(screens);
  return BuildSummary(screens.back());
}

auto WatchlistProvider::RenameWatchlist(const std::string& watchlist_id,
                                        const std::string& name)
    -> application::WatchlistSummary {
  auto screens = LoadData();
  for (auto& screen : screens) {
    if (screen.watchlist.id == watchlist_id) {
      screen.watchlist.name = name;
      const auto summaries = BuildSummaryList(screens);
      for (auto& item : screens) {
        item.available_watchlists = summaries;
      }
      SaveData(screens);
      return BuildSummary(screen);
    }
  }
  throw std::runtime_error("watchlist not found");
}

auto WatchlistProvider::AddWatchlistSymbol(const std::string& watchlist_id,
                                           const std::string& symbol)
    -> application::WatchlistSummary {
  auto screens = LoadData();
  const auto screen_it = FindScreen(screens, watchlist_id);
  if (screen_it != screens.end()) {
    const auto exists = std::find_if(screen_it->rows.begin(), screen_it->rows.end(),
                                     [&symbol](const application::WatchlistRow& row) {
                                       return row.symbol == symbol;
                                     });
    if (exists == screen_it->rows.end()) {
      screen_it->rows.push_back(MakePlaceholderRow(watchlist_id, symbol));
    }
    screen_it->watchlist.instrument_count = static_cast<int>(screen_it->rows.size());
    const auto summaries = BuildSummaryList(screens);
    for (auto& item : screens) {
      item.available_watchlists = summaries;
    }
    SaveData(screens);
    return BuildSummary(*screen_it);
  }
  throw std::runtime_error("watchlist not found");
}

auto WatchlistProvider::RemoveWatchlistSymbol(const std::string& watchlist_id,
                                              const std::string& symbol)
    -> application::WatchlistSummary {
  auto screens = LoadData();
  const auto screen_it = FindScreen(screens, watchlist_id);
  if (screen_it != screens.end()) {
    screen_it->rows.erase(
        std::remove_if(screen_it->rows.begin(), screen_it->rows.end(),
                       [&symbol](const application::WatchlistRow& row) {
                         return row.symbol == symbol;
                       }),
        screen_it->rows.end());
    screen_it->watchlist.instrument_count = static_cast<int>(screen_it->rows.size());
    const auto summaries = BuildSummaryList(screens);
    for (auto& item : screens) {
      item.available_watchlists = summaries;
    }
    SaveData(screens);
    return BuildSummary(*screen_it);
  }
  throw std::runtime_error("watchlist not found");
}

auto WatchlistProvider::PinWatchlistSymbol(const std::string& watchlist_id,
                                           const std::string& symbol,
                                           bool pinned)
    -> application::WatchlistSummary {
  auto screens = LoadData();
  const auto screen_it = FindScreen(screens, watchlist_id);
  if (screen_it == screens.end()) {
    throw std::runtime_error("watchlist not found");
  }

  const auto row_it = std::find_if(screen_it->rows.begin(), screen_it->rows.end(),
                                   [&symbol](const application::WatchlistRow& row) {
                                     return row.symbol == symbol;
                                   });
  if (row_it == screen_it->rows.end()) {
    throw std::runtime_error("watchlist symbol not found");
  }

  auto row = *row_it;
  row.is_pinned = pinned;
  screen_it->rows.erase(row_it);
  if (pinned) {
    const auto insert_it = std::find_if(screen_it->rows.begin(), screen_it->rows.end(),
                                        [](const application::WatchlistRow& item) {
                                          return !item.is_pinned;
                                        });
    screen_it->rows.insert(insert_it, row);
  } else {
    screen_it->rows.push_back(row);
  }

  SaveData(screens);
  return BuildSummary(*screen_it);
}

auto WatchlistProvider::MoveWatchlistSymbol(const std::string& watchlist_id,
                                            const std::string& symbol,
                                            const std::string& before_symbol)
    -> application::WatchlistSummary {
  auto screens = LoadData();
  const auto screen_it = FindScreen(screens, watchlist_id);
  if (screen_it == screens.end()) {
    throw std::runtime_error("watchlist not found");
  }

  auto& rows = screen_it->rows;
  const auto from = std::find_if(rows.begin(), rows.end(),
                                 [&symbol](const application::WatchlistRow& row) {
                                   return row.symbol == symbol;
                                 });
  const auto to = std::find_if(rows.begin(), rows.end(),
                               [&before_symbol](const application::WatchlistRow& row) {
                                 return row.symbol == before_symbol;
                               });
  if (from == rows.end() || to == rows.end()) {
    throw std::runtime_error("watchlist symbol not found");
  }

  auto row = *from;
  rows.erase(from);
  const auto insert_it = std::find_if(rows.begin(), rows.end(),
                                      [&before_symbol](const application::WatchlistRow& item) {
                                        return item.symbol == before_symbol;
                                      });
  rows.insert(insert_it, row);

  SaveData(screens);
  return BuildSummary(*screen_it);
}

auto WatchlistProvider::BuildFallbackData() const
    -> std::vector<application::WatchlistScreenData> {
  std::vector<application::WatchlistScreenData> screens;

  application::WatchlistScreenData core;
  core.watchlist = {"core", "Core", 3};
  core.rows = {
      {"row_aapl", "AAPL", "Apple Inc.", MakeMoney("217.00"),
       MakeChange("1.30", "0.60"), MakeMoney("216.95"),
       MakeMoney("217.02"), "2026-03-22T18:44:58Z", true},
      {"row_msft", "MSFT", "Microsoft Corp.", MakeMoney("420.10"),
       MakeChange("2.15", "0.51"), MakeMoney("420.00"),
       MakeMoney("420.15"), "2026-03-22T18:44:59Z", false},
      {"row_spy", "SPY", "SPDR S&P 500 ETF", MakeMoney("519.50"),
       MakeChange("3.70", "0.72"), MakeMoney("519.45"),
       MakeMoney("519.54"), "2026-03-22T18:45:00Z", false},
  };
  screens.push_back(core);

  application::WatchlistScreenData earnings;
  earnings.watchlist = {"earnings", "Earnings", 2};
  earnings.rows = {
      {"row_nvda", "NVDA", "NVIDIA Corp.", MakeMoney("915.20"),
       MakeChange("12.40", "1.37"), MakeMoney("915.10"),
       MakeMoney("915.30"), "2026-03-22T18:45:00Z", false},
      {"row_amzn", "AMZN", "Amazon.com Inc.", MakeMoney("204.80"),
       MakeChange("1.12", "0.55"), MakeMoney("204.75"),
       MakeMoney("204.84"), "2026-03-22T18:44:55Z", false},
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
             row_tree.get<std::string>("updatedAt", ""),
             row_tree.get<bool>("isPinned", false)});
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

auto WatchlistProvider::SaveData(
    const std::vector<application::WatchlistScreenData>& screens) const -> void {
  pt::ptree root;
  pt::ptree watchlists;
  for (const auto& screen : screens) {
    pt::ptree watchlist;
    watchlist.put("id", screen.watchlist.id);
    watchlist.put("name", screen.watchlist.name);

    pt::ptree rows;
    for (const auto& row : screen.rows) {
      pt::ptree row_tree;
      row_tree.put("id", row.id);
      row_tree.put("symbol", row.symbol);
      row_tree.put("name", row.name);
      row_tree.put("lastPrice", row.last_price.amount);
      row_tree.put("bid", row.bid.amount);
      row_tree.put("ask", row.ask.amount);
      row_tree.put("dayChangeAbsolute", row.day_change.absolute.amount);
      row_tree.put("dayChangePercent", row.day_change.percent);
      row_tree.put("updatedAt", row.updated_at);
      row_tree.put("isPinned", row.is_pinned);
      rows.push_back({"", row_tree});
    }

    watchlist.add_child("rows", rows);
    watchlists.push_back({"", watchlist});
  }

  root.add_child("watchlists", watchlists);
  std::ofstream file(path_);
  if (!file.good()) {
    throw std::runtime_error("unable to save watchlists");
  }
  pt::write_json(file, root);
}

}  // namespace premia::providers::local
