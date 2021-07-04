//  StartState Class
#include "StartState.hpp"
#include "QuoteState.hpp"

StartState StartState::m_StartState;

void StartState::init( SDL_Renderer *pRenderer, SDL_Window *pWindow )
{
    this->pRenderer = pRenderer;
    this->pWindow = pWindow;
    this->tda_data_interface = boost::make_shared<tda::TDAmeritrade>(tda::QUOTE);

    quotes["SPY"] = tda_data_interface->createQuote( "SPY" );
    quotes["QQQ"] = tda_data_interface->createQuote( "QQQ" );
    quotes["DIA"] = tda_data_interface->createQuote( "DIA" );
    quotes["TLT"] = tda_data_interface->createQuote( "TLT" );
    quotes["IWM"] = tda_data_interface->createQuote( "IWM" );
    quotes["VXX"] = tda_data_interface->createQuote( "VXX" );

    SDL_Color fontColor = { 255, 255, 255 };
    titleFont = TTF_OpenFont( "../assets/arial.ttf", 64 );
    menuFont = TTF_OpenFont( "../assets/arial.ttf", 36 );
    priceFont = TTF_OpenFont ( "../assets/arial.ttf", 24 );
    if ( menuFont == NULL || priceFont == NULL || titleFont == NULL )
    {
        SDL_Log( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
    }
    else
    {
        // if ( !testTexture.loadFromRenderedText( pRenderer, menuFont, newData.ticker, fontColor ) )
        // {
        //     SDL_Log("Failed to render text texture!\n");
        // }

        titleTexture.loadFromRenderedText( pRenderer, titleFont, "Premia Pro", fontColor );
        subtitleTexture.loadFromRenderedText( pRenderer, menuFont, "Risk Parity Analysis and Trading Program", fontColor );

        SDL_Color green_color = {0, 255, 0, 0};
        textures["POWERED"].loadFromRenderedText( pRenderer, priceFont, "powerered by ", fontColor );
        textures["TDA_SHILL"].loadFromRenderedText( pRenderer, priceFont, "TD Ameritrade", green_color );

        std::map<std::string, PTexture>::iterator it;
        for (it = textures.begin(); it != textures.end(); it++)
        {
            it->second.setRenderer( pRenderer );
            it->second.setFontColor( fontColor );
        }

        std::string spy_last_price = "SPY: $" + quotes["SPY"]->getQuoteVariable("lastPrice");
        textures["SPY_LAST_PRICE"].loadFromRenderedText( pRenderer, priceFont, spy_last_price, fontColor );

        std::string qqq_last_price = "QQQ: $" + quotes["QQQ"]->getQuoteVariable("lastPrice");
        textures["QQQ_LAST_PRICE"].loadFromRenderedText( pRenderer, priceFont, qqq_last_price, fontColor );

        std::string dia_last_price = "DIA: $" + quotes["DIA"]->getQuoteVariable("lastPrice");
        textures["DIA_LAST_PRICE"].loadFromRenderedText( pRenderer, priceFont, dia_last_price, fontColor );

        std::string tlt_last_price = "TLT: $" + quotes["TLT"]->getQuoteVariable("lastPrice");
        textures["TLT_LAST_PRICE"].loadFromRenderedText( pRenderer, priceFont, tlt_last_price, fontColor );

        std::string iwm_last_price = "IWM: $" + quotes["IWM"]->getQuoteVariable("lastPrice");
        textures["IWM_LAST_PRICE"].loadFromRenderedText( pRenderer, priceFont, iwm_last_price, fontColor );

        std::string vxx_last_price = "VXX: $" + quotes["VXX"]->getQuoteVariable("lastPrice");
        textures["VXX_LAST_PRICE"].loadFromRenderedText( pRenderer, priceFont, vxx_last_price, fontColor );
    }

    ImGui::CreateContext();
	ImGuiSDL::Initialize(pRenderer, 782, 543);
    ImGui::StyleColorsClassic();


}

void StartState::cleanup()
{
    TTF_CloseFont( titleFont );
    titleFont = NULL;

    TTF_CloseFont( menuFont );
    menuFont = NULL;

    TTF_CloseFont( priceFont );
    priceFont = NULL;

    titleTexture.free();
    subtitleTexture.free();

    std::map<std::string, PTexture>::iterator it;
    for (it = textures.begin(); it != textures.end(); it++)
    {
        it->second.free();
    }

    SDL_Log("StartState Cleanup\n");
}

void StartState::pause()
{
    SDL_Log("StartState Pause\n");
}

void StartState::resume()
{
    SDL_Log("StartState Resume\n");
}

void StartState::handleEvents( Manager* premia )
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
                        premia->change( QuoteState::instance() );
                        break;
                    default:
                        break;
                }
            case SDL_KEYUP:
            {
                int key = event.key.keysym.scancode;
                IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
                io.KeysDown[key] = (event.type == SDL_KEYDOWN);
                io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
                io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
                io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
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

            case SDL_TEXTINPUT:
                io.AddInputCharactersUTF8(event.text.text);
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

void StartState::update( Manager* game )
{
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(200, 200));
    ImGui::ShowDemoWindow();


    SDL_RenderClear(this->pRenderer);
}

void StartState::draw( Manager* game )
{
    // fill window bounds
    int w = 1920, h = 1080;
    SDL_SetRenderDrawColor( pRenderer, 0, 0, 0, 0 );
    SDL_GetWindowSize( pWindow, &w, &h );
    SDL_Rect f = { 0, 0, 1920, 1080 };
    SDL_RenderFillRect( pRenderer, &f );

    titleTexture.render(pRenderer, (SCREEN_WIDTH  - titleTexture.getWidth()) / 2, 10);
    subtitleTexture.render(pRenderer, (SCREEN_WIDTH - subtitleTexture.getWidth()) / 2, 75);
    textures["POWERED"].render(pRenderer, ((SCREEN_WIDTH - textures["POWERED"].getWidth()) / 2) - 50, 125);
    textures["TDA_SHILL"].render(pRenderer, ((SCREEN_WIDTH - textures["POWERED"].getWidth()) / 2) + textures["TDA_SHILL"].getWidth() - 50, 125);
    
    ImGui::Render();
    ImGuiSDL::Render(ImGui::GetDrawData());

    textures["SPY_LAST_PRICE"].render( pRenderer, ((SCREEN_WIDTH - textures["SPY_LAST_PRICE"].getWidth()) / 2) - 160, 400);
    textures["QQQ_LAST_PRICE"].render( pRenderer, (SCREEN_WIDTH - textures["QQQ_LAST_PRICE"].getWidth()) / 2, 400 );
    textures["DIA_LAST_PRICE"].render( pRenderer, ((SCREEN_WIDTH - textures["DIA_LAST_PRICE"].getWidth()) / 2) + 160, 400 );
    textures["TLT_LAST_PRICE"].render( pRenderer, ((SCREEN_WIDTH - textures["TLT_LAST_PRICE"].getWidth()) / 2) - 160, 425 );
    textures["IWM_LAST_PRICE"].render( pRenderer, (SCREEN_WIDTH - textures["IWM_LAST_PRICE"].getWidth()) / 2, 425 );
    textures["VXX_LAST_PRICE"].render( pRenderer, ((SCREEN_WIDTH - textures["VXX_LAST_PRICE"].getWidth()) / 2) + 156, 425 );

    SDL_RenderPresent(pRenderer);
}
