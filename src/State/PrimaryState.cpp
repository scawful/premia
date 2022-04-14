//  PrimaryState Class
#include "PrimaryState.hpp"
#include "DemoState.hpp"

PrimaryState PrimaryState::m_PrimaryState;

/**
 * @brief Initialize the PrimaryStates variables 
 * @author @scawful
 * 
 * @param premia 
 */
void PrimaryState::init(Manager * premia)
{
    this->premia = premia;
    this->title_string = "Home";

    mainMenu.import_manager(premia, &protected_mode, &tda_logged_in);
    premiaHome.import_manager(premia, &protected_mode, &tda_logged_in);
    console.import_manager(premia, &protected_mode, &tda_logged_in);
    portfolioFrame.import_manager(premia, &protected_mode, &tda_logged_in);
    loginFrame.import_manager(premia, &protected_mode, &tda_logged_in);
    candleChart.import_manager(premia, &protected_mode, &tda_logged_in);
    optionChain.import_manager(premia, &protected_mode, &tda_logged_in);
    watchlistFrame.import_manager(premia, &protected_mode, &tda_logged_in);
    fundOwnership.import_manager(premia);
    marketOverview.import_manager(premia);
    
    SDL_SetWindowSize(premia->pWindow, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetWindowPosition(premia->pWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Backspace] = SDL_GetScancodeFromKey( SDLK_BACKSPACE );
    
    ImGui::StyleColorsClassic();
}

/**
 * @brief Cleanup any allocated resources
 * @author @scawful
 * 
 */
void PrimaryState::cleanup()
{
    SDL_Log("PrimaryState Cleanup\n");
}

/**
 * @brief Pause the runtime loop of the state
 * @author @scawful
 * 
 */
void PrimaryState::pause()
{
    SDL_Log("PrimaryState Pause\n");
}

/**
 * @brief Resume the runtime loop of the state
 * @author @scawful
 * 
 */
void PrimaryState::resume()
{
    SDL_Log("PrimaryState Resume\n");
}

/**
 * @brief Handle input/output events via keyboard and mouse 
 * @author @scawful
 * 
 */
void PrimaryState::handleEvents()
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

    int mouseX;
    int mouseY;
    const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

    io.DeltaTime = 1.0f / 60.0f;
    io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
    io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
    io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
    io.MouseWheel = static_cast<float>(wheel);

}

/**
 * @brief Update the contents of the PrimaryState
 *        Construct GUI elements
 * @author @scawful
 * 
 */
void PrimaryState::update()
{
    ImGui::NewFrame();
    ImGui::SetNextWindowPos( ImVec2(0, 0) );
    const ImGuiIO & io = ImGui::GetIO();

    ImVec2 dimensions(io.DisplaySize.x, io.DisplaySize.y);
    if (mainMenu.watchlistView()) {
        dimensions.x = io.DisplaySize.x * (float) 0.75;
    }
    if (mainMenu.consoleView() || mainMenu.portfolioView() ) {
        dimensions.y = io.DisplaySize.y * (float) 0.70;
    }
    ImGui::SetNextWindowSize(dimensions, ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;
    if (!mainMenu.freeMode()) {
        flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
    }
    
    if (!ImGui::Begin(  title_string.c_str(), nullptr, flags)) {
        ImGui::End();
        return;
    }
    
    mainMenu.set_title(title_string);
    mainMenu.update();
    
    switch (mainMenu.get_current_frame()) 
    {
        case MenuFrame::PREMIA_HOME:
            premiaHome.update();
            break;
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
        case MenuFrame::SubFrame::MARKET_OVERVIEW:
            marketOverview.update();
            break;
        case MenuFrame::SubFrame::RISK_APPETITE:
            riskAppetite.update();
            break;
        case MenuFrame::SubFrame::FUND_OWNERSHIP:
            fundOwnership.update();
            break;
        default:
            break;
    }

    ImGui::End();    

    if (mainMenu.watchlistView()) 
    {
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.75, 0));
        auto size = ImVec2(io.DisplaySize.x * 0.25, io.DisplaySize.y * 0.70);
        if (!mainMenu.consoleView() && !mainMenu.portfolioView()) {
            size = ImVec2(io.DisplaySize.x * 0.25, io.DisplaySize.y);
        }
        ImGui::SetNextWindowSize(size, ImGuiCond_Always);
        watchlistFrame.update();
    }
    
    if (mainMenu.consoleView()) 
    {
        ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y * 0.70));
        auto size = ImVec2(io.DisplaySize.x * 0.5, io.DisplaySize.y * 0.30);
        if (!mainMenu.portfolioView()) {
            size = ImVec2(io.DisplaySize.x, io.DisplaySize.y * 0.30);
        }
        ImGui::SetNextWindowSize(size, ImGuiCond_Always);
        console.update();
    }

    if (mainMenu.portfolioView()) {
        auto size = ImVec2(io.DisplaySize.x * 0.5, io.DisplaySize.y * 0.30);
        auto pos = ImVec2(io.DisplaySize.x * 0.5, io.DisplaySize.y * 0.70);
        if (!mainMenu.consoleView()) {
            size = ImVec2(io.DisplaySize.x, io.DisplaySize.y * 0.30);
            pos = ImVec2(0, io.DisplaySize.y * 0.70);
        }       
        ImGui::SetNextWindowPos(pos);
        ImGui::SetNextWindowSize(size, ImGuiCond_Always);
        portfolioFrame.update();
    }
    
    
    SDL_RenderClear(premia->pRenderer);
}

/**
 * @brief Render to the screen
 * @author @scawful 
 * 
 */
void PrimaryState::draw()
{
    // fill window bounds
    int w = 1920;
    int h = 1080;
    SDL_SetRenderDrawColor(premia->pRenderer, 55, 55, 55, 0);
    SDL_GetWindowSize(premia->pWindow, &w, &h);
    SDL_Rect f = {0, 0, 1920, 1080};
    SDL_RenderFillRect(premia->pRenderer, &f);
    ImGui::Render();
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(premia->pRenderer);
}
