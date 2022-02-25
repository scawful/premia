#include "CandleChartFrame.hpp"

/**
 * @brief Initialize the candle arrays 
 * @author @scawful
 * 
 * @todo implement this in the build
 * 
 */
void CandleChartFrame::init_candles()
{
    size_t vecSize = candles.size();
    double xs[vecSize];
    double highs[vecSize];
    double lows[vecSize];
    double opens[vecSize];
    double closes[vecSize];

    for ( int i = 0; i < candles.size(); i++ )
    {
        double new_dt = boost::lexical_cast<double>(candles[i].raw_datetime);
        new_dt *= 0.001;
        xs[i] = new_dt;
        highs[i] = candles[i].highLow.first;
        lows[i] = candles[i].highLow.second;
        opens[i] = candles[i].openClose.first;
        closes[i] = candles[i].openClose.second;
    }

}

/**
 * @brief Rewrite the details of the instrument quote
 * @author @scawful
 * 
 */
void CandleChartFrame::init_quote_details()
{
    tda::Quote quote = premia->tda_client.createQuote( ticker_symbol );
    detailed_quote = "Exchange: " + quote.getQuoteVariable("exchangeName") +
                                 "\nBid: $" + quote.getQuoteVariable("bidPrice") + " - Size: " + quote.getQuoteVariable("bidSize") +
                                 "\nAsk: $" + quote.getQuoteVariable("askPrice") + " - Size: " + quote.getQuoteVariable("askSize") +
                                 "\nOpen: $" + quote.getQuoteVariable("openPrice") +
                                 "\nClose: $" + quote.getQuoteVariable("closePrice") +
                                 "\n52 Week High: $" + quote.getQuoteVariable("52WkHigh") +
                                 "\n52 Week Low: $" + quote.getQuoteVariable("52WkLow") +
                                 "\nTotal Volume: " + quote.getQuoteVariable("totalVolume");

    title_string = quote.getQuoteVariable("symbol") + " - " + quote.getQuoteVariable("description");
}

/**
 * @brief Load an instrument to be charted
 * @author @scawful
 * 
 * @param ticker 
 */
void CandleChartFrame::init_instrument(std::string ticker)
{
    quote = premia->tda_client.createQuote( ticker );
    price_history_data = premia->tda_client.createPriceHistory( ticker, tda::PeriodType::YEAR, 1, tda::FrequencyType::DAILY, 1, true );
    candles = price_history_data.getCandleVector();

    detailed_quote = "Exchange: " + quote.getQuoteVariable("exchangeName") +
                                 "\nBid: $" + quote.getQuoteVariable("bidPrice") + " - Size: " + quote.getQuoteVariable("bidSize") +
                                 "\nAsk: $" + quote.getQuoteVariable("askPrice") + " - Size: " + quote.getQuoteVariable("askSize") +
                                 "\nOpen: $" + quote.getQuoteVariable("openPrice") +
                                 "\nClose: $" + quote.getQuoteVariable("closePrice") +
                                 "\n52 Week High: $" + quote.getQuoteVariable("52WkHigh") +
                                 "\n52 Week Low: $" + quote.getQuoteVariable("52WkLow") +
                                 "\nTotal Volume: " + quote.getQuoteVariable("totalVolume");

    title_string = quote.getQuoteVariable("symbol") + " - " + quote.getQuoteVariable("description");
    ticker_symbol = ticker;
    for ( int i = 0; i < candles.size(); i++ )
    {
        volume.push_back( candles[i].volume );
    }
}

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
int CandleChartFrame::binary_search(const double* arr, int l, int r, double x) 
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
void CandleChartFrame::build_candle_chart( float width_percent, int count, ImVec4 bullCol, ImVec4 bearCol, bool tooltip )
{
    size_t vecSize = candles.size();
    double xs[vecSize];
    double highs[vecSize];
    double lows[vecSize];
    double opens[vecSize];
    double closes[vecSize];

    for ( int i = 0; i < candles.size(); i++ )
    {
        double new_dt = boost::lexical_cast<double>(candles[i].raw_datetime);
        new_dt *= 0.001;
        xs[i] = new_dt;
        highs[i] = candles[i].highLow.first;
        lows[i] = candles[i].highLow.second;
        opens[i] = candles[i].openClose.first;
        closes[i] = candles[i].openClose.second;
    }

    // get ImGui window DrawList
    ImDrawList* draw_list = ImPlot::GetPlotDrawList();
    
    // calc real value width
    double half_width = count > 1 ? (xs[1] - xs[0]) * width_percent : width_percent;

    // custom tool
    if ( ImPlot::IsPlotHovered() && tooltip ) 
    {
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
        int idx = binary_search(xs, 0, count - 1, mouse.x);
        // render tool tip (won't be affected by plot clip rect)
        if (idx != -1) {
            ImGui::BeginTooltip();
            char buff[32];
            ImPlot::FormatDate(ImPlotTime::FromDouble(xs[idx]),buff,32,ImPlotDateFmt_DayMoYr,ImPlot::GetStyle().UseISO8601);
            ImGui::Text("Day:   %s",  buff);
            ImGui::Text("Open:  $%.2f", opens[idx]);
            ImGui::Text("Close: $%.2f", closes[idx]);
            ImGui::Text("Low:   $%.2f", lows[idx]);
            ImGui::Text("High:  $%.2f", highs[idx]);
            ImGui::EndTooltip();
        }
    }

    // begin plot item
    if ( ImPlot::BeginItem( price_history_data.getPriceHistoryVariable("symbol").c_str() ) ) 
    {
        // override legend icon color
        ImPlot::GetCurrentItem()->Color = IM_COL32(64,64,64,255);
        // fit data if requested
        if (ImPlot::FitThisFrame()) {
            for (int i = 0; i < count; ++i) {
                ImPlot::FitPoint(ImPlotPoint(xs[i], lows[i]));
                ImPlot::FitPoint(ImPlotPoint(xs[i], highs[i]));
            }
        }
        // render data
        for (int i = 0; i < count; ++i) 
        {
            ImVec2 open_pos  = ImPlot::PlotToPixels(xs[i] - half_width, opens[i]);
            ImVec2 close_pos = ImPlot::PlotToPixels(xs[i] + half_width, closes[i]);
            ImVec2 low_pos   = ImPlot::PlotToPixels(xs[i], lows[i]);
            ImVec2 high_pos  = ImPlot::PlotToPixels(xs[i], highs[i]);
            ImU32 color      = ImGui::GetColorU32(opens[i] > closes[i] ? bearCol : bullCol);
            draw_list->AddLine(low_pos, high_pos, color);
            draw_list->AddRectFilled(open_pos, close_pos, color);
        }

        // end plot item
        ImPlot::EndItem();
    }
}

