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
    Logger logger;
   
    void drawFileMenu() const;
    void drawTradeMenu() const;
    void drawChartsMenu() const;
    void drawAnalyzeMenu() const;
    void drawColumnOptions(int x) const;
    void drawViewMenu() const;
    void drawHelpMenu();
    void drawScreen();

public:
    String getName() override;
    void addLogger(const Logger& logger) override;
    void addEvent(CRString, const EventHandler &) override;
    void update() override;
};

#endif