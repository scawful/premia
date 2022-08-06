#include "CandleChart.hpp"

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
void CandleChart::drawCandles(float width_percent, int count, ImVec4 bullCol,
                              ImVec4 bearCol, bool tooltip) {
  ImDrawList* draw_list = ImPlot::GetPlotDrawList();
  // calc real value width
  double half_width =
      count > 1 ? (model->getDate(1) - model->getDate(0)) * width_percent
                : width_percent;
  // custom tool
  if (ImPlot::IsPlotHovered() && tooltip) {
    ImPlotPoint mouse = ImPlot::GetPlotMousePos();
    mouse.x =
        ImPlot::RoundTime(ImPlotTime::FromDouble(mouse.x), ImPlotTimeUnit_Day)
            .ToDouble();
    float tool_l = ImPlot::PlotToPixels(mouse.x - half_width * 1.5, mouse.y).x;
    float tool_r = ImPlot::PlotToPixels(mouse.x + half_width * 1.5, mouse.y).x;
    float tool_t = ImPlot::GetPlotPos().y;
    float tool_b = tool_t + ImPlot::GetPlotSize().y;
    ImPlot::PushPlotClipRect();
    draw_list->AddRectFilled(ImVec2(tool_l, tool_t), ImVec2(tool_r, tool_b),
                             IM_COL32(128, 128, 128, 64));
    ImPlot::PopPlotClipRect();
    // find mouse location index
    int idx = binary_search(model->getDates(), 0, count - 1, mouse.x);
    // render tool tip (won't be affected by plot clip rect)
    if (idx != -1) {
      ImGui::BeginTooltip();
      char buff[32];
      ImPlot::FormatDate(ImPlotTime::FromDouble(model->getDate(idx)), buff, 32,
                         ImPlotDateFmt_DayMoYr, ImPlot::GetStyle().UseISO8601);
      ImGui::Text("Day:   %s", buff);
      ImGui::Text("Open:  $%.2f", model->getCandle(idx).open);
      ImGui::Text("Close: $%.2f", model->getCandle(idx).close);
      ImGui::Text("Low:   $%.2f", model->getCandle(idx).low);
      ImGui::Text("High:  $%.2f", model->getCandle(idx).high);
      ImGui::EndTooltip();
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
      draw_list->AddLine(low_pos, high_pos, color);
      draw_list->AddRectFilled(open_pos, close_pos, color);
    }

    // end plot item
    ImPlot::EndItem();
  }
}

void CandleChart::drawCandleChart() {
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

  ImPlot::GetStyle().UseLocalTime = true;

  if (ImPlot::BeginSubplots("##priceHistoryChart", 2, 1, ImVec2(-1, -1),
                            ImPlotSubplotFlags_LinkAllX)) {
    if (ImPlot::BeginPlot(quoteDetails.c_str(), ImVec2(-1, -1), 0)) {
      ImPlot::SetupAxisFormat(ImAxis_Y1, "$%.2f");
      ImPlot::SetupAxes("Date", "Price", ImPlotAxisFlags_AutoFit,
                        ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit);

      if (model->isActive()) {
        ImPlot::SetupAxesLimits(
            0, 100,
            boost::lexical_cast<double>(
                model->getQuote().getQuoteVariable("52WkLow")),
            boost::lexical_cast<double>(
                model->getQuote().getQuoteVariable("52WkHigh")));
        drawCandles(0.25, model->getNumCandles(), bullCol, bearCol, tooltip);
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

void CandleChart::update() { drawCandleChart(); }
}  // namespace premia