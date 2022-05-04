#ifndef MenuView_hpp
#define MenuView_hpp

#include "Virtual/View.hpp"
#include "Model/Model.hpp"

class MenuView : public View
{
private:
    bool initialized = false;
    bool about = false;
    bool privateBalance = false;
    ImGuiStyle * ref;
    ConsoleLogger logger;
    std::unordered_map<String, EventHandler> events;
   
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
    void addLogger(const ConsoleLogger & logger) override;
    void addEvent(CRString, const EventHandler &) override;
    void update() override;
};

#endif