#include "chart_view.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <memory>
#include <string>

#include "metatypes.h"
#include "premia/core/application/composition_root.hpp"
#include "premia/core/application/scaffold_application_service.hpp"
#include "view/chart/subview/candle_chart.h"
#include "view/view.h"

namespace premia {

namespace {

auto ResolveMarketDataSource(
    const std::vector<core::application::ConnectionSummary>& connections)
    -> std::string {
  for (const auto& connection : connections) {
    if (connection.provider == core::domain::Provider::kSchwab &&
        connection.status == core::domain::ConnectionStatus::kConnected) {
      return connection.display_name;
    }
  }
  return "Local Preview";
}

auto PeriodTypeLabel(int period_type) -> const char* {
  switch (period_type) {
    case 0:
      return "Day";
    case 1:
      return "Month";
    case 2:
      return "Year";
    case 3:
      return "YTD";
    default:
      return "Year";
  }
}

auto PeriodAmountValue(int period_amount) -> const char* {
  static const char* kValues[] = {"1", "2", "3", "4", "5",
                                  "6", "10", "15", "20"};
  constexpr int kCount = sizeof(kValues) / sizeof(kValues[0]);
  if (period_amount < 0 || period_amount >= kCount) {
    return "1";
  }
  return kValues[period_amount];
}

auto FrequencyTypeLabel(int frequency_type) -> const char* {
  switch (frequency_type) {
    case 0:
      return "Minute";
    case 1:
      return "Daily";
    case 2:
      return "Weekly";
    case 3:
      return "Monthly";
    default:
      return "Daily";
  }
}

auto FrequencyAmountValue(int frequency_amount) -> const char* {
  static const char* kValues[] = {"1", "5", "10", "15", "30"};
  constexpr int kCount = sizeof(kValues) / sizeof(kValues[0]);
  if (frequency_amount < 0 || frequency_amount >= kCount) {
    return "1";
  }
  return kValues[frequency_amount];
}

}  // namespace

void ChartView::initChart() {
  currentChart = "CANDLESTICK";
  charts["CANDLESTICK"] = std::make_shared<CandleChart>();
  charts["CANDLESTICK"]->importModel(model);
  isInit = true;
}

void ChartView::FetchChartData() {
  if (tickerSymbol.empty()) {
    return;
  }
  charts[currentChart]->fetchData(tickerSymbol, tda::PeriodType(period_type),
                                  period_amount, tda::FrequencyType(frequency_type),
                                  frequency_amount, true);
  pending_refresh_ = false;
}

void ChartView::DrawChart() { charts.at(currentChart)->Update(); }

void ChartView::DrawChartSettings() {
  if (ImGui::BeginTable("Chart Settings", 13, ImGuiTableFlags_SizingFixedFit)) {
    ImGui::TableNextColumn();
    ImGui::Text("Symbol");
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(50.f);
    ImGui::InputText("##chartEnterSymbol", &tickerSymbol,
                     ImGuiInputTextFlags_CharsUppercase);
    if (ImGui::IsItemDeactivatedAfterEdit() && !tickerSymbol.empty()) {
      pending_refresh_ = true;
      if (symbol_change_handler_) {
        symbol_change_handler_(tickerSymbol);
      }
    }
    ImGui::TableNextColumn();
    ImGui::Text("Period");
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(55.f);
    ImGui::Combo("##Period", &period_type, "Day\0Month\0Year\0YTD\0");
    ImGui::TableNextColumn();
    ImGui::Text("Type");
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(50.f);
    ImGui::Combo("##type", &period_amount,
                 " 1\0 2\0 3\0 4\0 5\0 6\0 10\0 15\0 20\0");
    ImGui::TableNextColumn();
    ImGui::Text("Frequency");
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(75.f);
    ImGui::Combo("##frequency", &frequency_type,
                 "Minute\0Daily\0Weekly\0Monthly\0");
    ImGui::TableNextColumn();
    ImGui::Text("Amount");
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(50.f);
    ImGui::Combo("##amount", &frequency_amount, " 1\0 5\0 10\0 15\0 30\0");
    ImGui::TableNextColumn();
    if (ImGui::Button("Search") && !tickerSymbol.empty()) {
      pending_refresh_ = true;
      if (symbol_change_handler_) {
        symbol_change_handler_(tickerSymbol);
      }
    }
    ImGui::EndTable();
  }
}

auto ChartView::GetSelectedRangeLabel() const -> std::string {
  return std::string(PeriodTypeLabel(period_type)) + ":" +
         PeriodAmountValue(period_amount);
}

auto ChartView::GetSelectedIntervalLabel() const -> std::string {
  return std::string(FrequencyTypeLabel(frequency_type)) + ":" +
         FrequencyAmountValue(frequency_amount);
}

void ChartView::DrawCoreContractPreview() {
  const std::string preview_symbol = tickerSymbol.empty() ? "AAPL" : tickerSymbol;
  auto& service = core::application::CompositionRoot::Instance().AppService();
  const auto connections = service.GetConnections();
  const auto quote = service.GetQuoteDetail(preview_symbol);
  const auto chart = service.GetChartScreen(preview_symbol, GetSelectedRangeLabel(),
                                            GetSelectedIntervalLabel(), true);

  ImGui::Separator();
  ImGui::Text("Core Contract Preview");
  ImGui::TextColored(ImVec4(0.40f, 0.72f, 0.96f, 1.0f),
                     "Market Data Source: %s",
                     ResolveMarketDataSource(connections).c_str());
  ImGui::TextDisabled(
      "Charts prefer Schwab when connected and fall back to local preview data otherwise.");

  if (ImGui::BeginTable("ChartContractPreview", 5,
                        ImGuiTableFlags_SizingStretchSame)) {
    ImGui::TableNextColumn();
    ImGui::Text("Symbol");
    ImGui::TableNextColumn();
    ImGui::Text("Range");
    ImGui::TableNextColumn();
    ImGui::Text("Interval");
    ImGui::TableNextColumn();
    ImGui::Text("Last");
    ImGui::TableNextColumn();
    ImGui::Text("Change");

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("%s", quote.instrument.symbol.c_str());
    ImGui::TableNextColumn();
    ImGui::Text("%s", chart.range.c_str());
    ImGui::TableNextColumn();
    ImGui::Text("%s", chart.interval.c_str());
    ImGui::TableNextColumn();
    ImGui::Text("$%s", quote.quote.last_price.amount.c_str());
    ImGui::TableNextColumn();
    ImGui::Text("$%s (%s%%)", chart.stats.change.absolute.amount.c_str(),
                chart.stats.change.percent.c_str());
    ImGui::EndTable();
  }

  ImGui::Text("Series Type: %s | Bars: %d | Exchange: %s", chart.series.type.c_str(),
              static_cast<int>(chart.series.bars.size()),
              quote.instrument.primary_exchange.c_str());
}

std::string ChartView::getName() { return "Chart"; }

void ChartView::addLogger(const Logger& newLogger) { this->logger = newLogger; }

void ChartView::addEvent(const std::string& key, const EventHandler& event) {
  this->events[key] = event;
}

void ChartView::SetTickerSymbol(const std::string& symbol) {
  if (symbol.empty() || tickerSymbol == symbol) {
    return;
  }
  tickerSymbol = symbol;
  pending_refresh_ = true;
}

void ChartView::SetSymbolChangeHandler(
    const std::function<void(const std::string&)>& handler) {
  symbol_change_handler_ = handler;
}

void ChartView::Update() {
  if (!isInit) {
    initChart();
  }
  if (pending_refresh_) {
    FetchChartData();
  }
  DrawChartSettings();
  DrawCoreContractPreview();
  DrawChart();
}

}  // namespace premia
