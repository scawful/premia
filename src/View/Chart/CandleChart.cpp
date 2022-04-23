#include "CandleChart.hpp"

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
int CandleChart::binary_search(const double* arr, int l, int r, double x) 
{
    if (r >= l) 
    {
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
void CandleChart::drawCandles(float width_percent, int count, ImVec4 bullCol, ImVec4 bearCol, bool tooltip)
{
    ImDrawList* draw_list = ImPlot::GetPlotDrawList();
    // calc real value width
    double half_width = count > 1 ? (dates[1] - dates[0]) * width_percent : width_percent;
    // custom tool
    if (ImPlot::IsPlotHovered() && tooltip) {
        ImPlotPoint mouse   = ImPlot::GetPlotMousePos();
        mouse.x             = ImPlot::RoundTime(ImPlotTime::FromDouble(mouse.x), ImPlotTimeUnit_Day).ToDouble();
        float  tool_l       = ImPlot::PlotToPixels(mouse.x - half_width * 1.5, mouse.y).x;
        float  tool_r       = ImPlot::PlotToPixels(mouse.x + half_width * 1.5, mouse.y).x;
        float  tool_t       = ImPlot::GetPlotPos().y;
        float  tool_b       = tool_t + ImPlot::GetPlotSize().y;
        ImPlot::PushPlotClipRect();
        draw_list->AddRectFilled(ImVec2(tool_l, tool_t), ImVec2(tool_r, tool_b), IM_COL32(128,128,128,64));
        ImPlot::PopPlotClipRect();
        // find mouse location index
        int idx = binary_search(dates, 0, count - 1, mouse.x);
        // render tool tip (won't be affected by plot clip rect)
        if (idx != -1) {
            ImGui::BeginTooltip();
            char buff[32];
            ImPlot::FormatDate(ImPlotTime::FromDouble(dates[idx]),buff,32,ImPlotDateFmt_DayMoYr,ImPlot::GetStyle().UseISO8601);
            ImGui::Text("Day:   %s",  buff);
            ImGui::Text("Open:  $%.2f", model.getCandle(idx).open);
            ImGui::Text("Close: $%.2f", model.getCandle(idx).close);
            ImGui::Text("Low:   $%.2f", model.getCandle(idx).low);
            ImGui::Text("High:  $%.2f", model.getCandle(idx).high);
            ImGui::EndTooltip();
        }
    }

    // begin plot item
    if (ImPlot::BeginItem( "symbol" )) {
        // override legend icon color
        ImPlot::GetCurrentItem()->Color = IM_COL32(64,64,64,255);
        // fit data if requested
        if (ImPlot::FitThisFrame()) {
            for (int i = 0; i < count; ++i) {
                ImPlot::FitPoint(ImPlotPoint(dates[i], model.getCandle(i).low));
                ImPlot::FitPoint(ImPlotPoint(dates[i], model.getCandle(i).high));
            }
        }
        // render data
        for (int i = 0; i < count; ++i) {
            ImVec2 open_pos  = ImPlot::PlotToPixels(dates[i] - half_width, model.getCandle(i).open);
            ImVec2 close_pos = ImPlot::PlotToPixels(dates[i] + half_width, model.getCandle(i).close);
            ImVec2 low_pos   = ImPlot::PlotToPixels(dates[i], model.getCandle(i).low);
            ImVec2 high_pos  = ImPlot::PlotToPixels(dates[i], model.getCandle(i).high);
            ImU32 color      = ImGui::GetColorU32(model.getCandle(i).open > model.getCandle(i).close ? bearCol : bullCol);
            draw_list->AddLine(low_pos, high_pos, color);
            draw_list->AddRectFilled(open_pos, close_pos, color);
        }

        // end plot item
        ImPlot::EndItem();
    }
}

void
CandleChart::drawCandleChart()
{
    static int period_type = 0, period_amount = 1, frequency_type  = 0, frequency_amount = 1;

    ImGui::InputText("Enter symbol", &tickerSymbol, ImGuiInputTextFlags_CharsUppercase);
    ImGui::SameLine(); 
    if (ImGui::Button("Search")) {
        if (!tickerSymbol.empty() && !model.isActive()) {
            model.fetchPriceHistory(tickerSymbol, tda::PeriodType(period_type), period_amount, 
                                                  tda::FrequencyType(frequency_type), frequency_amount, true);
        }
    }

    ImGui::Text("%s", model.getQuoteDetails().c_str());
    ImGui::Spacing();

    // ====== implot chart ======= //

    static bool tooltip = false;
    ImGui::Checkbox("Show Tooltip", &tooltip);
    ImGui::SameLine();
    static ImVec4 bullCol = ImVec4(0.000f, 1.000f, 0.441f, 1.000f);
    static ImVec4 bearCol = ImVec4(0.853f, 0.050f, 0.310f, 1.000f);
    ImGui::SameLine(); ImGui::ColorEdit4("##Bull", &bullCol.x, ImGuiColorEditFlags_NoInputs);
    ImGui::SameLine(); ImGui::ColorEdit4("##Bear", &bearCol.x, ImGuiColorEditFlags_NoInputs);

    ImGui::SetNextItemWidth( 75.f );
    ImGui::SameLine(); ImGui::Combo("Period", &period_type, "Day\0Month\0Year\0YTD\0");
    ImGui::SetNextItemWidth( 50.f );
    ImGui::SameLine(); ImGui::Combo("Type | ", &period_amount, " 1\0 2\0 3\0 4\0 5\0 6\0 10\0 15\0 20\0");

    ImGui::SetNextItemWidth( 75.f );
    ImGui::SameLine(); ImGui::Combo("Frequency ", &frequency_type, "Minute\0Daily\0Weekly\0Monthly\0");
    ImGui::SetNextItemWidth( 50.f );
    ImGui::SameLine(); ImGui::Combo("Amount", &frequency_amount, " 1\0 5\0 10\0 15\0 30\0");

    ImGui::SameLine();

    if (ImGui::Button("Apply")) {
        model.fetchPriceHistory(tickerSymbol, tda::PeriodType(period_type), period_amount, 
                                              tda::FrequencyType(frequency_type), frequency_amount, true);
        // init_instrument(ticker_symbol);
        // init_candles(frequency_type);
    }  

    if (ImGui::BeginPopupContextItem()) {
        ImGui::Text("Edit name:");
        if (ImGui::Button("Close"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    ImPlot::GetStyle().UseLocalTime = true;
    if (ImPlot::BeginPlot("Candlestick Chart",ImGui::GetContentRegionAvail(),0))  {
        ImPlot::SetupAxisFormat(ImAxis_Y1, "$%.2f");
        ImPlot::SetupAxes("Date","Price",ImPlotAxisFlags_Time,ImPlotAxisFlags_AutoFit|ImPlotAxisFlags_RangeFit);

        if (model.isActive()) {
            ImPlot::SetupAxesLimits(0,100, boost::lexical_cast<double>(model.getQuote().getQuoteVariable("52WkLow")), 
                                           boost::lexical_cast<double>(model.getQuote().getQuoteVariable("52WkHigh")));
            drawCandles( 0.25, model.getNumCandles(), bullCol, bearCol, tooltip );
        }
        ImPlot::EndPlot();
    }
}


void
CandleChart::update()
{
    drawCandleChart();
}