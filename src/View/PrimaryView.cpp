#include "PrimaryView.hpp"


void PrimaryView::drawInfoPane()
{
    ImGui::Text("General");
    if (ImGui::TreeNode("Getting Started")) {
        ImGui::Separator();
        ImGui::Text("Welcome to Premia!");
        ImGui::Separator();
        ImGui::TreePop();
    }
    
    if (ImGui::TreeNode("Linking Accounts")) {
        ImGui::Separator(); ImGui::TextColored(ImVec4(42.f, 170.f, 138.f, 1.0f), "TDAmeritrade"); ImGui::Separator();            
        ImGui::TextColored(ImVec4(0.f, 128.f, 128.f, 1.0f), "Coinbase"); ImGui::Separator();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Premia Pro Subscription")) {
        ImGui::TreePop();
    }


    ImGui::Text("Education");
    if (ImGui::TreeNode("Delta Hedging")) {
        ImGui::Separator();
        ImGui::Text("Delta is an important hedge parameter for an options dealers portfolio.");
        ImGui::TextWrapped("Options traders adjust delta frequently, making it close to zero, by trading the underlying asset.");
        ImGui::TextWrapped("The Practioner Black-Scholes Delta is the partial derivative of the option price with respect to the underlying asset price with other variables, including the implied volatility, kept constant.");
        ImGui::Separator();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Gamma Exposure")) {
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Standardizing Volatility")) {
        ImGui::Separator(); ImGui::Text("Average True Range"); ImGui::Separator();
        ImGui::TextWrapped("One of the most common ways to measure volatility is in terms of the average true range of a trading session.");

        ImGui::Separator(); ImGui::Text("Historical Volatility"); ImGui::Separator();
        ImGui::TextWrapped("Statistical or realized volatility.");

        ImGui::Separator(); ImGui::Text("Sigma Spike"); ImGui::Separator();
        ImGui::TextWrapped("Standard deviation of the past 20-day return.");
        ImGui::BulletText("Calculate returns for the price series");
        ImGui::BulletText("Calculate the 20-day standard deviation of the returns.");
        ImGui::BulletText("Base Variation = 20-day std dev * closing price");
        ImGui::BulletText("Spike = (Close[i] - Close[i - 1]) * Previous Base Variation");
        ImGui::Separator();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Bonds and Interest Rates")) {
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Futures and Futures Options")) {
        ImGui::TreePop();
    }

    ImGui::Separator();
    ImGui::Text("Development");
    if (ImGui::TreeNode("Changelog")) {
        ImGui::Separator();
        ImGui::BulletText("V0.03 \tExtensive upgrades to API interfaces and more GUI enhancements.");
        ImGui::BulletText("V0.02 \tAdded support for CoinbasePro and created basic GUI layout.");
        ImGui::BulletText("V0.01 \tStarted Premia using TDAmeritrade API and SDL with ImGUI.");
        ImGui::Separator();
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Credits")) {
        ImGui::Separator();
        ImGui::TextWrapped("I'd like to thank myself, I'm the only one who could have made this possible - scawful");
        ImGui::Separator();
        ImGui::TreePop();
    }
}

void PrimaryView::drawAccountPane()
{
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Overview")) {
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Text("Account ID: " );
    ImGui::Text("Net Liq: ");
    ImGui::Text("Available Funds: ");
    ImGui::Text("Cash: ");
    ImGui::Separator();

    static double xs2[11], ys2[11], v[11];
    for (int i = 0; i < 11; ++i) {
        xs2[i] = i;
        ys2[i] = xs2[i] * xs2[i];
        v[i] = i;
    }
    if (ImPlot::BeginPlot("Account Performance")) {
        ImPlot::PlotBars("Value", v, 11);
        ImPlot::PlotLine("Return", xs2, ys2, 11);
        ImPlot::EndPlot();
    }

    ImGui::Separator();

    /**
     * @brief Button Table
     * 
     */
    if (ImGui::BeginTable("split", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings)) {
        for (int i = 0; i < 10; i++) {
            char buf[32];
            sprintf(buf, "%03d", i);
            ImGui::TableNextColumn();
            ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
        }
        ImGui::EndTable();
    }
}

void PrimaryView::drawScreen()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

    // Child 1: no border, enable horizontal scrollbar
    {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x * 0.30f, ImGui::GetContentRegionAvail().y), false, window_flags);
        drawAccountPane();
        ImGui::EndChild();
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    // Child 2: rounded border
    {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
        ImGui::BeginChild("ChildR", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y) , true, window_flags);
        drawInfoPane();
        ImGui::EndChild();
        ImGui::PopStyleVar();
    }

    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
}

void PrimaryView::addEvent(const std::string & key, const VoidEventHandler & event)
{
    this->events[key] = event;
}

void PrimaryView::update() 
{
    drawScreen();
}
