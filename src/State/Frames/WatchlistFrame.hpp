#ifndef WatchlistFrame_hpp
#define WatchlistFrame_hpp

#include "Frame.hpp"

class WatchlistFrame : public Frame
{
private:
    std::string title_string;
    std::vector<tda::Watchlist> watchlists;
    std::vector<std::string> watchlist_names;
    std::vector<const char*> watchlist_names_char;

    void draw_watchlist_table();
    void draw_custom_watchlist_table();

public:
    WatchlistFrame();
    
    void init_watchlists();
    void update();
};

#endif 