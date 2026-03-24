#include "account_view.h"

#include "premia/core/application/composition_root.hpp"
#include "premia/core/application/scaffold_application_service.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>

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

auto ParseAmount(const std::string& amount) -> double {
  try {
    return std::stod(amount);
  } catch (...) {
    return 0.0;
  }
}

auto AllocationRatio(double market_value, double total_value) -> float {
  if (total_value <= 0.0) {
    return 0.0f;
  }
  return static_cast<float>(std::clamp(market_value / total_value, 0.0, 1.0));
}

auto FormatAmount(double amount) -> std::string {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2) << amount;
  return oss.str();
}

auto HeatmapColor(double pnl_percent) -> ImVec4 {
  const float intensity = static_cast<float>(std::min(std::abs(pnl_percent) / 5.0, 1.0));
  if (pnl_percent < 0.0) {
    return ImVec4(0.36f + 0.32f * intensity, 0.16f, 0.18f, 0.96f);
  }
  return ImVec4(0.12f, 0.28f + 0.36f * intensity, 0.22f, 0.96f);
}

void DrawMetricCard(const char* id, const char* label, const std::string& value,
                    const ImVec4& color, const std::string& note) {
  ImGui::BeginChild(id, ImVec2(0.0f, 84.0f), true);
  ImGui::TextDisabled("%s", label);
  ImGui::PushStyleColor(ImGuiCol_Text, color);
  ImGui::Text("%s", value.c_str());
  ImGui::PopStyleColor();
  ImGui::TextDisabled("%s", note.c_str());
  ImGui::EndChild();
}

}  // namespace

