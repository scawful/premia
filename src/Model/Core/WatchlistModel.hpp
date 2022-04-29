#ifndef WatchlistModel_hpp
#define WatchlistModel_hpp

#include "../Model.hpp"

class WatchlistModel: public Model
{
private:
    bool active = false;
    std::vector<int> openList;
    std::string titleString;
    std::vector<tda::Watchlist> watchlists;
    std::vector<std::string> watchlistNames;
    std::vector<const char*> watchlistNamesChar;
    std::unordered_map<std::string, tda::Quote> quotes;


public:
    bool isActive() const;
    
    void initWatchlist();
    bool getOpenList(int n);
    void setOpenList(int n);
    std::string getWatchlistName(int index);
    tda::Quote & getQuote(std::string key);
    tda::Watchlist & getWatchlist(int index);
    void setQuote(std::string key, tda::Quote quote);
    std::vector<const char *> getWatchlistNamesCharVec();
};

#endif 