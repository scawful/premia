//  QuoteState Class
#include "QuoteState.hpp"
#include "StartState.hpp"
#include "OptionState.hpp"
#include "Layout/Menu.hpp"

QuoteState QuoteState::m_QuoteState;

void QuoteState::initCandleArrays()
{
    size_t vecSize = candleVector.size();
    double xs[vecSize];
    double highs[vecSize];
    double lows[vecSize];
    double opens[vecSize];
    double closes[vecSize];

    for ( int i = 0; i < candleVector.size(); i++ )
    {
        double new_dt = boost::lexical_cast<double>(candleVector[i].raw_datetime);
        new_dt *= 0.001;
        xs[i] = new_dt;
        highs[i] = candleVector[i].highLow.first;
        lows[i] = candleVector[i].highLow.second;
        opens[i] = candleVector[i].openClose.first;
        closes[i] = candleVector[i].openClose.second;
    }

}

void QuoteState::setDetailedQuote( std::string ticker )
{
    tda::Quote quote = premia->tda_client.createQuote( ticker );
    std::cout << premia->tda_client.getBaseUrl() << std::endl;
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

void QuoteState::init(Manager *premia)
{
    this->premia = premia;
    setQuote("TLT");

    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui::StyleColorsClassic();

    for ( int i = 0; i < candleVector.size(); i++ )
    {
        volumeVector.push_back( candleVector[i].volume );
    }
}

void QuoteState::cleanup()
{
    SDL_Log("QuoteState Cleanup\n");
}

void QuoteState::pause()
{
    SDL_Log("QuoteState Pause\n");
}

void QuoteState::resume()
{
    SDL_Log("QuoteState Resume\n");
}

void QuoteState::setQuote( std::string ticker )
{
    quoteData = premia->tda_client.createQuote( ticker );
    priceHistoryData = premia->tda_client.createPriceHistory( ticker, tda::PeriodType::YEAR, 1, tda::FrequencyType::DAILY, 1, true );

    detailed_quote = "Exchange: " + quoteData.getQuoteVariable("exchangeName") +
                                 "\nBid: $" + quoteData.getQuoteVariable("bidPrice") + " - Size: " + quoteData.getQuoteVariable("bidSize") +
                                 "\nAsk: $" + quoteData.getQuoteVariable("askPrice") + " - Size: " + quoteData.getQuoteVariable("askSize") +
                                 "\nOpen: $" + quoteData.getQuoteVariable("openPrice") +
                                 "\nClose: $" + quoteData.getQuoteVariable("closePrice") +
                                 "\n52 Week High: $" + quoteData.getQuoteVariable("52WkHigh") +
                                 "\n52 Week Low: $" + quoteData.getQuoteVariable("52WkLow") +
                                 "\nTotal Volume: " + quoteData.getQuoteVariable("totalVolume");

    title_string = quoteData.getQuoteVariable("symbol") + " - " + quoteData.getQuoteVariable("description");

    ticker_symbol = ticker;

    candleVector = priceHistoryData.getCandleVector();

    // for ( auto& vector_it: candleVector )
    // {
    //     std::cout << "open: " << vector_it.openClose.first << "\n";
    //     std::cout << "close: " << vector_it.openClose.second << "\n";
    //     std::cout << "high: " << vector_it.highLow.first << "\n";
    //     std::cout << "low: " << vector_it.highLow.second << "\n";
    //     std::cout << "volume: " << vector_it.volume << "\n";
    //     std::cout << "datetime: " << vector_it.datetime << "\n";
    //     std::cout << "raw_datetime: " << vector_it.raw_datetime << "\n";
    // }
}

template <typename T>
inline T RandomRange(T min, T max) 
{
    T scale = rand() / (T) RAND_MAX;
    return min + scale * ( max - min );
}

template <typename T>
int BinarySearch(const T* arr, int l, int r, T x) 
{
    if (r >= l) 
    {
        int mid = l + (r - l) / 2;
        if (arr[mid] == x)
            return mid;
        if (arr[mid] > x)
            return BinarySearch(arr, l, mid - 1, x);
        return BinarySearch(arr, mid + 1, r, x);
    }
    return -1;
}

void QuoteState::createCandleChart( float width_percent, int count, ImVec4 bullCol, ImVec4 bearCol, bool tooltip )
{
    size_t vecSize = candleVector.size();
    double xs[vecSize];
    double highs[vecSize];
    double lows[vecSize];
    double opens[vecSize];
    double closes[vecSize];

    for ( int i = 0; i < candleVector.size(); i++ )
    {
        double new_dt = boost::lexical_cast<double>(candleVector[i].raw_datetime);
        new_dt *= 0.001;
        xs[i] = new_dt;
        highs[i] = candleVector[i].highLow.first;
        lows[i] = candleVector[i].highLow.second;
        opens[i] = candleVector[i].openClose.first;
        closes[i] = candleVector[i].openClose.second;
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
        int idx = BinarySearch(xs, 0, count - 1, mouse.x);
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
    if ( ImPlot::BeginItem( priceHistoryData.getPriceHistoryVariable("symbol").c_str() ) ) 
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

void QuoteState::handleEvents()
{
    int wheel = 0;
    SDL_Event event;

    ImGuiIO& io = ImGui::GetIO();

    while ( SDL_PollEvent(&event) ) 
    {
        switch ( event.type ) 
        {
            case SDL_KEYDOWN:
                switch ( event.key.keysym.sym )
                {
                    case SDLK_ESCAPE:
                        premia->quit();
                        break;
                    case SDLK_LEFT:
                        premia->change( StartState::instance() );
                        break;
                    case SDLK_RIGHT:
                        premia->change( OptionState::instance() );
                        break;
                    default:
                        break;
                }
                break;

            case SDL_TEXTINPUT:
                io.AddInputCharactersUTF8(event.text.text);
                break;
            
            case SDL_KEYUP:
            {
                int key = event.key.keysym.scancode;
                IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
                io.KeysDown[key] = (event.type == SDL_KEYDOWN);
                io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
                io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
                io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
                break;
            }

            case SDL_WINDOWEVENT:
                switch ( event.window.event ) 
                {
                    case SDL_WINDOWEVENT_CLOSE:   // exit game
                        premia->quit();
                        break;
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        io.DisplaySize.x = static_cast<float>(event.window.data1);
					    io.DisplaySize.y = static_cast<float>(event.window.data2);
                        break;

                    default:
                        break;
                }
                break;      

            case SDL_MOUSEWHEEL:
                wheel = event.wheel.y;
                break;

            default:
                break;
        }
    }

    int mouseX, mouseY;
    const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

    io.DeltaTime = 1.0f / 60.0f;
    io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
    io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
    io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
    io.MouseWheel = static_cast<float>(wheel);
}

void QuoteState::update()
{    
    draw_imgui_menu( premia, title_string );

    static int period_type = 0, period_amount = 1, frequency_type  = 0, frequency_amount = 1;
    static char buf[64] = "";
    ImGui::Text("Symbol: ");
    ImGui::SameLine(); ImGui::InputText("##symbol", buf, 64, ImGuiInputTextFlags_CharsUppercase );
    ImGui::SameLine(); 
    if ( ImGui::Button("Search") )
    {
        if ( strcmp(buf, "") != 0 )
        {
            // quoteData.reset();
            // priceHistoryData.reset();
            quoteData = premia->tda_client.createQuote( buf );
            priceHistoryData = premia->tda_client.createPriceHistory( buf, tda::PeriodType(period_type), period_amount, 
                                                                 tda::FrequencyType(frequency_type), frequency_amount, true);
            candleVector = priceHistoryData.getCandleVector();
            setDetailedQuote( buf );
            ticker_symbol = buf;
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
        priceHistoryData = premia->tda_client.createPriceHistory(ticker_symbol, tda::PeriodType(period_type), period_amount, 
                                                                 tda::FrequencyType(frequency_type), frequency_amount, true);
        candleVector = priceHistoryData.getCandleVector();
    }  

    if (ImGui::BeginPopupContextItem())
    {
        ImGui::Text("Edit name:");
        //ImGui::InputText("##edit", name, IM_ARRAYSIZE(name));
        if (ImGui::Button("Close"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }


    // ImPlot::GetStyle().UseLocalTime = true;
    // ImPlot::SetNextPlotFormatY("$%.2f");
    // ImPlot::SetNextAxesLimits((1609740000000 * 0.001), (1625267160000 * 0.001), 
    //                             boost::lexical_cast<double>(quoteData->getQuoteVariable("52WkLow")), 
    //                             boost::lexical_cast<double>(quoteData->getQuoteVariable("52WkHigh")), ImGuiCond_Once);

    // if (ImPlot::BeginPlot("Candlestick Chart",NULL,NULL,ImVec2(-1,0),0,
    //                         ImPlotAxisFlags_Time,
    //                         ImPlotAxisFlags_AutoFit|ImPlotAxisFlags_RangeFit|ImPlotAxisFlags_LockMax)) 
    // {
    //     createCandleChart( 0.25, 218, bullCol, bearCol, tooltip );
    //     ImPlot::EndPlot();
    // }

    //ImGui::PlotHistogram("Volume", volumeVector, volumeVector.size(), 0, NULL, 0.0f, 1.0f, ImVec2(0, 80.0f));

    // ===========================  // 

        /*
        static double xs1[101], ys1[101], ys2[101], ys3[101];
        srand(0);
        for (int i = 0; i < 101; ++i) {
            xs1[i] = (float)i;
            ys1[i] = RandomRange(400.0,450.0);
            ys2[i] = RandomRange(275.0,350.0);
            ys3[i] = RandomRange(150.0,225.0);
        }

        for ( int i = 0; i < candleVector.size(); i++ )
        {
            
        }

        static bool show_lines = true;
        static bool show_fills = true;
        static float fill_ref = 0;
        static int shade_mode = 0;
        ImGui::Checkbox("Lines",&show_lines); ImGui::SameLine();
        ImGui::Checkbox("Fills",&show_fills);
        if (show_fills) {
            ImGui::SameLine();
            if (ImGui::RadioButton("To -INF",shade_mode == 0))
                shade_mode = 0;
            ImGui::SameLine();
            if (ImGui::RadioButton("To +INF",shade_mode == 1))
                shade_mode = 1;
            ImGui::SameLine();
            if (ImGui::RadioButton("To Ref",shade_mode == 2))
                shade_mode = 2;
            if (shade_mode == 2) {
                ImGui::SameLine();
                ImGui::SetNextItemWidth(100);
                ImGui::DragFloat("##Ref",&fill_ref, 1, -100, 500);
            }
        }


        ImPlot::SetNextPlotLimits(0,100,0,500);
        if (ImPlot::BeginPlot("Stock Prices", "Days", "Price")) {
            if (show_fills) {
                ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
                ImPlot::PlotShaded("EMA(9)", xs1, ys1, 101, shade_mode == 0 ? -INFINITY : shade_mode == 1 ? INFINITY : fill_ref);
                ImPlot::PlotShaded("EMA(50)", xs1, ys2, 101, shade_mode == 0 ? -INFINITY : shade_mode == 1 ? INFINITY : fill_ref);
                ImPlot::PlotShaded("EMA(100)", xs1, ys3, 101, shade_mode == 0 ? -INFINITY : shade_mode == 1 ? INFINITY : fill_ref);
                ImPlot::PopStyleVar();
            }
            if (show_lines) {
                ImPlot::PlotLine("EMA(9)", xs1, ys1, 101);
                ImPlot::PlotLine("EMA(50)", xs1, ys2, 101);
                ImPlot::PlotLine("EMA(100)", xs1, ys3, 101);
            }
            ImPlot::EndPlot();
        }
        */

    // ===== drawable chart ====== //

        /*
        static ImVector<ImVec2> points;
        static ImVec2 scrolling(0.0f, 0.0f);
        static bool opt_enable_grid = true;
        static bool opt_enable_context_menu = true;
        static bool adding_line = false;

        ImGui::Checkbox("Enable grid", &opt_enable_grid);

        // Using InvisibleButton() as a convenience 1) 
        // it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
        ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
        ImVec2 canvas_sz = ImVec2( 600.f, 250.f );
        // ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
        // if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
        // if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
        ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

        // Draw border and background color
        ImGuiIO& io = ImGui::GetIO();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
        draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

        // This will catch our interactions
        ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
        const bool is_hovered = ImGui::IsItemHovered(); // Hovered
        const bool is_active = ImGui::IsItemActive();   // Held
        const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin
        const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

        // Add first and second point
        if (is_hovered && !adding_line && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            points.push_back(mouse_pos_in_canvas);
            points.push_back(mouse_pos_in_canvas);
            adding_line = true;
        }

        if (adding_line)
        {
            points.back() = mouse_pos_in_canvas;
            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
                adding_line = false;
        }

        // Pan (we use a zero mouse threshold when there's no context menu)
        // You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
        const float mouse_threshold_for_pan = opt_enable_context_menu ? -1.0f : 0.0f;
        if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
        {
            scrolling.x += io.MouseDelta.x;
            scrolling.y += io.MouseDelta.y;
        }

        // Context menu (under default mouse threshold)
        ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
        if (opt_enable_context_menu && ImGui::IsMouseReleased(ImGuiMouseButton_Right) && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
            ImGui::OpenPopupOnItemClick("context");
        if (ImGui::BeginPopup("context"))
        {
            if (adding_line)
                points.resize(points.size() - 2);
            adding_line = false;
            if (ImGui::MenuItem("Remove one", NULL, false, points.Size > 0)) { points.resize(points.size() - 2); }
            if (ImGui::MenuItem("Remove all", NULL, false, points.Size > 0)) { points.clear(); }
            ImGui::EndPopup();
        }

        // Draw grid + all lines in the canvas
        draw_list->PushClipRect(canvas_p0, canvas_p1, true);
        if (opt_enable_grid)
        {
            const float GRID_STEP = 32.0f;
            for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
                draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
            for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
                draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
        }

        for (int n = 0; n < points.Size; n += 2)
            draw_list->AddLine(ImVec2(origin.x + points[n].x, origin.y + points[n].y), ImVec2(origin.x + points[n + 1].x, origin.y + points[n + 1].y), IM_COL32(255, 255, 0, 255), 2.0f);
        draw_list->PopClipRect();
        */

    // ====================================== //
    
        /*
        static bool animate = true;
        static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };

        // Use functions to generate output
        // FIXME: This is rather awkward because current plot API only pass in indices.
        // We probably want an API passing floats and user provide sample rate/count.
        struct Funcs
        {
            static float Sin(void*, int i) { return sinf(i * 0.1f); }
            static float Saw(void*, int i) { return (i & 1) ? 1.0f : -1.0f; }
        };
        static int func_type = 0, display_count = 70;
        ImGui::Separator();
        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
        ImGui::Combo("func", &func_type, "Daily\0Weekly\0Monthly\0");
        ImGui::SameLine();
        ImGui::SliderInt("Time Period", &display_count, 1, 360);
        float (*func)(void*, int) = (func_type == 0) ? Funcs::Sin : Funcs::Saw;
        ImGui::PlotLines("Momentum", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
        ImGui::PlotHistogram("MACD", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
        ImGui::PlotHistogram("Volume", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0, 80.0f));
        */

    // ====================================== // 

    ImGui::End();

    SDL_RenderClear( premia->pRenderer );
}

void QuoteState::draw()
{
    // fill window bounds
    int w = 1920, h = 1080;
    SDL_SetRenderDrawColor( premia->pRenderer, 55, 55, 55, 0 );
    SDL_GetWindowSize( premia->pWindow, &w, &h );
    SDL_Rect f = {0, 0, 1920, 1080};
    SDL_RenderFillRect( premia->pRenderer, &f );

    ImGui::Render();
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

    SDL_RenderPresent( premia->pRenderer );
}
