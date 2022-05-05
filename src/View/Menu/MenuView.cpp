#include "MenuView.hpp"

void MenuView::drawFileMenu() const
{
    if (ImGui::BeginMenu(ICON_MD_DASHBOARD)) {
        if (ImGui::MenuItem("Home"))
            events.at("goHome")();
        ImGui::MenuItem("New Workspace", "CTRL + N");
        ImGui::Separator();
        ImGui::MenuItem("Open Workspace", "CTRL + O");
        if (ImGui::BeginMenu("Open Recent")) {
            ImGui::MenuItem("None");
            ImGui::EndMenu();
        }
        ImGui::Separator();
        ImGui::MenuItem("Save Workspace", "CTRL + S");
        ImGui::Separator();
        if (ImGui::BeginMenu("Preferences")) {
            static bool privateBalance = false;
            if (ImGui::MenuItem("Private Balances", "", &privateBalance)) {
                halext::HLXT::getInstance().setPrivateBalance(privateBalance);
            }
            ImGui::EndMenu();
        }

        if ( ImGui::MenuItem("Quit", "ESC") ) {
            events.at("quit")();
        }

        ImGui::EndMenu();
    }
}

void MenuView::drawViewMenu() const
{
    static bool show_imgui_metrics       = false;
    static bool show_implot_metrics      = false;
    static bool show_imgui_style_editor  = false;
    static bool show_implot_style_editor = false;
    if (show_imgui_metrics) {
        ImGui::ShowMetricsWindow(&show_imgui_metrics);
    }
    if (show_implot_metrics) {
        ImPlot::ShowMetricsWindow(&show_implot_metrics);
    }
    if (show_imgui_style_editor) {
        ImGui::Begin("Style Editor (ImGui)", &show_imgui_style_editor);
        ImGui::ShowStyleEditor();
        ImGui::End();
    }
    if (show_implot_style_editor) {
        ImGui::SetNextWindowSize(ImVec2(415,762), ImGuiCond_Appearing);
        ImGui::Begin("Style Editor (ImPlot)", &show_implot_style_editor);
        ImPlot::ShowStyleEditor();
        ImGui::End();
    }

    if (ImGui::BeginMenu(ICON_MD_TUNE))
    {         
        ImGui::MenuItem("Console");
        ImGui::MenuItem("Watchlists");

        ImGui::Separator();
        if (ImGui::BeginMenu("Appearance")) {
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window Layout")) {
            ImGui::EndMenu();
        }
        ImGui::Separator();
        if (ImGui::BeginMenu("GUI Tools")) {
            ImGui::MenuItem("Metrics (ImGui)",       nullptr, &show_imgui_metrics);
            ImGui::MenuItem("Metrics (ImPlot)",      nullptr, &show_implot_metrics);
            ImGui::MenuItem("Style Editor (ImGui)",  nullptr, &show_imgui_style_editor);
            ImGui::MenuItem("Style Editor (ImPlot)", nullptr, &show_implot_style_editor);
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
}

void MenuView::drawTradeMenu() const
{
    if (ImGui::BeginMenu(ICON_MD_SYNC_ALT))
    {
        ImGui::MenuItem("Place Order", "N/A");
        ImGui::MenuItem("Replace Order", "N/A");
        ImGui::MenuItem("Cancel Order", "N/A");
        ImGui::Separator();
        ImGui::MenuItem("Get Order", "N/A");
        ImGui::Separator();
        if (ImGui::MenuItem("Option Chain")) {
            events.at("optionChainView")();
        }
        ImGui::EndMenu();
    }
}

void MenuView::drawChartsMenu() const
{
    if (ImGui::BeginMenu(ICON_MD_ADD_CHART)) {
        if (ImGui::MenuItem("Line Plot", ICON_MD_SHOW_CHART)) {
            events.at("linePlotView")();
        }
        if (ImGui::MenuItem("Candlestick", ICON_MD_CANDLESTICK_CHART)) {
            events.at("chartView")();
        } 

        ImGui::EndMenu();
    }
}

void MenuView::drawAnalyzeMenu() const
{
    if (ImGui::BeginMenu(ICON_MD_TOPIC))
    {
        ImGui::MenuItem("Fundamentals", "N/A");
        ImGui::MenuItem("Market Movers", "N/A");
        ImGui::Separator();
        ImGui::MenuItem("Insider Roster", "PRO");
        ImGui::MenuItem("Insider Summary", "PRO");
        ImGui::MenuItem("Insider Transactions", "PRO");
        ImGui::MenuItem("Fund Ownership", "PRO");
        ImGui::Separator();
        ImGui::MenuItem("Retail Money Funds", "PRO");
        ImGui::MenuItem("Institutional Money Funds", "PRO");
        ImGui::MenuItem("Institutional Ownership", "PRO");
        ImGui::Separator();
        ImGui::MenuItem("Largest Trades", "PRO");
        ImGui::MenuItem("Market Volume (U.S.)", "PRO");
        ImGui::Separator();
        ImGui::MenuItem("Daily Treasury Rates", "PRO");
        ImGui::MenuItem("Federal Funds Rate", "PRO");
        ImGui::MenuItem("Unemployment Rate", "PRO");
        ImGui::MenuItem("US Recession Probabilities", "PRO");
        ImGui::Separator();
        ImGui::MenuItem("Consumer Price Index", "PRO");
        ImGui::MenuItem("Industrial Production Index", "PRO");
        ImGui::Separator();
        ImGui::EndMenu();
    }
}

void MenuView::drawAccountMenu() const
{
    if (ImGui::BeginMenu(ICON_MD_ACCOUNT_BALANCE)) {
        ImGui::MenuItem("Sync Data", "N/A");
        ImGui::Separator();
        ImGui::MenuItem("General Settings", "N/A");
        ImGui::MenuItem("Graphical Settings", "N/A");
        ImGui::Separator();
        ImGui::MenuItem("Activate Trial", "N/A");
        ImGui::EndMenu();
    }
}

void MenuView::drawHelpMenu()
{
    if ( ImGui::BeginMenu(ICON_MD_HELP) )
    {
        ImGui::MenuItem("Get Started");
        ImGui::MenuItem("Tips and Tricks");
        if (ImGui::MenuItem("About"))
            about = true;

        ImGui::EndMenu();
    }
}

void MenuView::drawScreen()
{
    if (ImGui::BeginMenuBar()) {
        drawFileMenu();
        drawTradeMenu();
        drawChartsMenu();
        drawAnalyzeMenu();
        drawAccountMenu();
        drawViewMenu();
        drawHelpMenu();
        ImGui::EndMenuBar();
    }

    if (about)
        ImGui::OpenPopup("About");

    if (ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize )) {
        ImGui::Text("Premia Version 0.4");
        ImGui::Text("Written by: Justin Scofield (scawful)");
        ImGui::Text("Dependencies: Boost, SDL2, ImGui, ImPlot");
        ImGui::Text("Data provided by: TDAmeritrade, CoinbasePro");

        if (ImGui::Button("Close", ImVec2(120, 0))) { 
            about = false; 
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::EndPopup();
    }
}

String 
MenuView::getName() {
    return "Menu";
}

void MenuView::addLogger(const ConsoleLogger & newLogger)
{
    this->logger = newLogger;
}

void MenuView::addEvent(CRString key, const EventHandler & event)
{
    this->events[key] = event;
}

void MenuView::update() 
{
    drawScreen();
}
