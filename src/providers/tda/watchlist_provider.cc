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
    summaries.push_back(summary);
  }
  return summaries;
}

}  // namespace

WatchlistProvider::WatchlistProvider(std::string config_path)
    : config_path_(std::move(config_path)) {}

auto WatchlistProvider::HasUsableConfig() const -> bool {
  pt::ptree tree;
  if (!ReadConfigTree(config_path_, tree)) {
    return false;
  }

  const auto consumer_key = tree.get<std::string>("consumer_key", "");
  const auto refresh_token = tree.get<std::string>("refresh_token", "");
  return !IsPlaceholderValue(consumer_key) && !IsPlaceholderValue(refresh_token);
}

auto WatchlistProvider::LoadScreens() const
    -> std::vector<application::WatchlistScreenData> {
  if (!HasUsableConfig()) {
    throw std::runtime_error("tda watchlist config unavailable");
  }

  pt::ptree tree;
  if (!ReadConfigTree(config_path_, tree)) {
    throw std::runtime_error("unable to read tda watchlist config");
  }

  ::premia::tda::Client client;
  ::premia::tda::Parser parser;
  client.addAuth(tree.get<std::string>("consumer_key"),
                 tree.get<std::string>("refresh_token"));
  client.fetch_access_token();
  (void)client.get_all_accounts();

  const auto account_ids = client.get_all_account_ids();
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
      screen.watchlist.id = account_id + ":" + std::to_string(index);
      screen.watchlist.name = watchlist.getName();

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

}  // namespace premia::providers::tda
