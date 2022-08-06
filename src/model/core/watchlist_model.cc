#include "watchlist_model.h"

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
  std::string account_num;
  Try { account_num = tda::TDA::getInstance().getDefaultAccount(); }
  catch (const std::out_of_range& e) {
    std::string error(e.what());
    logger("[error] " + error);
    throw premia::NotLoggedInException();
  }
  catch (const boost::property_tree::ptree_error& e) {
    std::string error(e.what());
    logger("[error] " + error);
    throw premia::NotLoggedInException();
  }
  finally {
    watchlists = tda::TDA::getInstance().getWatchlistsByAccount(account_num);

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
  Proceed;
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
