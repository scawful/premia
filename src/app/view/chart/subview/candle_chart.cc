#include "candle_chart.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

#include <imgui/imgui.h>
#include <implot/implot.h>
#include <implot/implot_internal.h>

#include "model/chart/chart_model.h"
#include "view/chart/chart.h"

namespace premia {
/**
 * @brief double array binary search
 * @author @scawful
 *
 * @param arr
 * @param l
 * @param r
 * @param x
 * @return int
 */
int CandleChart::binary_search(const std::vector<double>& arr, int l, int r,
                               double x) {
  if (r >= l) {
    int mid = l + (r - l) / 2;
    if (arr[mid] == x) {
      return mid;
    }
    if (arr[mid] > x) {
      return binary_search(arr, l, mid - 1, x);
    }
    return binary_search(arr, mid + 1, r, x);
  }
  return -1;
}

int CandleChart::nearest_index(const std::vector<double>& arr, double x) {
  if (arr.empty()) {
    return -1;
  }
  const auto lower = std::lower_bound(arr.begin(), arr.end(), x);
  if (lower == arr.begin()) {
    return 0;
  }
  if (lower == arr.end()) {
    return static_cast<int>(arr.size()) - 1;
  }
  const auto next_index = static_cast<int>(lower - arr.begin());
  const auto prev_index = next_index - 1;
  return std::fabs(arr[next_index] - x) < std::fabs(arr[prev_index] - x)
             ? next_index
             : prev_index;
}

void CandleChart::DrawReadoutCard() {
  if (model == nullptr || !model->isActive() || model->getNumCandles() == 0) {
    return;
  }

  const int index = hovered_index_ >= 0 ? hovered_index_ : model->getNumCandles() - 1;
  const auto candle = model->getCandle(index);
  const double absolute_change = candle.close - candle.open;
  const double percent_change = candle.open == 0.0 ? 0.0
                                                   : (absolute_change / candle.open) * 100.0;

  ImGui::BeginChild("CandleReadoutCard", ImVec2(0.0f, 82.0f), true);
  ImGui::Text("%s", model->getTickerSymbol().c_str());
  ImGui::SameLine();
  ImGui::TextDisabled("%s", hovered_index_ >= 0 ? "Hovered candle" : "Latest candle");
  ImGui::TextDisabled("%s", candle.datetime.c_str());

  if (ImGui::BeginTable("CandleReadoutTable", 6,
                        ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInnerV)) {
    ImGui::TableNextColumn();
    ImGui::Text("Open\n$%.2f", candle.open);
    ImGui::TableNextColumn();
    ImGui::Text("High\n$%.2f", candle.high);
    ImGui::TableNextColumn();
    ImGui::Text("Low\n$%.2f", candle.low);
    ImGui::TableNextColumn();
    ImGui::Text("Close\n$%.2f", candle.close);
    ImGui::TableNextColumn();
    ImGui::TextColored(absolute_change < 0.0 ? ImVec4(0.89f, 0.34f, 0.36f, 1.0f)
                                             : ImVec4(0.24f, 0.78f, 0.55f, 1.0f),
                       "Change\n%+.2f (%+.2f%%)", absolute_change,
                       percent_change);
    ImGui::TableNextColumn();
    ImGui::Text("Volume\n%.0f", candle.volume);
    ImGui::EndTable();
  }
  ImGui::EndChild();
}

void CandleChart::DrawOverlayMarkers() {
  if (model == nullptr || !model->isActive()) {
    return;
  }

  const auto& markers = model->getOverlayMarkers();
  if (markers.empty()) {
    return;
  }

  auto* draw_list = ImPlot::GetPlotDrawList();
  const float left = ImPlot::GetPlotPos().x;
  const float right = left + ImPlot::GetPlotSize().x;
  const auto latest_close = model->getCandle(model->getNumCandles() - 1).close;

  ImPlot::PushPlotClipRect();
  for (const auto& marker : markers) {
    const auto point = ImPlot::PlotToPixels(model->getDate(model->getNumCandles() - 1),
                                            marker.price);
    ImU32 color = IM_COL32(198, 176, 72, 210);
    if (marker.kind == "avg_cost") {
      color = IM_COL32(90, 180, 255, 210);
      const auto latest_point =
          ImPlot::PlotToPixels(model->getDate(model->getNumCandles() - 1), latest_close);
      const auto zone_top = std::min(point.y, latest_point.y);
      const auto zone_bottom = std::max(point.y, latest_point.y);
      draw_list->AddRectFilled(
          ImVec2(left, zone_top), ImVec2(right, zone_bottom),
          latest_close >= marker.price ? IM_COL32(60, 170, 110, 28)
                                       : IM_COL32(180, 70, 80, 28));
    } else if (marker.kind == "order") {
      color = IM_COL32(102, 214, 140, 210);
    } else if (marker.kind == "fill") {
      color = IM_COL32(255, 162, 66, 220);
    }
    draw_list->AddLine(ImVec2(left, point.y), ImVec2(right, point.y), color, 1.25f);
    draw_list->AddText(ImVec2(left + 8.0f, point.y - 14.0f), color,
                       (marker.label + " @ $" +
                        [&marker]() {
                          std::ostringstream oss;
                          oss << std::fixed << std::setprecision(2) << marker.price;
                          return oss.str();
                        }())
                           .c_str());
  }
  ImPlot::PopPlotClipRect();
}

/**
 * @brief Build the ImGui candle chart
 * @author @scawful
 *
 * @param width_percent
 * @param count
 * @param bullCol
 * @param bearCol
 * @param tooltip
 */
void CandleChart::DrawCandles(float width_percent, int count, ImVec4 bullCol,
                               ImVec4 bearCol, bool tooltip) {
  ImDrawList* Draw_list = ImPlot::GetPlotDrawList();
  // calc real value width
  double half_width =
      count > 1 ? (model->getDate(1) - model->getDate(0)) * width_percent
                : width_percent;
  hovered_index_ = -1;
  if (ImPlot::IsPlotHovered()) {
    ImPlotPoint mouse = ImPlot::GetPlotMousePos();
    int idx = nearest_index(model->getDates(), mouse.x);
    if (idx != -1) {
      hovered_index_ = idx;
      const auto candle = model->getCandle(idx);
      const auto x = model->getDate(idx);
      const auto close_point = ImPlot::PlotToPixels(x, candle.close);
      const float tool_l = ImPlot::PlotToPixels(x - half_width * 1.5, candle.close).x;
      const float tool_r = ImPlot::PlotToPixels(x + half_width * 1.5, candle.close).x;
      const float tool_t = ImPlot::GetPlotPos().y;
      const float tool_b = tool_t + ImPlot::GetPlotSize().y;
      const float plot_l = ImPlot::GetPlotPos().x;
      const float plot_r = plot_l + ImPlot::GetPlotSize().x;
      ImPlot::PushPlotClipRect();
      Draw_list->AddRectFilled(ImVec2(tool_l, tool_t), ImVec2(tool_r, tool_b),
                               IM_COL32(128, 128, 128, 48));
      Draw_list->AddLine(ImVec2(close_point.x, tool_t),
                         ImVec2(close_point.x, tool_b),
                         IM_COL32(198, 176, 72, 180), 1.0f);
      Draw_list->AddLine(ImVec2(plot_l, close_point.y),
                         ImVec2(plot_r, close_point.y),
                         IM_COL32(102, 168, 255, 120), 1.0f);
      ImPlot::PopPlotClipRect();

      if (tooltip) {
        ImGui::BeginTooltip();
        char buff[32];
        ImPlot::FormatDate(ImPlotTime::FromDouble(model->getDate(idx)), buff, 32,
                           ImPlotDateFmt_DayMoYr, ImPlot::GetStyle().UseISO8601);
        ImGui::Text("Day:    %s", buff);
        ImGui::Text("Open:   $%.2f", candle.open);
        ImGui::Text("High:   $%.2f", candle.high);
        ImGui::Text("Low:    $%.2f", candle.low);
        ImGui::Text("Close:  $%.2f", candle.close);
        ImGui::Text("Volume: %.0f", candle.volume);
        ImGui::Text("Change: %+.2f (%+.2f%%)", candle.close - candle.open,
                    candle.open == 0.0 ? 0.0
                                       : ((candle.close - candle.open) / candle.open) *
                                             100.0);
        ImGui::EndTooltip();
      }
    }
  }

  // begin plot item
  if (ImPlot::BeginItem(model->getTickerSymbol().c_str())) {
    // override legend icon color
    ImPlot::GetCurrentItem()->Color = IM_COL32(64, 64, 64, 255);
    // fit data if requested
    if (ImPlot::FitThisFrame()) {
      for (int i = 0; i < count; ++i) {
        ImPlot::FitPoint(
            ImPlotPoint(model->getDate(i), model->getCandle(i).low));
        ImPlot::FitPoint(
            ImPlotPoint(model->getDate(i), model->getCandle(i).high));
      }
    }
    // render data
    for (int i = 0; i < count; ++i) {
      ImVec2 open_pos = ImPlot::PlotToPixels(model->getDate(i) - half_width,
                                             model->getCandle(i).open);
      ImVec2 close_pos = ImPlot::PlotToPixels(model->getDate(i) + half_width,
                                              model->getCandle(i).close);
      ImVec2 low_pos =
          ImPlot::PlotToPixels(model->getDate(i), model->getCandle(i).low);
      ImVec2 high_pos =
          ImPlot::PlotToPixels(model->getDate(i), model->getCandle(i).high);
      ImU32 color = ImGui::GetColorU32(
          model->getCandle(i).open > model->getCandle(i).close ? bearCol
                                                               : bullCol);
      Draw_list->AddLine(low_pos, high_pos, color);
      Draw_list->AddRectFilled(open_pos, close_pos, color);
    }

    // end plot item
    ImPlot::EndItem();
  }
}

void CandleChart::DrawCandleChart() {
  static bool tooltip = false;
  static auto bullCol = ImVec4(0.000f, 1.000f, 0.441f, 1.000f);
  static auto bearCol = ImVec4(0.853f, 0.050f, 0.310f, 1.000f);

  ImGui::ColorEdit4("##Bull", &bullCol.x, ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine();
  ImGui::ColorEdit4("##Bear", &bearCol.x, ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine();
  ImGui::Checkbox("Show Tooltip", &tooltip);

  if (ImGui::BeginPopupContextItem()) {
    ImGui::Text("Edit name:");
    if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
    ImGui::EndPopup();
  }

  DrawReadoutCard();

  ImPlot::GetStyle().UseLocalTime = true;

  if (ImPlot::BeginSubplots("##priceHistoryChart", 2, 1, ImVec2(-1, -1),
                            ImPlotSubplotFlags_LinkAllX)) {
    if (ImPlot::BeginPlot(quoteDetails.c_str(), ImVec2(-1, -1), 0)) {
      ImPlot::SetupAxisFormat(ImAxis_Y1, "$%.2f");
      ImPlot::SetupAxes("Date", "Price", ImPlotAxisFlags_AutoFit,
                        ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit);

      if (model->isActive()) {
        ImPlot::SetupAxesLimits(
            0, 100, model->getLowBound(), model->getHighBound());
        DrawCandles(0.25, model->getNumCandles(), bullCol, bearCol, tooltip);
        DrawOverlayMarkers();
      }
      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("##volume", ImVec2(-1, 200.f))) {
      ImPlot::SetupAxes("Date", "Volume",
                        ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoLabel,
                        ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_AutoFit |
                            ImPlotAxisFlags_RangeFit);
      ImPlot::SetupLegend(ImPlotLocation_NorthEast, ImPlotLegendFlags_None);
      auto size = (int)model->getVolumeVector().size();
      ImPlot::PlotBars("Volume", model->getDates().data(),
                       model->getVolumeVector().data(), size, 0.5f);
      ImPlot::EndPlot();
    }

    ImPlot::EndSubplots();
  }
}

void CandleChart::fetchData(const std::string &ticker, tda::PeriodType ptype,
                            int period_amt, tda::FrequencyType ftype,
                            int freq_amt, bool ext) {
  model->fetchPriceHistory(ticker, ptype, period_amt, ftype, freq_amt, ext);
  quoteDetails = model->getQuoteDetails();
}

void CandleChart::importModel(std::shared_ptr<ChartModel> newModel) {
  this->model = newModel;
}

void CandleChart::Update() { DrawCandleChart(); }
}  // namespace premia
