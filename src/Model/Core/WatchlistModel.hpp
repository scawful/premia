#ifndef WatchlistModel_hpp
#define WatchlistModel_hpp

#include "../Model.hpp"

class WatchlistModel: public Model
{
private:
    bool active = false;
    std::vector<int> openList;
    String titleString;
    std::vector<tda::Watchlist> watchlists;
    std::vector<String> watchlistNames;
    std::vector<const char*> watchlistNamesChar;
    ConsoleLogger logger;
    std::unordered_map<String, tda::Quote> quotes;


public:
    bool isActive() const;
    
    void addLogger(const ConsoleLogger & logger);
    void initWatchlist();
    bool getOpenList(int n);
    void setOpenList(int n);
    String getWatchlistName(int index);
    tda::Quote & getQuote(CRString key);
    tda::Watchlist & getWatchlist(int index);
    void setQuote(CRString key, const tda::Quote & quote);
    std::vector<const char *> getWatchlistNamesCharVec() const;
};

#endif 