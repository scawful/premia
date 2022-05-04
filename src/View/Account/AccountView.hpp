#ifndef AccountView_hpp
#define AccountView_hpp

#include "Virtual/View.hpp"
#include "Model/Options/OptionsModel.hpp"
#include "Interface/HLXT.hpp"

class AccountView: public View 
{
private:
    bool isInit = false;
    EventMap events;
    ConsoleLogger logger;
    // -----------------------------
    ArrayList<String> account_ids_std;
    ArrayList<const char*> account_ids;
    String default_account;

    std::unordered_map<String, tda::Quote> quotes;

    tda::Account account_data;
    ArrayList<String> positions_vector;

    void initPositions();    
    void load_account(CRString account);
    void draw_balance_string(CRString variable);
    void draw_positions();

    // ------------------------------
    void drawAccountPane();

public:
    String getName() override;
    void addLogger(const ConsoleLogger & logger) override;
    void addEvent(CRString key, const EventHandler & event) override;
    void update() override;

};

#endif