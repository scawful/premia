#ifndef WatchlistFrame_hpp
#define WatchlistFrame_hpp

#include "Frame.hpp"

class WatchlistFrame : public Frame
{
private:
    std::string title_string;

public:
    WatchlistFrame();

    void update();
};

#endif 