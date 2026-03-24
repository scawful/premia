#include "account_view.h"

#include "premia/core/application/composition_root.hpp"
#include "premia/core/application/scaffold_application_service.hpp"

namespace premia {

namespace {

auto ResolveAccountSource(
    const std::vector<core::application::ConnectionSummary>& connections,
    const std::string& account_id) -> std::string {
  for (const auto& connection : connections) {
    if (connection.provider == core::domain::Provider::kSchwab &&
        connection.status == core::domain::ConnectionStatus::kConnected) {
      return connection.display_name;
    }
  }
  for (const auto& connection : connections) {
    if (connection.provider == core::domain::Provider::kIBKR &&
        connection.status == core::domain::ConnectionStatus::kConnected) {
      return connection.display_name;
    }
  }
  if (!account_id.empty() && account_id.rfind("U", 0) == 0) {
    return "Interactive Brokers";
  }
  return "Local Preview";
}

auto ChangeColor(const std::string& amount) -> ImVec4 {
  try {
    return std::stod(amount) < 0.0 ? ImVec4(0.89f, 0.34f, 0.36f, 1.0f)
                                    : ImVec4(0.24f, 0.78f, 0.55f, 1.0f);
  } catch (...) {
    return ImVec4(0.58f, 0.63f, 0.71f, 1.0f);
  }
}

}  // namespace

void AccountView::DrawCoreAccountPreview() {
  if (!core_model.hasData()) {
    core_model.refresh();
  }

  const auto& account = core_model.getAccountDetail();
  const auto& portfolio = core_model.getPortfolioSummary();
  const auto& connections = core_model.getConnections();

  ImGui::Text("Core Account Preview");
  ImGui::TextColored(ImVec4(0.40f, 0.72f, 0.96f, 1.0f),
                     "Account Source: %s",
                     ResolveAccountSource(connections, account.account_id).c_str());
  ImGui::TextDisabled(
      "This account pane is driven by provider-backed account contracts.");
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

  if (ImGui::BeginTable("CoreHoldings", 6,
                        ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                            ImGuiTableFlags_SizingStretchSame)) {
    ImGui::TableSetupColumn("Symbol");
    ImGui::TableSetupColumn("Name");
    ImGui::TableSetupColumn("Quantity");
    ImGui::TableSetupColumn("Avg Price");
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
      ImGui::Text("%s", holding.quantity.c_str());
      ImGui::TableSetColumnIndex(3);
      ImGui::Text("$%s", holding.average_price.amount.c_str());
      ImGui::TableSetColumnIndex(4);
      ImGui::Text("$%s", holding.market_value.amount.c_str());
      ImGui::TableSetColumnIndex(5);
      ImGui::TextColored(ChangeColor(holding.day_profit_loss.amount), "$%s (%s%%)",
                         holding.day_profit_loss.amount.c_str(),
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
