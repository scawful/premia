#ifndef WatchlistView_hpp
#define WatchlistView_hpp

#include "../View.hpp"
#include "../../Model/Core/WatchlistModel.hpp"

class WatchlistView: public View 
{
private:
    WatchlistModel model;
    ConsoleLogger logger;
    std::unordered_map<std::string, VoidEventHandler> events;

    void initWatchlist();
    void drawWatchlistTable();
    void drawCustomWatchlistTable();

public:
    void addAuth(const std::string & key , const std::string & token) override;
    void addLogger(const ConsoleLogger & logger) override;
    void addEvent(const std::string &, const VoidEventHandler &) override;
    void update() override;

};

#endif