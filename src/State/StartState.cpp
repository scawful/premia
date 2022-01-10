//  StartState Class
#include "StartState.hpp"
#include "DemoState.hpp"

StartState StartState::m_StartState;

/**
 * @brief 
 * 
 */
void StartState::tda_login()
{
    optionChain.init_chain("TLT");
    candleChart.init_instrument("TLT");
    portfolioFrame.init_positions();
    watchlistFrame.init_watchlists();
}

/**
 * @brief Initialize the StartStates variables 
 * @author @scawful
 * 
 * @param premia 
 */
void StartState::init(Manager *premia)
{
    this->premia = premia;
    this->title_string = "Home";

    mainMenu.import_manager(premia);
    portfolioFrame.import_manager(premia, &protected_mode, &tda_logged_in);
    loginFrame.import_manager(premia, &protected_mode, &tda_logged_in);
    candleChart.import_manager(premia, &protected_mode, &tda_logged_in);
    optionChain.import_manager(premia, &protected_mode, &tda_logged_in);
    watchlistFrame.import_manager(premia, &protected_mode, &tda_logged_in);
    tda_login();
    
    ImGui::StyleColorsClassic();
}

/**
 * @brief Cleanup any allocated resources
 * @author @scawful
 * 
 */
void StartState::cleanup()
{
    SDL_Log("StartState Cleanup\n");
}

/**
 * @brief Pause the runtime loop of the state
 * @author @scawful
 * 
 */
void StartState::pause()
{
    SDL_Log("StartState Pause\n");
}

/**
 * @brief Resume the runtime loop of the state
 * @author @scawful
 * 
 */
void StartState::resume()
{
    SDL_Log("StartState Resume\n");
}

/**
 * @brief Handle input/output events via keyboard and mouse 
 * @author @scawful
 * 
 */
void StartState::handleEvents()
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
                    case SDLK_UP:
                    case SDLK_DOWN:
                    case SDLK_RETURN:
                    case SDLK_BACKSPACE:
                        io.KeysDown[event.key.keysym.scancode] = (event.type == SDL_KEYDOWN);
                        break;
                    default:
                        break;
                }
                break;

            case SDL_KEYUP:
            {
                int key = event.key.keysym.scancode;
                IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
                io.KeysDown[key] = (event.type == SDL_KEYDOWN);
                io.KeyMap[key] = io.KeysDown[key];
                io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
                io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
                io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
                io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
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

/**
 * @brief Update the contents of the StartState
 *        Construct GUI elements
 * @author @scawful
 * 
 */
void StartState::update()
{
    mainMenu.set_title(title_string);
    mainMenu.update();
    
    switch (mainMenu.get_current_frame()) {
        case MenuFrame::SubFrame::LOGIN:
            loginFrame.update();
            break;
        case MenuFrame::SubFrame::LINE_PLOT:
            linePlot.update();
            break;
        case MenuFrame::SubFrame::CANDLE_CHART:
            candleChart.update();
            break;
        case MenuFrame::SubFrame::OPTION_CHAIN:
            optionChain.update();
            break;
        case MenuFrame::SubFrame::TRADING:
            tradingFrame.update();
            break;
        default:
            break;
    }

    if ( ImGui::Button("KuCoin Test") )
    {
        premia->kucoin_interface.list_accounts();
    }

    ImGui::End();    

    watchlistFrame.update();
    console.update();
    portfolioFrame.update();
    
    SDL_RenderClear(premia->pRenderer);
}

/**
 * @brief Render to the screen
 * @author @scawful 
 * 
 */
void StartState::draw()
{
    // fill window bounds
    int w = 1920, h = 1080;
    SDL_SetRenderDrawColor(premia->pRenderer, 55, 55, 55, 0);
    SDL_GetWindowSize(premia->pWindow, &w, &h);
    SDL_Rect f = {0, 0, 1920, 1080};
    SDL_RenderFillRect(premia->pRenderer, &f);
    ImGui::Render();
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(premia->pRenderer);
}
