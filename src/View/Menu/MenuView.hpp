#ifndef MenuView_hpp
#define MenuView_hpp

#include "Virtual/View.hpp"
#include "Model/Model.hpp"

class MenuView : public View
{
private:
    // Local Variables 
    ImGuiStyle * ref = nullptr;
    bool initialized = false;
    bool about = false;

    // Premia Context Utilities 
    EventMap events;
    ConsoleLogger logger;
   
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
    String getName() override;
    void addLogger(const ConsoleLogger & logger) override;
    void addEvent(CRString, const EventHandler &) override;
    void update() override;
};

#endif