#include "AccountView.hpp"

/**
 * @brief 
 * 
 * @param variable 
 */
void AccountView::draw_balance_string(CRString variable)
{
    String str = (halext::HLXT::getInstance().getPrivateBalance()) ? "***" : variable;
    ImGui::Text("%s", str.c_str());
}

/**
 * @brief Load an individual account 
 * @author @scawful 
 * 
 * @param account_num 
 */
void AccountView::load_account(CRString account_num)
{
    account_data = tda::TDA::getInstance().getAccount(account_num);

    if (!positions_vector.empty()) {
        positions_vector.clear();
    }

    for (int i = 0; i < account_data.get_position_vector_size(); i++) {
        for (const auto & [key, value] : account_data.get_position(i)) {
            if (key == "symbol") {
                String str = value;
                positions_vector.push_back(str);
            }
        }
    }
}

/**
 * @brief Load all accounts 
 * @author @scawful 
 * 
 */
void AccountView::initPositions()
{
    account_ids_std = tda::TDA::getInstance().getAllAcountNumbers();
    int i = 0;
    for (String const& each_id : account_ids_std) {
        account_ids.push_back(each_id.c_str());
        i++;
    }
    default_account = account_ids_std.at(0);
    load_account(default_account);
}

/**
 * @brief Construct the table with the current portfolio positions
 * @author @scawful
 * 
 */
void AccountView::draw_positions()
{
    static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

    if (ImGui::BeginTable("PositionsTable", 6, flags)) {
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
        ImGui::TableSetupColumn("Symbol", ImGuiTableColumnFlags_WidthStretch );
        ImGui::TableSetupColumn("P/L Day", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableSetupColumn("P/L %", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableSetupColumn("Avg Price",  ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableSetupColumn("Mkt Value",  ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableSetupColumn("Qty", ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin( (int) positions_vector.size() );
        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                ImGui::TableNextRow();
                for (int column = 0; column < 6; column++) {
                    String symbol = positions_vector[row];
                    ImGui::TableSetColumnIndex(column);
                    switch(column) {
                        case 0:
                            ImGui::Text("%s", symbol.c_str() );
                            break;
                        case 1:
                            draw_balance_string(account_data.get_position_balances(symbol, "currentDayProfitLoss"));
                            break;
                        case 2:
                            draw_balance_string(account_data.get_position_balances(symbol, "currentDayProfitLossPercentage"));
                            break;
                        case 3:
                            draw_balance_string(account_data.get_position_balances( symbol, "averagePrice"));
                            break;
                        case 4:
                            draw_balance_string(account_data.get_position_balances(symbol, "marketValue"));
                            break;
                        case 5:
                            draw_balance_string(account_data.get_position_balances(symbol, "longQuantity"));
                            break;
                        default:
                            break;
                    }
                }
            }
        }

        ImGui::EndTable();
    }
}


void AccountView::drawAccountPane()
{
    ImGui::Text("Account ID: %s", account_data.get_account_variable("accountId").c_str());
    ImGui::Text("Net Liq: %s", account_data.get_balance_variable("liquidationValue").c_str());
    ImGui::Text("Available Funds: %s", account_data.get_balance_variable("availableFunds").c_str());
    ImGui::Text("Cash: %s", account_data.get_balance_variable("cashBalance").c_str());
    ImGui::Separator();

    /////

    // Load Account IDs
    static int n = 0;
    const char **accounts = account_ids.data();
    if (ImGui::Button("Switch")) {
        load_account( accounts[n] );
    } 
    ImGui::SameLine(); ImGui::SetNextItemWidth(50.f);
    ImGui::Combo("##accounts", &n,  accounts, 6); 

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {
        if (ImGui::BeginTabItem("Positions")) {
            draw_positions();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Performance")) {
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
            if (ImGui::BeginTable("split", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchSame)) {
                for (int i = 0; i < 6; i++) {
                    char buf[32];
                    sprintf(buf, "%03d", i);
                    ImGui::TableNextColumn();
                    ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
                }
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

String AccountView::getName() {
    return "Account";
}

void AccountView::addLogger(const ConsoleLogger & newLogger)
{
    this->logger = newLogger;
}

void AccountView::addEvent(CRString key, const EventHandler & event)
{
    this->events[key] = event;
}

void AccountView::update() 
{
    if (!isInit) {
        initPositions();
        isInit = true;
    } else {
        drawAccountPane();
    }
}
