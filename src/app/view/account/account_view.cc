#include "account_view.h"

#include "premia/core/application/composition_root.hpp"
#include "premia/core/application/scaffold_application_service.hpp"

namespace premia {
void AccountView::DrawCoreAccountPreview() {
  if (!core_model.hasData()) {
    core_model.refresh();
  }

  const auto& account = core_model.getAccountDetail();
  const auto& portfolio = core_model.getPortfolioSummary();

  ImGui::Text("Core Account Preview");
  ImGui::TextColored(ImVec4(0.40f, 0.72f, 0.96f, 1.0f),
                     "Primary Brokerage: Charles Schwab");
  ImGui::TextDisabled(
      "This account pane is driven by provider-backed core contracts.");
  ImGui::Separator();

  ImGui::Text("Account ID: %s", account.account_id.c_str());
  ImGui::Text("Total Value: $%s", portfolio.total_value.amount.c_str());
  ImGui::Text("Cash: $%s", account.cash.amount.c_str());
  ImGui::Text("Buying Power: $%s", account.buying_power.amount.c_str());
  ImGui::Text("Day Change: $%s (%s%%)",
              portfolio.day_change.absolute.amount.c_str(),
              portfolio.day_change.percent.c_str());
  ImGui::Text("Holdings Count: %d", portfolio.holdings_count);
  ImGui::Separator();

  if (ImGui::BeginTable("CoreConnections", 3,
                        ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
    ImGui::TableSetupColumn("Provider");
    ImGui::TableSetupColumn("Status");
    ImGui::TableSetupColumn("Last Sync");
    ImGui::TableHeadersRow();

    for (const auto& connection : core_model.getConnections()) {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      if (connection.provider == core::domain::Provider::kSchwab) {
        ImGui::TextColored(ImVec4(0.40f, 0.72f, 0.96f, 1.0f), "%s",
                           connection.display_name.c_str());
      } else {
        ImGui::Text("%s", connection.display_name.c_str());
      }
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%s",
                  core::domain::ConnectionStatusToString(connection.status)
                      .c_str());
      ImGui::TableSetColumnIndex(2);
      ImGui::Text("%s",
                  connection.last_sync_at.empty()
                      ? "-"
                      : connection.last_sync_at.c_str());
    }

    ImGui::EndTable();
  }

  ImGui::Separator();

  if (ImGui::BeginTable("CoreHoldings", 4,
                        ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                            ImGuiTableFlags_SizingStretchSame)) {
    ImGui::TableSetupColumn("Symbol");
    ImGui::TableSetupColumn("Name");
    ImGui::TableSetupColumn("Market Value");
    ImGui::TableSetupColumn("Day Change");
    ImGui::TableHeadersRow();

    for (const auto& holding : account.positions) {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("%s", holding.symbol.c_str());
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%s", holding.name.c_str());
      ImGui::TableSetColumnIndex(2);
      ImGui::Text("$%s", holding.market_value.amount.c_str());
      ImGui::TableSetColumnIndex(3);
      ImGui::Text("$%s (%s%%)", holding.day_profit_loss.amount.c_str(),
                  holding.day_profit_loss_percent.c_str());
    }

    ImGui::EndTable();
  }
}

std::string AccountView::getName() { return "Account"; }

void AccountView::addLogger(const Logger &newLogger) {
  this->logger = newLogger;
}

void AccountView::addEvent(const std::string &key, const EventHandler &event) {
  this->events[key] = event;
}

void AccountView::Update() {
  if (!isInit) {
    core_model.addLogger(logger);
    core_model.refresh();
    isInit = true;
  }

  if (ImGui::Button("Refresh Core Account")) {
    core_model.refresh();
  }

  DrawCoreAccountPreview();
}
}  // namespace premia
