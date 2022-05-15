#ifndef AccountView_hpp
#define AccountView_hpp

#include "Virtual/View.hpp"
#include "Model/Options/OptionsModel.hpp"
#include "Interface/HLXT.hpp"

class AccountView: public View 
{
private:
    bool isInit = false;
    bool isLoggedIn = false;
    EventMap events;
    Logger logger;
    // -----------------------------
    ArrayList<String> account_ids_std;
    ArrayList<const char*> account_ids;
    String default_account;

    std::unordered_map<String, tda::Quote> quotes;

    tda::Account account_data;
    ArrayList<String> positions_vector;

    void initPositions();    
    void load_account(CRString account);
    void load_all_accounts();
    void draw_symbol_string(CRString symbol);
    void draw_balance_string(CRString variable);
    void draw_positions();

    // ------------------------------
    void drawAccountPane();

public:
    String getName() override;
    void addLogger(const Logger& logger) override;
    void addEvent(CRString key, const EventHandler & event) override;
    void update() override;

private:
    ImGuiTableFlags positionFlags = ImGuiTableFlags_ScrollY      | 
                                    ImGuiTableFlags_Sortable     | 
                                    ImGuiTableFlags_RowBg        | 
                                    ImGuiTableFlags_BordersOuter | 
                                    ImGuiTableFlags_BordersV     | 
                                    ImGuiTableFlags_Resizable    | 
                                    ImGuiTableFlags_Reorderable  | 
                                    ImGuiTableFlags_Hideable;

};

#endif