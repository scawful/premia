#ifndef WatchlistModel_hpp
#define WatchlistModel_hpp

#include <string>
#include <vector>

#include "model/model.h"
#include "service/TDAmeritrade/data/Watchlist.hpp"

namespace premia {
class WatchlistModel : public Model {
 private:
  Logger logger;
  bool active = false;
  std::string titleString;
  std::vector<int> openList;
  std::vector<tda::Watchlist> watchlists;
  std::vector<std::string> watchlistNames;
  std::vector<const char*> watchlistNamesChar;
  std::unordered_map<std::string, tda::Quote> quotes;

 public:
  bool isActive() const;
  void addLogger(const Logger& logger);

  void resetWatchlist();
  void initLocalWatchlist();
  void initTDAWatchlists();
  void saveWatchlists();  // TODO: Some method that'll save to disk current
                          // watchlist state
  bool getOpenList(int n);
  void setOpenList(int n);
  void setOpenList(int n, int m);
  std::string getWatchlistName(int index);
  tda::Quote& getQuote(const std::string& key);
  tda::Watchlist& getWatchlist(int index);
  void setQuote(const std::string& key, const tda::Quote& quote);
  std::vector<const char*> getWatchlistNamesCharVec() const;
};
}  // namespace premia

#endif
