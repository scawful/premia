#include "AccountView.hpp"

namespace premia {
void AccountView::draw_balance_string(CRString variable) {
  String str =
      (halext::HLXT::getInstance().getPrivateBalance()) ? "***" : variable;
  ImGui::Text("%s", str.c_str());
}

void AccountView::draw_symbol_string(CRString symbol) {
  String outputSymbol = symbol;
  if (symbol.find('_') != String::npos) {
    outputSymbol = tda::TDA::getInstance().parseOptionSymbol(symbol);
  }
  ImGui::Text("%s", outputSymbol.c_str());
}

/**
 * @brief Load an individual account
 * @author @scawful
 *
 * @param account_num
 */
void AccountView::load_account(CRString account_num) {
  account_data = tda::TDA::getInstance().getAccount(account_num);

  if (!positions_vector.empty()) positions_vector.clear();

  for (int i = 0; i < account_data.get_position_vector_size(); i++) {
    for (const auto &[key, value] : account_data.get_position(i)) {
      if (key == "symbol") {
        String str = value;
        positions_vector.push_back(str);
      }
    }
  }
}

void AccountView::load_all_accounts() {
  account_data = tda::TDA::getInstance().getAllAccounts();

  if (!positions_vector.empty()) {
    positions_vector.clear();
  }

  for (int i = 0; i < account_data.get_position_vector_size(); i++) {
    for (const auto &[key, value] : account_data.get_position(i)) {
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
void AccountView::initPositions() {
  // @todo move this into a model class
  Try { account_ids_std = tda::TDA::getInstance().getAllAcountNumbers(); }
  catch (const std::bad_function_call &e) {
    String error(e.what());
    logger("[error] " + error);
    throw premia::NotLoggedInException();
  }
  catch (const boost::property_tree::ptree_error &e) {
    String error(e.what());
    logger("[error] " + error);
    throw premia::NotLoggedInException();
  }
  finally { logger("[finally] : AccountView init"); }
  Proceed;
  int i = 0;
  for (String const &each_id : account_ids_std) {
    account_ids.push_back(each_id.c_str());
    i++;
  }
  default_account = account_ids_std.at(0);
  load_account(default_account);
}

//     if (ImGui::BeginTable("table_sorting", 4, flags, ImVec2(0.0f,
//     TEXT_BASE_HEIGHT * 15), 0.0f))
//     {
//         // Declare columns
//         // We use the "user_id" parameter of TableSetupColumn() to specify a
//         user id that will be stored in the sort specifications.
//         // This is so our sort function can identify a column given our own
//         identifier. We could also identify them based on their index!
//         // Demonstrate using a mixture of flags among available sort-related
//         flags:
//         // - ImGuiTableColumnFlags_DefaultSort
//         // - ImGuiTableColumnFlags_NoSort /
//         ImGuiTableColumnFlags_NoSortAscending /
//         ImGuiTableColumnFlags_NoSortDescending
//         // - ImGuiTableColumnFlags_PreferSortAscending /
//         ImGuiTableColumnFlags_PreferSortDescending
//         ImGui::TableSetupColumn("ID",       ImGuiTableColumnFlags_DefaultSort
//         | ImGuiTableColumnFlags_WidthFixed,   0.0f, MyItemColumnID_ID);
//         ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed,
//         0.0f, MyItemColumnID_Name); ImGui::TableSetupColumn("Action",
//         ImGuiTableColumnFlags_NoSort               |
//         ImGuiTableColumnFlags_WidthFixed,   0.0f, MyItemColumnID_Action);
//         ImGui::TableSetupColumn("Quantity",
//         ImGuiTableColumnFlags_PreferSortDescending |
//         ImGuiTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Quantity);
//         ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
//         ImGui::TableHeadersRow();

//         // Sort our data if sort specs have been changed!
//         if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
//             if (sorts_specs->SpecsDirty)
//             {
//                 MyItem::s_current_sort_specs = sorts_specs; // Store in
//                 variable accessible by the sort function. if (items.Size > 1)
//                     qsort(&items[0], (size_t)items.Size, sizeof(items[0]),
//                     MyItem::CompareWithSortSpecs);
//                 MyItem::s_current_sort_specs = NULL;
//                 sorts_specs->SpecsDirty = false;
//             }

//         // Demonstrate using clipper for large vertical lists
//         ImGuiListClipper clipper;
//         clipper.Begin(items.Size);
//         while (clipper.Step())
//             for (int row_n = clipper.DisplayStart; row_n <
//             clipper.DisplayEnd; row_n++)
//             {
//                 // Display a data item
//                 MyItem* item = &items[row_n];
//                 ImGui::PushID(item->ID);
//                 ImGui::TableNextRow();
//                 ImGui::TableNextColumn();
//                 ImGui::Text("%04d", item->ID);
//                 ImGui::TableNextColumn();
//                 ImGui::TextUnformatted(item->Name);
//                 ImGui::TableNextColumn();
//                 ImGui::SmallButton("None");
//                 ImGui::TableNextColumn();
//                 ImGui::Text("%d", item->Quantity);
//                 ImGui::PopID();
//             }
//         ImGui::EndTable();
//     }
//     ImGui::TreePop();
// }

/**
 * @brief Construct the table with the current portfolio positions
 * @author @scawful
 *
 */
void AccountView::draw_positions() {
  if (ImGui::BeginTable("PositionsTable", 6, positionFlags)) {
    ImGui::TableSetupScrollFreeze(0, 1);  // Make top row always visible
    ImGui::TableSetupColumn("Symbol", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("P/L Day", ImGuiTableColumnFlags_NoSort |
                                           ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("P/L %", ImGuiTableColumnFlags_NoSort |
                                         ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Avg Price", ImGuiTableColumnFlags_NoSort |
                                             ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Mkt Value", ImGuiTableColumnFlags_NoSort |
                                             ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Qty", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableHeadersRow();

    ImGuiListClipper clipper;
    clipper.Begin((int)positions_vector.size());
    while (clipper.Step()) {
      for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
        ImGui::TableNextRow();
        for (int column = 0; column < 6; column++) {
          String symbol = positions_vector[row];
          ImGui::TableSetColumnIndex(column);
          switch (column) {
            case 0:
              draw_symbol_string(symbol);
              break;
            case 1:
              draw_balance_string(account_data.get_position_balances(
                  symbol, "currentDayProfitLoss"));
              break;
            case 2:
              draw_balance_string(account_data.get_position_balances(
                  symbol, "currentDayProfitLossPercentage"));
              break;
            case 3:
              draw_balance_string(
                  account_data.get_position_balances(symbol, "averagePrice"));
              break;
            case 4:
              draw_balance_string(
                  account_data.get_position_balances(symbol, "marketValue"));
              break;
            case 5:
              draw_balance_string(
                  account_data.get_position_balances(symbol, "longQuantity"));
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

void AccountView::drawAccountPane() {
  ImGui::Text("Account ID: %s",
              account_data.get_account_variable("accountId").c_str());
  ImGui::Text("Cash: %s",
              account_data.get_balance_variable("cashBalance").c_str());
  ImGui::Text("Net Liq: %s",
              account_data.get_balance_variable("liquidationValue").c_str());
  ImGui::Text("Available Funds: %s",
              account_data.get_balance_variable("availableFunds").c_str());
  ImGui::Text("Long Mkt Val: %s",
              account_data.get_balance_variable("longMarketValue").c_str());
  ImGui::Text("Short Mtk Val: %s",
              account_data.get_balance_variable("shortMarketValue").c_str());
  ImGui::Text("Cumulative BP: %s",
              account_data.get_balance_variable("buyingPower").c_str());
  ImGui::Text("Equity: %s, %s%",
              account_data.get_balance_variable("equity").c_str(),
              account_data.get_balance_variable("equityPercentage").c_str());
  ImGui::Text("Margin Balance: %s",
              account_data.get_balance_variable("marginBalance").c_str());
  ImGui::Separator();

  //     "accruedInterest": 0,
  //     "cashBalance": 0,
  //     "cashReceipts": 0,
  //     "longOptionMarketValue": 0,
  //     "liquidationValue": 0,
  //     "moneyMarketFund": 0,
  //     "savings": 0,
  //     "pendingDeposits": 0,
  //     "dayTradingBuyingPower": 0,
  //     "dayTradingBuyingPowerCall": 0,
  //     "longMarginValue": 0,
  //     "maintenanceCall": 0,
  //     "maintenanceRequirement": 0,
  //     "regTCall": 0,
  //     "sma": 0,
  //   },

  // Load Account IDs
  static int n = 0;
  const char **accounts = account_ids.data();
  if (ImGui::Button("Switch")) {
    load_account(accounts[n]);
  }
  ImGui::SameLine();
  if (ImGui::Button("Test")) {
    load_all_accounts();
  }
  ImGui::SameLine();
  ImGui::SetNextItemWidth(100.f);
  ImGui::Combo("##accounts", &n, accounts, account_ids.size());

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
      if (ImGui::BeginTable(
              "split", 2,
              ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchSame)) {
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

String AccountView::getName() { return "Account"; }

void AccountView::addLogger(const Logger &newLogger) {
  this->logger = newLogger;
}

void AccountView::addEvent(CRString key, const EventHandler &event) {
  this->events[key] = event;
}

void AccountView::update() {
  if (!isInit) {
    Try {
      initPositions();
      isLoggedIn = true;
    }
    catch (const premia::NotLoggedInException &e) {
      logger("User not logged in, loading empty account pane");
    }
    finally { isInit = true; }
    Proceed;
  } else {
    if (isLoggedIn) {
      drawAccountPane();
    } else {
      ImGui::Text("empty account pane goes here");
    }
  }
}
}  // namespace premia
