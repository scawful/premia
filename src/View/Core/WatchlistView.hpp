#ifndef WatchlistView_hpp
#define WatchlistView_hpp

#include "../View.hpp"
#include "../../Model/Core/WatchlistModel.hpp"

class WatchlistView: public View 
{
private:
    WatchlistModel model;
    Premia::ConsoleLogger logger;
    std::unordered_map<std::string, Premia::EventHandler> events;

    void initWatchlist();
    void drawWatchlistTable();
    void drawCustomWatchlistTable();

public:
    void addAuth(String key , String token) override;
    void addLogger(const Premia::ConsoleLogger & logger) override;
    void addEvent(String, const Premia::EventHandler &) override;
    void update() override;

};

#endif