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
        ImGui::Separator(); 
        ImGui::TextColored(ImVec4(0.f, 170.f, 138.f, 1.0f), "TDAmeritrade"); ImGui::Separator();            
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
        ImGui::BulletText("V0.04 \tAdded more event driven programming.");
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

void PrimaryView::drawScreen()
{
    drawInfoPane();
}

String PrimaryView::getName() {
    return "Overview";
}

void PrimaryView::addLogger(const ConsoleLogger & newLogger)
{
    this->logger = newLogger;
}

void PrimaryView::addEvent(CRString key, const EventHandler & event)
{
    this->events[key] = event;
}

void PrimaryView::update() 
{
    drawScreen();
}
