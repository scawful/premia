#ifndef WatchlistView_hpp
#define WatchlistView_hpp

#include "View.hpp"
#include "Interface/TDA.hpp"
#include "Model/Core/WatchlistModel.hpp"


class WatchlistView: public View 
{
private:
    WatchlistModel model;
    ConsoleLogger logger;
    std::unordered_map<String, EventHandler> events;

    void initWatchlist();
    void drawWatchlistTable();
    void drawCustomWatchlistTable();

public:
    void addLogger(const ConsoleLogger & logger) override;
    void addEvent(CRString, const EventHandler &) override;
    void update() override;

};

#endif