#ifndef PositionsFrame_hpp
#define PositionsFrame_hpp

#include "Frame.hpp"

class PositionsFrame : public Frame
{
private:
    std::vector<std::string> account_ids_std;
    std::vector<const char*> account_ids;
    std::string default_account;

    std::map<std::string, tda::Quote> quotes;

    tda::Account account_data;
    std::vector<std::string> positions_vector;


public:
    PositionsFrame();
    
    void init_positions();
    void load_account( std::string account );
    void update();
};

#endif