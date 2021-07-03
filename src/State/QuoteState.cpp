//  QuoteState Class
#include "QuoteState.hpp"
#include "StartState.hpp"

QuoteState QuoteState::m_QuoteState;

void QuoteState::init( SDL_Renderer *pRenderer, SDL_Window *pWindow )
{
    this->pRenderer = pRenderer;
    this->pWindow = pWindow;
    this->tda_data_interface = boost::make_shared<tda::TDAmeritrade>(tda::QUOTE);
    this->setQuote( "TLT" );

    SDL_Color fontColor = { 255, 255, 255 };
    tickerFont = TTF_OpenFont( "../assets/arial.ttf", 52 );
    tickerTexture.loadFromRenderedText( pRenderer, tickerFont, this->quoteData->getQuoteVariable("symbol"), fontColor );

    TTF_CloseFont( tickerFont );
    tickerFont = TTF_OpenFont( "../assets/arial.ttf", 32 );
    descTexture.loadFromRenderedText( pRenderer, tickerFont, this->quoteData->getQuoteVariable("description"), fontColor );

    ImGui::CreateContext();
	ImGuiSDL::Initialize(pRenderer, 782, 543);
    ImGui::StyleColorsClassic();

}

void QuoteState::cleanup()
{
    TTF_CloseFont( tickerFont );

    printf("QuoteState Cleanup\n");
}

void QuoteState::pause()
{
    printf("QuoteState Pause\n");
}

void QuoteState::resume()
{
    printf("QuoteState Resume\n");
}

void QuoteState::setQuote( std::string ticker )
{
    this->quoteData = tda_data_interface->createQuote( ticker );
}

void QuoteState::handleEvents( Manager* premia )
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
    ImGui_ImplSDL2_NewFrame( pWindow );
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
