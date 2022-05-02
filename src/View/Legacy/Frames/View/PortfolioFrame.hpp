#ifndef PortfolioFrame_hpp
#define PortfolioFrame_hpp

class PortfolioFrame
{
private:
    std::vector<String> account_ids_std;
    std::vector<const char*> account_ids;
    String default_account;

    std::unordered_map<String, tda::Quote> quotes;

    tda::Account account_data;
    std::vector<String> positions_vector;

    void draw_balance_string(CRString variable);

    void draw_positions();
    void draw_balances();
    void draw_orders();

    void draw_tabbed_view();

public:
    PortfolioFrame();
    
    void init_positions();
    void load_account(CRString account);
    void update();
};

#endif