#ifndef WatchlistModel_hpp
#define WatchlistModel_hpp

#include "model/model.h"
#include "service/TDAmeritrade/Data/Watchlist.hpp"

namespace premia {
class WatchlistModel : public Model {
 private:  
  Logger logger;
  bool active = false;  
  String titleString;
  ArrayList<int> openList;
  ArrayList<tda::Watchlist> watchlists;
  ArrayList<String> watchlistNames;
  ArrayList<const char*> watchlistNamesChar;
  std::unordered_map<String, tda::Quote> quotes;

 public:
  bool isActive() const;
  void addLogger(const Logger& logger);

  void resetWatchlist();
  void initLocalWatchlist();
  void initTDAWatchlists();
  void saveWatchlists(); //TODO: Some method that'll save to disk current watchlist state
  bool getOpenList(int n);
  void setOpenList(int n);
  void setOpenList(int n, int m);
  String getWatchlistName(int index);
  tda::Quote& getQuote(CRString key);
  tda::Watchlist& getWatchlist(int index);
  void setQuote(CRString key, const tda::Quote& quote);
  ArrayList<const char*> getWatchlistNamesCharVec() const;
};
}  // namespace premia

#endif
