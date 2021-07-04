//  QuoteState Class
#include "QuoteState.hpp"
#include "StartState.hpp"

QuoteState QuoteState::m_QuoteState;

void QuoteState::init( SDL_Renderer *pRenderer, SDL_Window *pWindow )
{
    this->pRenderer = pRenderer;
    this->pWindow = pWindow;
    tda_data_interface = boost::make_shared<tda::TDAmeritrade>(tda::QUOTE);
    setQuote( "TLT" );

    SDL_Color fontColor = { 255, 255, 255 };
    tickerFont = TTF_OpenFont( "../assets/arial.ttf", 52 );
    tickerTexture.loadFromRenderedText( pRenderer, tickerFont, quoteData->getQuoteVariable("symbol"), fontColor );

    TTF_CloseFont( tickerFont );
    tickerFont = TTF_OpenFont( "../assets/arial.ttf", 32 );
    descTexture.loadFromRenderedText( pRenderer, tickerFont, quoteData->getQuoteVariable("description"), fontColor );

    ImGui::CreateContext();
	ImGuiSDL::Initialize(pRenderer, 782, 543);
    ImGui::StyleColorsClassic();

    candleVector = priceHistoryData->getCandleVector();

    for ( auto& vector_it: candleVector )
    {
        std::cout << "open: " << vector_it.openClose.first << "\n";
        std::cout << "close: " << vector_it.openClose.second << "\n";
        std::cout << "high: " << vector_it.highLow.first << "\n";
        std::cout << "low: " << vector_it.highLow.second << "\n";
        std::cout << "volume: " << vector_it.volume << "\n";
        std::cout << "datetime: " << vector_it.datetime << "\n";
    }

}

void QuoteState::cleanup()
{
    TTF_CloseFont( tickerFont );

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
    quoteData = tda_data_interface->createQuote( ticker );
    priceHistoryData = tda_data_interface->createPriceHistory( ticker );
}

void QuoteState::handleEvents( Manager* premia )
{
    int wheel = 0;
    SDL_Event event;
    SDL_Color fontColor = { 255, 255, 255 };

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
                    case SDLK_SPACE:
                        premia->change( StartState::instance() );
                        break;
                    default:
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

void QuoteState::update( Manager* game )
{    
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT), ImGuiCond_Once);
    
    std::string title_string = this->quoteData->getQuoteVariable("symbol") + " - " + this->quoteData->getQuoteVariable("description");
    if (!ImGui::Begin(  title_string.c_str(), NULL, ImGuiWindowFlags_MenuBar ))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

    // Menu Bar
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Trade"))
        {
            ImGui::MenuItem("Main menu bar");
            ImGui::EndMenu();
        }
        if ( ImGui::BeginMenu("Analyze"))
        {
            ImGui::MenuItem("Risk Contribution");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Research"))
        {
            ImGui::MenuItem("research menu bar");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools"))
        {
            ImGui::MenuItem("tools menu bar");
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    std::string detailed_quote = "Exchange: " + quoteData->getQuoteVariable("exchangeName") +
                                 "\nBid: $" + quoteData->getQuoteVariable("bidPrice") + " - Size: " + quoteData->getQuoteVariable("bidSize") +
                                 "\nAsk: $" + quoteData->getQuoteVariable("askPrice") + " - Size: " + quoteData->getQuoteVariable("askSize") +
                                 "\nOpen: $" + quoteData->getQuoteVariable("openPrice") +
                                 "\nClose: $" + quoteData->getQuoteVariable("closePrice") +
                                 "\n52 Week High: $" + quoteData->getQuoteVariable("52WkHigh") +
                                 "\n52 Week Low: $" + quoteData->getQuoteVariable("52WkLow") +
                                 "\nTotal Volume: " + quoteData->getQuoteVariable("totalVolume");

    ImGui::Text( "%s", detailed_quote.c_str() );
    ImGui::Spacing();

    // ===== drawable chart ====== //

    static ImVector<ImVec2> points;
    static ImVec2 scrolling(0.0f, 0.0f);
    static bool opt_enable_grid = true;
    static bool opt_enable_context_menu = true;
    static bool adding_line = false;

    ImGui::Checkbox("Enable grid", &opt_enable_grid);

    // Typically you would use a BeginChild()/EndChild() pair to benefit from a clipping region + own scrolling.
    // Here we demonstrate that this can be replaced by simple offsetting + custom drawing + PushClipRect/PopClipRect() calls.
    // To use a child window instead we could use, e.g:
    //      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));      // Disable padding
    //      ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(50, 50, 50, 255));  // Set a background color
    //      ImGui::BeginChild("canvas", ImVec2(0.0f, 0.0f), true, ImGuiWindowFlags_NoMove);
    //      ImGui::PopStyleColor();
    //      ImGui::PopStyleVar();
    //      [...]
    //      ImGui::EndChild();

    // Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
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
        const float GRID_STEP = 64.0f;
        for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
            draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
        for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
            draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
    }
    for (int n = 0; n < points.Size; n += 2)
        draw_list->AddLine(ImVec2(origin.x + points[n].x, origin.y + points[n].y), ImVec2(origin.x + points[n + 1].x, origin.y + points[n + 1].y), IM_COL32(255, 255, 0, 255), 2.0f);
    draw_list->PopClipRect();

    // ====================================== //
    
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

    ImGui::End();

    SDL_RenderClear( this->pRenderer );
}

void QuoteState::draw( Manager* game )
{
    // fill window bounds
    int w = 1920, h = 1080;
    SDL_SetRenderDrawColor( this->pRenderer, 55, 55, 55, 0 );
    SDL_GetWindowSize( this->pWindow, &w, &h );
    SDL_Rect f = {0, 0, 1920, 1080};
    SDL_RenderFillRect( this->pRenderer, &f );

    ImGui::Render();
    ImGuiSDL::Render(ImGui::GetDrawData());

    //tickerTexture.render( pRenderer, 10, 0 );
    //descTexture.render( pRenderer, 10, 50 );

    SDL_RenderPresent( this->pRenderer );
}
