#ifndef WatchlistView_hpp
#define WatchlistView_hpp

#include "Virtual/View.hpp"
#include "Interface/TDA.hpp"
#include "Model/Core/WatchlistModel.hpp"


class WatchlistView: public View 
{
private:
    bool isLoggedIn = false;
    bool isInit = false;
    EventMap events;
    ConsoleLogger logger;
    WatchlistModel model;

    void initWatchlist();
    void drawWatchlistTable();

public:
    void addLogger(const ConsoleLogger & logger) override;
    void addEvent(CRString, const EventHandler &) override;
    void update() override;

private:

    ImGuiTableFlags watchlistFlags = ImGuiTableFlags_ScrollY      | 
                                     ImGuiTableFlags_Sortable     | 
                                     ImGuiTableFlags_RowBg        | 
                                     ImGuiTableFlags_BordersOuter | 
                                     ImGuiTableFlags_BordersV     | 
                                     ImGuiTableFlags_Resizable    | 
                                     ImGuiTableFlags_Reorderable  | 
                                     ImGuiTableFlags_Hideable;

};

#endif