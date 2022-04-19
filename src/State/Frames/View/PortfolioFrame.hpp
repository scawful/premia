#ifndef PortfolioFrame_hpp
#define PortfolioFrame_hpp

#include "../Frame.hpp"

class PortfolioFrame : public Frame
{
private:
    std::vector<std::string> account_ids_std;
    std::vector<const char*> account_ids;
    std::string default_account;

    std::unordered_map<std::string, tda::Quote> quotes;

    tda::Account account_data;
    std::vector<std::string> positions_vector;

    void draw_balance_string(const std::string & variable);

    void draw_positions();
    void draw_balances();
    void draw_orders();

    void draw_tabbed_view();

public:
    PortfolioFrame();
    
    void init_positions();
    void load_account(const std::string & account);
    void update();
};

#endif