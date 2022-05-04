#ifndef WatchlistModel_hpp
#define WatchlistModel_hpp

#include "../Model.hpp"

class WatchlistModel: public Model
{
private:
    bool active = false;
    ArrayList<int> openList;
    String titleString;
    ArrayList<tda::Watchlist> watchlists;
    ArrayList<String> watchlistNames;
    ArrayList<const char*> watchlistNamesChar;
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
    ArrayList<const char *> getWatchlistNamesCharVec() const;
};

#endif 