#ifndef PortfolioFrame_hpp
#define PortfolioFrame_hpp

class PortfolioFrame
{
private:
    ArrayList<String> account_ids_std;
    ArrayList<const char*> account_ids;
    String default_account;

    std::unordered_map<String, tda::Quote> quotes;

    tda::Account account_data;
    ArrayList<String> positions_vector;

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