void AccountView::DrawCoreAccountPreview() {
  if (!core_model.hasData()) {
    core_model.refresh();
  }

  const auto& account = core_model.getAccountDetail();
  const auto& portfolio = core_model.getPortfolioSummary();
  const auto& connections = core_model.getConnections();
  const auto portfolio_total = ParseAmount(portfolio.total_value.amount);
  const auto cash_total = ParseAmount(account.cash.amount);
  const auto invested_total = std::max(0.0, portfolio_total - cash_total);

  int winners = 0;
  int losers = 0;
  for (const auto& holding : account.positions) {
    if (ParseAmount(holding.day_profit_loss.amount) >= 0.0) {
      ++winners;
    } else {
      ++losers;
    }
  }

  ImGui::Text("Core Account Preview");
  ImGui::TextColored(ImVec4(0.40f, 0.72f, 0.96f, 1.0f),
                     "Account Source: %s",
                     ResolveAccountSource(connections, account.account_id).c_str());
  ImGui::TextDisabled(
      "This account pane is driven by provider-backed account contracts.");
  ImGui::Separator();

  if (ImGui::BeginTable("AccountMetrics", 4,
                        ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInnerV)) {
    ImGui::TableNextColumn();
    DrawMetricCard("AccountValueCard", "Account Value",
                   "$" + portfolio.total_value.amount,
                   ImVec4(0.83f, 0.69f, 0.28f, 1.0f),
                   "Account ID " + account.account_id);
    ImGui::TableNextColumn();
    DrawMetricCard("AccountCashCard", "Cash",
                   "$" + account.cash.amount,
                   ImVec4(0.24f, 0.78f, 0.55f, 1.0f),
                   "Buying power $" + account.buying_power.amount);
    ImGui::TableNextColumn();
    DrawMetricCard("InvestedCapitalCard", "Invested Capital",
                   "$" + FormatAmount(invested_total),
                   ImVec4(0.40f, 0.72f, 0.96f, 1.0f),
                   "Net invested after cash reserve");
    ImGui::TableNextColumn();
    DrawMetricCard("BreadthCard", "Breadth",
                   std::to_string(winners) + " up / " + std::to_string(losers) + " down",
                   winners >= losers ? ImVec4(0.24f, 0.78f, 0.55f, 1.0f)
                                     : ImVec4(0.89f, 0.34f, 0.36f, 1.0f),
                   "Day change $" + portfolio.day_change.absolute.amount +
                       " (" + portfolio.day_change.percent + "%)");
    ImGui::EndTable();
  }

  ImGui::Spacing();

  if (ImGui::BeginChild("HoldingsExposureCard", ImVec2(0.0f, 142.0f), true)) {
    ImGui::Text("Holdings Exposure");
    ImGui::TextDisabled("Largest positions in the current account selection.");
    std::vector<core::application::AccountPositionRow> ranked_positions = account.positions;
    std::sort(ranked_positions.begin(), ranked_positions.end(),
              [](const auto& lhs, const auto& rhs) {
                return ParseAmount(lhs.market_value.amount) >
                       ParseAmount(rhs.market_value.amount);
              });
    const auto visible_count = std::min<size_t>(5, ranked_positions.size());
    for (size_t index = 0; index < visible_count; ++index) {
      const auto& holding = ranked_positions[index];
      const auto ratio = AllocationRatio(ParseAmount(holding.market_value.amount),
                                         portfolio_total);
      ImGui::Text("%s", holding.symbol.c_str());
      ImGui::SameLine();
      ImGui::TextDisabled("%s", holding.name.c_str());
      ImGui::ProgressBar(ratio, ImVec2(-FLT_MIN, 0.0f),
                         (holding.market_value.amount + " / " +
                          std::to_string(static_cast<int>(ratio * 100.0f)) + "%")
                             .c_str());
    }
    ImGui::EndChild();
  }

  ImGui::Spacing();

  if (ImGui::BeginChild("PortfolioHeatmapCard", ImVec2(0.0f, 210.0f), true)) {
    ImGui::Text("Portfolio Heatmap");
    ImGui::TextDisabled("Largest positions by weight, tinted by daily performance.");

    std::vector<core::application::AccountPositionRow> ranked_positions = account.positions;
    std::sort(ranked_positions.begin(), ranked_positions.end(),
              [](const auto& lhs, const auto& rhs) {
                return ParseAmount(lhs.market_value.amount) >
                       ParseAmount(rhs.market_value.amount);
              });

    const auto visible_count = std::min<size_t>(6, ranked_positions.size());
    if (visible_count == 0) {
      ImGui::TextDisabled("No positions available for heatmap rendering.");
    } else if (ImGui::BeginTable("PortfolioHeatmapTable", 3,
                                 ImGuiTableFlags_SizingStretchSame |
                                     ImGuiTableFlags_BordersInnerV)) {
      for (size_t index = 0; index < visible_count; ++index) {
        if (index % 3 == 0) {
          ImGui::TableNextRow();
        }
        ImGui::TableSetColumnIndex(static_cast<int>(index % 3));
        const auto& holding = ranked_positions[index];
        const auto pnl_percent = ParseAmount(holding.day_profit_loss_percent);
        const auto allocation = AllocationRatio(ParseAmount(holding.market_value.amount),
                                                portfolio_total);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, HeatmapColor(pnl_percent));
        ImGui::BeginChild(("HeatmapCell" + holding.symbol).c_str(),
                          ImVec2(0.0f, 92.0f), true);
        ImGui::Text("%s", holding.symbol.c_str());
        ImGui::TextDisabled("$%s", holding.market_value.amount.c_str());
        ImGui::Text("Alloc: %d%%", static_cast<int>(allocation * 100.0f));
        ImGui::Text("Day: %s%%", holding.day_profit_loss_percent.c_str());
        ImGui::EndChild();
        ImGui::PopStyleColor();
      }
      ImGui::EndTable();
    }
    ImGui::EndChild();
  }

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

  if (ImGui::BeginTable("CoreHoldings", 7,
                        ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                            ImGuiTableFlags_SizingStretchSame)) {
    ImGui::TableSetupColumn("Symbol");
    ImGui::TableSetupColumn("Name");
    ImGui::TableSetupColumn("Quantity");
    ImGui::TableSetupColumn("Avg Price");
    ImGui::TableSetupColumn("Market Value");
    ImGui::TableSetupColumn("Allocation");
    ImGui::TableSetupColumn("Day Change");
    ImGui::TableHeadersRow();

    for (const auto& holding : account.positions) {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      const bool is_selected = selected_symbol_ == holding.symbol;
      if (ImGui::Selectable(holding.symbol.c_str(), is_selected,
                            ImGuiSelectableFlags_SpanAllColumns)) {
        selected_symbol_ = holding.symbol;
        if (symbol_selection_handler_) {
          symbol_selection_handler_(holding.symbol);
        }
      }
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%s", holding.name.c_str());
      ImGui::TableSetColumnIndex(2);
      ImGui::Text("%s", holding.quantity.c_str());
      ImGui::TableSetColumnIndex(3);
      ImGui::Text("$%s", holding.average_price.amount.c_str());
      ImGui::TableSetColumnIndex(4);
      ImGui::Text("$%s", holding.market_value.amount.c_str());
      ImGui::TableSetColumnIndex(5);
      const auto ratio = AllocationRatio(ParseAmount(holding.market_value.amount),
                                         portfolio_total);
      ImGui::ProgressBar(ratio, ImVec2(-FLT_MIN, 0.0f),
                         (std::to_string(static_cast<int>(ratio * 100.0f)) + "%")
                             .c_str());
      ImGui::TableSetColumnIndex(6);
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

void AccountView::SetActiveAccountId(const std::string& account_id) {
  active_account_id_ = account_id;
  core_model.setActiveAccountId(account_id);
}

void AccountView::SetSelectedSymbol(const std::string& symbol) {
  selected_symbol_ = symbol;
}

void AccountView::SetSymbolSelectionHandler(
    const std::function<void(const std::string&)>& handler) {
  symbol_selection_handler_ = handler;
}

void AccountView::Update() {
  if (!isInit) {
    core_model.addLogger(logger);
    core_model.setActiveAccountId(active_account_id_);
    core_model.refresh();
    isInit = true;
  }

  if (ImGui::Button("Refresh Core Account")) {
    core_model.setActiveAccountId(active_account_id_);
    core_model.refresh();
  }

  DrawCoreAccountPreview();
}
}  // namespace premia
