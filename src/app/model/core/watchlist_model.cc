#include "watchlist_model.h"

#include "premia/core/application/composition_root.hpp"

namespace premia {

void WatchlistModel::resetWatchlist() {
  watchlists = std::vector<tda::Watchlist>();
  openList = std::vector<int>();
  watchlistNames = std::vector<std::string>();
  watchlistNamesChar = std::vector<const char*>();
}

void WatchlistModel::initLocalWatchlist() {
  // TODO: load file in (preferably some xml or json)
  // Parse into openList<int> (binary list of which entries are unloaded (0) or loaded (1)),
  //   watchlists<TDA:Watchlist> (Watchlist data)

  // For now, just populate with blank entries
  resetWatchlist();
  tda::Watchlist watchlist; // Initialze test watchlist
  watchlist.setName("Local watchlist");
  watchlist.setId(0);
  watchlist.setAccountId("local_acc");
  watchlist.addInstrument("AAPL", "", "Stock"); // Add  test entry
  watchlists.push_back(watchlist); // Add test watchlist to watchlists


  for (int i = 0; i < watchlists.size(); i++) {
    watchlistNames.push_back(watchlists[i].getName());
    openList.push_back(0);
  }

  watchlistNamesChar.clear();
  for (std::string const& str : watchlistNames) {
    watchlistNamesChar.push_back(str.data());
  }

  active = true;
}

void WatchlistModel::initTDAWatchlists() {
  resetWatchlist();
  try {
    auto& service = core::application::CompositionRoot::Instance().Watchlists();
    const auto summaries = service.ListWatchlists();
    for (size_t i = 0; i < summaries.size(); ++i) {
      auto screen = service.GetWatchlistScreen(summaries[i].id);
      tda::Watchlist watchlist;
      watchlist.setName(screen.watchlist.name);
      watchlist.setId(static_cast<int>(i));
      watchlist.setAccountId(screen.watchlist.id);

      for (const auto& row : screen.rows) {
        watchlist.addInstrument(row.symbol, row.name, "Stock");
        tda::Quote quote;
        quote.setQuoteVariable("bidPrice", row.bid.amount);
        quote.setQuoteVariable("askPrice", row.ask.amount);
        quote.setQuoteVariable("openPrice", row.last_price.amount);
        quote.setQuoteVariable("closePrice", row.last_price.amount);
        quotes[row.symbol] = quote;
      }

      watchlists.push_back(watchlist);
      watchlistNames.push_back(watchlist.getName());
      openList.push_back(1);
    }

    for (const std::string& str : watchlistNames) {
      watchlistNamesChar.push_back(str.c_str());
    }
    active = !watchlists.empty();
  } catch (const std::exception& e) {
    logger(std::string("[error] ") + e.what());
    throw premia::NotLoggedInException();
  }
}

bool WatchlistModel::getOpenList(int n) { return openList.at(n); }

void WatchlistModel::setOpenList(int n) { this->openList[n] = 1; }
void WatchlistModel::setOpenList(int n, int m) { this->openList[n] = m; }

bool WatchlistModel::isActive() const { return active; }

void WatchlistModel::addLogger(const Logger& log) { this->logger = log; }

std::string WatchlistModel::getWatchlistName(int index) {
  return watchlistNames.at(index);
}

tda::Quote& WatchlistModel::getQuote(const std::string &key) { return quotes.at(key); }

tda::Watchlist& WatchlistModel::getWatchlist(int index) {
  return watchlists.at(index);
}

void WatchlistModel::setQuote(const std::string &key, const tda::Quote& quote) {
  this->quotes[key] = quote;
}

std::vector<const char*> WatchlistModel::getWatchlistNamesCharVec() const {
  return watchlistNamesChar;
}
}  // namespace premia