/**
 * @brief 
 * 
 */
void CandleChartFrame::draw_chart()
{

}

/**
 * @brief Construct a new Candle Chart Frame:: Candle Chart Frame object
 * @author @scawful
 * 
 */
CandleChartFrame::CandleChartFrame()
{

}

/**
 * @brief 
 * 
 */
void CandleChartFrame::update() 
{
    static int period_type = 0, period_amount = 1, frequency_type  = 0, frequency_amount = 1;

    ImGui::InputText("Enter symbol", &ticker_symbol, ImGuiInputTextFlags_CharsUppercase);
    ImGui::SameLine(); 
    if ( ImGui::Button("Search") )
    {
        if ( strcmp(buf, "") != 0 )
        {
            // quoteData.reset();
            // priceHistoryData.reset();
            quote = premia->tda_client.createQuote( buf );
            price_history_data = premia->tda_client.createPriceHistory( buf, tda::PeriodType(period_type), period_amount, 
                                                                 tda::FrequencyType(frequency_type), frequency_amount, true);
            candles = price_history_data.getCandleVector();
            init_instrument(buf);
        }
    }

    ImGui::Text( "%s", detailed_quote.c_str() );
    ImGui::Spacing();

    // ====== implot chart ======= //

    static bool tooltip = false;
    // ImGui::Checkbox("Show Tooltip", &tooltip);
    // ImGui::SameLine();
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

    if ( ImGui::Button("Apply") ) 
    {
        std::cout << "Change chart settings!" << std::endl;
        price_history_data = premia->tda_client.createPriceHistory(ticker_symbol, tda::PeriodType(period_type), period_amount, 
                                                                 tda::FrequencyType(frequency_type), frequency_amount, true);
        candles = price_history_data.getCandleVector();
    }  

    if (ImGui::BeginPopupContextItem())
    {
        ImGui::Text("Edit name:");
        if (ImGui::Button("Close"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }


    ImPlot::GetStyle().UseLocalTime = true;
    // ImPlot::SetNextPlotFormatY("$%.2f");
    if (ImPlot::BeginPlot("Candlestick Chart",ImVec2(-1,0),0)) 
    {
        ImPlot::SetupAxes("Date","Price");
        ImPlot::SetupAxesLimits(0,100,
                                boost::lexical_cast<double>(quote.getQuoteVariable("52WkLow")), 
                                boost::lexical_cast<double>(quote.getQuoteVariable("52WkHigh")));
        build_candle_chart( 0.25, 218, bullCol, bearCol, tooltip );
        ImPlot::EndPlot();
    }
}
