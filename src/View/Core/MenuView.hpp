#ifndef MenuView_hpp
#define MenuView_hpp

#include "../View.hpp"
#include "../../Model/Model.hpp"

class MenuView : public View
{
private:
    bool initialized = false;
    bool about = false;
    bool privateBalance = false;
    ImGuiStyle * ref;
    Premia::ConsoleLogger logger;
    std::unordered_map<std::string, Premia::EventHandler> events;
   
    void StyleColorsPremia(ImGuiStyle* dst = nullptr) const;
    void drawStyleEditor();
    void drawFileMenu() const;
    void drawEditMenu();
    void drawViewMenu() const;
    void drawTradeMenu() const;
    void drawChartsMenu() const;
    void drawAnalyzeMenu() const;
    void drawAccountMenu() const;
    void drawHelpMenu();
    void drawScreen();

public:
    void addAuth(String key , String token) override;
    void addLogger(const Premia::ConsoleLogger & logger) override;
    void addEvent(String, const Premia::EventHandler &) override;
    void update() override;
};

#endif