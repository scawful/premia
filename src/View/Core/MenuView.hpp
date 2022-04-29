#ifndef MenuView_hpp
#define MenuView_hpp

#include "../View.hpp"
#include "../../Model/Model.hpp"

class MenuView : public View
{
private:
    bool initialized = false;
    bool about = false;
    ImGuiStyle * ref;
    ConsoleLogger logger;
    std::unordered_map<std::string, VoidEventHandler> events;

    void drawStyleEditor();
    void drawFileMenu();
    void drawEditMenu();
    void drawViewMenu();
    void drawTradeMenu();
    void drawChartsMenu();
    void drawResearchMenu();
    void drawAnalyzeMenu();
    void drawAccountMenu();
    void drawDebugMenu();
    void drawHelpMenu();
    void drawScreen();

public:
    void addLogger(const ConsoleLogger & logger) override;
    void addEvent(const std::string &, const VoidEventHandler &) override;
    void update() override;
};

#endif