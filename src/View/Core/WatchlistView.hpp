#ifndef WatchlistView_hpp
#define WatchlistView_hpp

#include "../View.hpp"
#include "../../Model/Core/WatchlistModel.hpp"

class WatchlistView: public View 
{
private:
    WatchlistModel model;
    Premia::ConsoleLogger logger;
    std::unordered_map<String, Premia::EventHandler> events;

    void initWatchlist();
    void drawWatchlistTable();
    void drawCustomWatchlistTable();

public:
    void addAuth(CRString key , CRString token) override;
    void addLogger(const Premia::ConsoleLogger & logger) override;
    void addEvent(CRString, const Premia::EventHandler &) override;
    void update() override;

};

#endif