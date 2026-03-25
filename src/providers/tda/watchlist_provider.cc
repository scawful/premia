#include "premia/providers/tda/watchlist_provider.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "TDAmeritrade/client.h"
#include "TDAmeritrade/parser.h"

namespace premia::providers::tda {

namespace application = premia::core::application;
namespace domain = premia::core::domain;
namespace pt = boost::property_tree;

namespace {

auto MakeMoney(const std::string& amount) -> domain::Money {
  return domain::Money{amount.empty() ? "0.00" : amount, "USD"};
}

auto MakeChange(const std::string& absolute, const std::string& percent)
    -> domain::AbsolutePercentChange {
  return domain::AbsolutePercentChange{MakeMoney(absolute.empty() ? "0.00" : absolute),
                                       percent.empty() ? "0.00" : percent};
}

auto IsPlaceholderValue(const std::string& value) -> bool {
  return value.empty() || value.rfind("YOUR_", 0) == 0;
}

auto ReadConfigTree(const std::string& path, pt::ptree& tree) -> bool {
  std::ifstream file(path);
  if (!file.good()) {
    return false;
  }

  try {
    pt::read_json(file, tree);
  } catch (const std::exception&) {
    return false;
  }
  return true;
}

auto BuildSummaries(
    const std::vector<application::WatchlistScreenData>& screens)
    -> std::vector<application::WatchlistSummary> {
  std::vector<application::WatchlistSummary> summaries;
  summaries.reserve(screens.size());
  for (const auto& screen : screens) {
    application::WatchlistSummary summary;
    summary.id = screen.watchlist.id;
    summary.name = screen.watchlist.name;
    summary.instrument_count = static_cast<int>(screen.rows.size());
    summary.is_archived = screen.watchlist.is_archived;
    summaries.push_back(summary);
  }
  return summaries;
}

auto BuildAuthorizedClient(const std::string& config_path, ::premia::tda::Client& client)
    -> std::vector<std::string> {
  pt::ptree tree;
  if (!ReadConfigTree(config_path, tree)) {
    throw std::runtime_error("unable to read tda watchlist config");
  }

  const auto consumer_key = tree.get<std::string>("consumer_key", "");
  const auto refresh_token = tree.get<std::string>("refresh_token", "");
  if (IsPlaceholderValue(consumer_key) || IsPlaceholderValue(refresh_token)) {
    throw std::runtime_error("tda watchlist config unavailable");
  }

  client.addAuth(consumer_key, refresh_token);
  client.fetch_access_token();
  (void)client.get_all_accounts();
  return client.get_all_account_ids();
}

auto SplitWatchlistId(const std::string& watchlist_id)
    -> std::pair<std::string, std::string> {
  const auto separator = watchlist_id.find(':');
  if (separator == std::string::npos) {
    throw std::runtime_error("tda watchlist id missing account prefix");
  }
  return {watchlist_id.substr(0, separator), watchlist_id.substr(separator + 1)};
}

auto BuildWatchlistPayload(const application::WatchlistScreenData& screen) -> std::string {
  pt::ptree root;
  root.put("name", screen.watchlist.name);
  pt::ptree items;
  for (const auto& row : screen.rows) {
    pt::ptree item;
    pt::ptree instrument;
    instrument.put("symbol", row.symbol);
    instrument.put("assetType", "EQUITY");
    item.add_child("instrument", instrument);
    items.push_back({"", item});
  }
  root.add_child("watchlistItems", items);
  std::ostringstream output;
  pt::write_json(output, root, false);
  return output.str();
}

auto MakeOverlayMoney(const std::string& amount) -> domain::Money {
  return domain::Money{amount.empty() ? "0.00" : amount, "USD"};
}

auto MakeOverlayChange(const std::string& absolute, const std::string& percent)
    -> domain::AbsolutePercentChange {
  return domain::AbsolutePercentChange{MakeOverlayMoney(absolute), percent.empty() ? "0.00" : percent};
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

WatchlistProvider::WatchlistProvider(std::string config_path, std::string overlay_path)
    : config_path_(std::move(config_path)), overlay_path_(std::move(overlay_path)) {}

auto WatchlistProvider::HasUsableConfig() const -> bool {
  pt::ptree tree;
  if (!ReadConfigTree(config_path_, tree)) {
    return false;
  }

  const auto consumer_key = tree.get<std::string>("consumer_key", "");
  const auto refresh_token = tree.get<std::string>("refresh_token", "");
  return !IsPlaceholderValue(consumer_key) && !IsPlaceholderValue(refresh_token);
}

auto WatchlistProvider::LoadRemoteScreens() const
    -> std::vector<application::WatchlistScreenData> {
  if (!HasUsableConfig()) {
    throw std::runtime_error("tda watchlist config unavailable");
  }

  ::premia::tda::Client client;
  ::premia::tda::Parser parser;
  const auto account_ids = BuildAuthorizedClient(config_path_, client);
  if (account_ids.empty()) {
    throw std::runtime_error("tda returned no account ids");
  }

  std::vector<application::WatchlistScreenData> screens;
  for (const auto& account_id : account_ids) {
    const auto response = client.get_watchlist_by_account(account_id);
    const auto watchlists = parser.parse_watchlist_data(parser.read_response(response));

    for (std::size_t index = 0; index < watchlists.size(); ++index) {
      auto watchlist = watchlists[index];
      application::WatchlistScreenData screen;
      screen.watchlist.id = account_id + ":" + std::to_string(watchlist.getId());
      screen.watchlist.name = watchlist.getName();
      screen.watchlist.is_archived = false;

      for (int instrument_index = 0;
           instrument_index < watchlist.getNumInstruments(); ++instrument_index) {
        const auto symbol = watchlist.getInstrumentSymbol(instrument_index);
        auto quote = parser.parse_quote(parser.read_response(client.get_quote(symbol)));
        screen.rows.push_back(
            {screen.watchlist.id + ":" + symbol,
             symbol,
             watchlist.getInstrumentDescription(instrument_index).empty()
                 ? symbol
                 : watchlist.getInstrumentDescription(instrument_index),
             MakeMoney(quote.getQuoteVariable("lastPrice")),
             MakeChange(quote.getQuoteVariable("netChange"), "0.00"),
             MakeMoney(quote.getQuoteVariable("bidPrice")),
             MakeMoney(quote.getQuoteVariable("askPrice")),
             quote.getQuoteVariable("quoteTime"),
             false});
      }

      screen.watchlist.instrument_count = static_cast<int>(screen.rows.size());
      screens.push_back(screen);
    }
  }

  if (screens.empty()) {
    throw std::runtime_error("tda returned no watchlists");
  }

  const auto summaries = BuildSummaries(screens);
  for (auto& screen : screens) {
    screen.available_watchlists = summaries;
  }
  return screens;
}

auto WatchlistProvider::ListWatchlists() const
    -> std::vector<application::WatchlistSummary> {
  return BuildSummaries(LoadScreens());
}

auto WatchlistProvider::GetWatchlistScreen(const std::string& watchlist_id) const
    -> application::WatchlistScreenData {
  const auto screens = LoadScreens();
  for (const auto& screen : screens) {
    if (screen.watchlist.id == watchlist_id) {
      return screen;
    }
  }
  return screens.front();
}

auto WatchlistProvider::LoadScreens() const
    -> std::vector<application::WatchlistScreenData> {
  return SyncOverlayWithRemote(LoadRemoteScreens());
}

auto WatchlistProvider::LoadOverlayScreens() const
    -> std::vector<application::WatchlistScreenData> {
  std::ifstream file(overlay_path_);
  if (!file.good()) {
    return {};
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
      screen.watchlist.is_archived = tree.get<bool>("isArchived", false);
      for (const auto& row_node : tree.get_child("rows")) {
        const auto& row_tree = row_node.second;
        screen.rows.push_back({row_tree.get<std::string>("id", "row"),
                               row_tree.get<std::string>("symbol", ""),
                               row_tree.get<std::string>("name", ""),
                               MakeOverlayMoney(row_tree.get<std::string>("lastPrice", "0.00")),
                               MakeOverlayChange(row_tree.get<std::string>("dayChangeAbsolute", "0.00"),
                                                 row_tree.get<std::string>("dayChangePercent", "0.00")),
                               MakeOverlayMoney(row_tree.get<std::string>("bid", "0.00")),
                               MakeOverlayMoney(row_tree.get<std::string>("ask", "0.00")),
                               row_tree.get<std::string>("updatedAt", ""),
                               row_tree.get<bool>("isPinned", false)});
      }
      screen.watchlist.instrument_count = static_cast<int>(screen.rows.size());
      screens.push_back(screen);
    }
    return screens;
  } catch (const std::exception&) {
    return {};
  }
}

void WatchlistProvider::SaveOverlayScreens(
    const std::vector<application::WatchlistScreenData>& screens) const {
  pt::ptree root;
  pt::ptree watchlists;
  for (const auto& screen : screens) {
    pt::ptree watchlist;
    watchlist.put("id", screen.watchlist.id);
    watchlist.put("name", screen.watchlist.name);
    watchlist.put("isArchived", screen.watchlist.is_archived);
    pt::ptree rows;
    for (const auto& row : screen.rows) {
      pt::ptree item;
      item.put("id", row.id);
      item.put("symbol", row.symbol);
      item.put("name", row.name);
      item.put("lastPrice", row.last_price.amount);
      item.put("bid", row.bid.amount);
      item.put("ask", row.ask.amount);
      item.put("dayChangeAbsolute", row.day_change.absolute.amount);
      item.put("dayChangePercent", row.day_change.percent);
      item.put("updatedAt", row.updated_at);
      item.put("isPinned", row.is_pinned);
      rows.push_back({"", item});
    }
    watchlist.add_child("rows", rows);
    watchlists.push_back({"", watchlist});
  }
  root.add_child("watchlists", watchlists);
  std::ofstream output(overlay_path_);
  pt::write_json(output, root);
}

auto WatchlistProvider::BuildPayload(
    const application::WatchlistScreenData& screen) const -> std::string {
  return BuildWatchlistPayload(screen);
}

auto WatchlistProvider::SyncOverlayWithRemote(
    const std::vector<application::WatchlistScreenData>& remote_screens) const
    -> std::vector<application::WatchlistScreenData> {
  auto merged = remote_screens;
  const auto overlay = LoadOverlayScreens();
  for (auto& screen : merged) {
    const auto overlay_it = std::find_if(
        overlay.begin(), overlay.end(),
        [&screen](const application::WatchlistScreenData& item) {
          return item.watchlist.id == screen.watchlist.id;
        });
    if (overlay_it == overlay.end()) {
      continue;
    }
    screen.watchlist.is_archived = overlay_it->watchlist.is_archived;
    std::vector<application::WatchlistRow> ordered_rows;
    for (const auto& overlay_row : overlay_it->rows) {
      const auto remote_it = std::find_if(
          screen.rows.begin(), screen.rows.end(),
          [&overlay_row](const application::WatchlistRow& row) {
            return row.symbol == overlay_row.symbol;
          });
      if (remote_it != screen.rows.end()) {
        auto remote_row = *remote_it;
        remote_row.is_pinned = overlay_row.is_pinned;
        ordered_rows.push_back(remote_row);
      }
    }
    for (const auto& remote_row : screen.rows) {
      const auto exists = std::find_if(
          ordered_rows.begin(), ordered_rows.end(),
          [&remote_row](const application::WatchlistRow& row) {
            return row.symbol == remote_row.symbol;
          });
      if (exists == ordered_rows.end()) {
        ordered_rows.push_back(remote_row);
      }
    }
    screen.rows = ordered_rows;
  }
  const auto summaries = BuildSummaries(merged);
  for (auto& screen : merged) {
    screen.available_watchlists = summaries;
  }
  return merged;
}

auto WatchlistProvider::ResolveWatchlist(const std::string& watchlist_id) const
    -> application::WatchlistScreenData {
  auto screens = LoadScreens();
  const auto it = FindScreen(screens, watchlist_id);
  if (it == screens.end()) {
    throw std::runtime_error("watchlist not found");
  }
  return *it;
}

auto WatchlistProvider::FindAccountId() const -> std::string {
  ::premia::tda::Client client;
  const auto account_ids = BuildAuthorizedClient(config_path_, client);
  if (account_ids.empty()) {
    throw std::runtime_error("tda returned no account ids");
  }
  return account_ids.front();
}

auto WatchlistProvider::CreateWatchlist(const std::string& name)
    -> application::WatchlistSummary {
  ::premia::tda::Client client;
  const auto account_ids = BuildAuthorizedClient(config_path_, client);
  application::WatchlistScreenData screen;
  screen.watchlist.name = name;
  client.create_watchlist(account_ids.front(), BuildPayload(screen));

  const auto merged = LoadScreens();
  SaveOverlayScreens(merged);
  for (const auto& item : merged) {
    if (item.watchlist.name == name) {
      return BuildSummaries({item}).front();
    }
  }
  throw std::runtime_error("tda watchlist create did not return the new watchlist");
}

auto WatchlistProvider::RenameWatchlist(const std::string& watchlist_id,
                                        const std::string& name)
    -> application::WatchlistSummary {
  auto screen = ResolveWatchlist(watchlist_id);
  screen.watchlist.name = name;
  const auto [account_id, remote_id] = SplitWatchlistId(watchlist_id);
  ::premia::tda::Client client;
  BuildAuthorizedClient(config_path_, client);
  client.replace_watchlist(account_id, remote_id, BuildPayload(screen));
  const auto merged = LoadScreens();
  SaveOverlayScreens(merged);
  return BuildSummaries({ResolveWatchlist(watchlist_id)}).front();
}

auto WatchlistProvider::AddWatchlistSymbol(const std::string& watchlist_id,
                                           const std::string& symbol)
    -> application::WatchlistSummary {
  auto screen = ResolveWatchlist(watchlist_id);
  const auto exists = std::find_if(screen.rows.begin(), screen.rows.end(),
                                   [&symbol](const application::WatchlistRow& row) {
                                     return row.symbol == symbol;
                                   });
  if (exists == screen.rows.end()) {
    screen.rows.push_back({watchlist_id + ":" + symbol,
                           symbol,
                           symbol,
                           MakeMoney("0.00"),
                           MakeChange("0.00", "0.00"),
                           MakeMoney("0.00"),
                           MakeMoney("0.00"),
                           "",
                           false});
  }
  const auto [account_id, remote_id] = SplitWatchlistId(watchlist_id);
  ::premia::tda::Client client;
  BuildAuthorizedClient(config_path_, client);
  client.replace_watchlist(account_id, remote_id, BuildPayload(screen));
  SaveOverlayScreens(LoadScreens());
  return BuildSummaries({ResolveWatchlist(watchlist_id)}).front();
}

auto WatchlistProvider::RemoveWatchlistSymbol(const std::string& watchlist_id,
                                              const std::string& symbol)
    -> application::WatchlistSummary {
  auto screen = ResolveWatchlist(watchlist_id);
  screen.rows.erase(std::remove_if(screen.rows.begin(), screen.rows.end(),
                                   [&symbol](const application::WatchlistRow& row) {
                                     return row.symbol == symbol;
                                   }),
                    screen.rows.end());
  const auto [account_id, remote_id] = SplitWatchlistId(watchlist_id);
  ::premia::tda::Client client;
  BuildAuthorizedClient(config_path_, client);
  client.replace_watchlist(account_id, remote_id, BuildPayload(screen));
  SaveOverlayScreens(LoadScreens());
  return BuildSummaries({ResolveWatchlist(watchlist_id)}).front();
}

auto WatchlistProvider::PinWatchlistSymbol(const std::string& watchlist_id,
                                           const std::string& symbol,
                                           bool pinned)
    -> application::WatchlistSummary {
  auto screen = ResolveWatchlist(watchlist_id);
  const auto it = std::find_if(screen.rows.begin(), screen.rows.end(),
                               [&symbol](const application::WatchlistRow& row) {
                                 return row.symbol == symbol;
                               });
  if (it == screen.rows.end()) {
    throw std::runtime_error("watchlist symbol not found");
  }
  auto row = *it;
  row.is_pinned = pinned;
  screen.rows.erase(it);
  if (pinned) {
    screen.rows.insert(screen.rows.begin(), row);
  } else {
    screen.rows.push_back(row);
  }
  const auto [account_id, remote_id] = SplitWatchlistId(watchlist_id);
  ::premia::tda::Client client;
  BuildAuthorizedClient(config_path_, client);
  client.replace_watchlist(account_id, remote_id, BuildPayload(screen));
  auto refreshed = ResolveWatchlist(watchlist_id);
  for (auto& item : refreshed.rows) {
    if (item.symbol == symbol) {
      item.is_pinned = pinned;
    }
  }
  auto overlays = LoadOverlayScreens();
  auto overlay_it = FindScreen(overlays, watchlist_id);
  if (overlay_it == overlays.end()) {
    overlays.push_back(refreshed);
  } else {
    *overlay_it = refreshed;
  }
  SaveOverlayScreens(overlays);
  return BuildSummaries({refreshed}).front();
}

auto WatchlistProvider::MoveWatchlistSymbol(const std::string& watchlist_id,
                                            const std::string& symbol,
                                            const std::string& before_symbol)
    -> application::WatchlistSummary {
  auto screen = ResolveWatchlist(watchlist_id);
  const auto from = std::find_if(screen.rows.begin(), screen.rows.end(),
                                 [&symbol](const application::WatchlistRow& row) {
                                   return row.symbol == symbol;
                                 });
  const auto to = std::find_if(screen.rows.begin(), screen.rows.end(),
                               [&before_symbol](const application::WatchlistRow& row) {
                                 return row.symbol == before_symbol;
                               });
  if (from == screen.rows.end() || to == screen.rows.end()) {
    throw std::runtime_error("watchlist symbol not found");
  }
  auto row = *from;
  screen.rows.erase(from);
  const auto insert_at = std::find_if(screen.rows.begin(), screen.rows.end(),
                                      [&before_symbol](const application::WatchlistRow& item) {
                                        return item.symbol == before_symbol;
                                      });
  screen.rows.insert(insert_at, row);
  const auto [account_id, remote_id] = SplitWatchlistId(watchlist_id);
  ::premia::tda::Client client;
  BuildAuthorizedClient(config_path_, client);
  client.replace_watchlist(account_id, remote_id, BuildPayload(screen));
  auto overlays = LoadOverlayScreens();
  auto overlay_it = FindScreen(overlays, watchlist_id);
  if (overlay_it == overlays.end()) {
    overlays.push_back(screen);
  } else {
    *overlay_it = screen;
  }
  SaveOverlayScreens(overlays);
  return BuildSummaries({ResolveWatchlist(watchlist_id)}).front();
}

auto WatchlistProvider::ArchiveWatchlist(const std::string& watchlist_id, bool archived)
    -> application::WatchlistSummary {
  auto screen = ResolveWatchlist(watchlist_id);
  screen.watchlist.is_archived = archived;
  auto overlays = LoadOverlayScreens();
  auto overlay_it = FindScreen(overlays, watchlist_id);
  if (overlay_it == overlays.end()) {
    overlays.push_back(screen);
  } else {
    overlay_it->watchlist.is_archived = archived;
  }
  SaveOverlayScreens(overlays);
  return BuildSummaries({screen}).front();
}

auto WatchlistProvider::DeleteWatchlist(const std::string& watchlist_id)
    -> application::WatchlistSummary {
  const auto screen = ResolveWatchlist(watchlist_id);
  const auto [account_id, remote_id] = SplitWatchlistId(watchlist_id);
  ::premia::tda::Client client;
  BuildAuthorizedClient(config_path_, client);
  client.delete_watchlist(account_id, remote_id);
  auto overlays = LoadOverlayScreens();
  overlays.erase(std::remove_if(overlays.begin(), overlays.end(),
                                [&watchlist_id](const application::WatchlistScreenData& item) {
                                  return item.watchlist.id == watchlist_id;
                                }),
                 overlays.end());
  SaveOverlayScreens(overlays);
  return BuildSummaries({screen}).front();
}

auto WatchlistProvider::MoveSymbolToWatchlist(
    const std::string& source_watchlist_id,
    const std::string& destination_watchlist_id,
    const std::string& symbol) -> application::WatchlistSummary {
  auto source = ResolveWatchlist(source_watchlist_id);
  auto destination = ResolveWatchlist(destination_watchlist_id);
  const auto row_it = std::find_if(source.rows.begin(), source.rows.end(),
                                   [&symbol](const application::WatchlistRow& row) {
                                     return row.symbol == symbol;
                                   });
  if (row_it == source.rows.end()) {
    throw std::runtime_error("watchlist symbol not found");
  }
  auto moved = *row_it;
  moved.id = destination_watchlist_id + ":" + symbol;
  moved.is_pinned = false;
  source.rows.erase(row_it);
  destination.rows.push_back(moved);

  ::premia::tda::Client client;
  BuildAuthorizedClient(config_path_, client);
  const auto [source_account, source_remote] = SplitWatchlistId(source_watchlist_id);
  const auto [destination_account, destination_remote] = SplitWatchlistId(destination_watchlist_id);
  client.replace_watchlist(source_account, source_remote, BuildPayload(source));
  client.replace_watchlist(destination_account, destination_remote, BuildPayload(destination));
  auto overlays = LoadOverlayScreens();
  auto source_overlay = FindScreen(overlays, source_watchlist_id);
  if (source_overlay == overlays.end()) {
    overlays.push_back(source);
  } else {
    *source_overlay = source;
  }
  auto dest_overlay = FindScreen(overlays, destination_watchlist_id);
  if (dest_overlay == overlays.end()) {
    overlays.push_back(destination);
  } else {
    *dest_overlay = destination;
  }
  SaveOverlayScreens(overlays);
  return BuildSummaries({ResolveWatchlist(destination_watchlist_id)}).front();
}

}  // namespace premia::providers::tda
