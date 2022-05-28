#ifndef WatchlistModel_hpp
#define WatchlistModel_hpp

#include "../Model.hpp"

namespace Premia {
class WatchlistModel : public Model {
 private:  
  Logger logger;
  bool active = false;  
  ArrayList<int> openList;
  String titleString;
  ArrayList<tda::Watchlist> watchlists;
  ArrayList<String> watchlistNames;
  ArrayList<const char*> watchlistNamesChar;
  std::unordered_map<String, tda::Quote> quotes;

 public:
  bool isActive() const;
  void addLogger(const Logger& logger);

  void initWatchlist();
  void initTDAWatchlists();
  bool getOpenList(int n);
  void setOpenList(int n);
  String getWatchlistName(int index);
  tda::Quote& getQuote(CRString key);
  tda::Watchlist& getWatchlist(int index);
  void setQuote(CRString key, const tda::Quote& quote);
  ArrayList<const char*> getWatchlistNamesCharVec() const;
};
}  // namespace Premia

#endif