#ifndef WatchlistView_hpp
#define WatchlistView_hpp

#include "../View.hpp"
#include "../../Model/Core/WatchlistModel.hpp"

class WatchlistView: public View 
{
private:
    WatchlistModel model;
    std::unordered_map<std::string, VoidEventHandler> events;

    void initWatchlist();
    void drawWatchlistTable();
    void drawCustomWatchlistTable();

public:
    void addEvent(const std::string &, const VoidEventHandler &) override;
    void update() override;

};

#endif