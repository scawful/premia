//  LoginState Class
#include "LoginState.hpp"
#include "DemoState.hpp"

LoginState LoginState::m_LoginState;

/**
 * @brief Initialize the LoginStates variables 
 * @author @scawful
 * 
 * @param premia 
 */
void LoginState::init(Manager *premia)
{
    this->premia = premia;
    loginFrame.import_manager(premia);
    ImGui::StyleColorsClassic();
}

/**
 * @brief Cleanup any allocated resources
 * @author @scawful
 * 
 */
void LoginState::cleanup()
{
    SDL_Log("LoginState Cleanup\n");
}

/**
 * @brief Pause the runtime loop of the state
 * @author @scawful
 * 
 */
void LoginState::pause()
{
    SDL_Log("LoginState Pause\n");
}

/**
 * @brief Resume the runtime loop of the state
 * @author @scawful
 * 
 */
void LoginState::resume()
{
    SDL_Log("LoginState Resume\n");
}

/**
 * @brief Handle input/output events via keyboard and mouse 
 * @author @scawful
 * 
 */
void LoginState::handleEvents()
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
                //io.KeysDown[key] = (event.type == SDL_KEYDOWN);
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
 * @brief Update the contents of the LoginState
 *        Construct GUI elements
 * @author @scawful
 * 
 */
void LoginState::update()
{
    ImGui::NewFrame();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(300,400), ImGuiCond_Always);

    if (!ImGui::Begin("Premia Login", NULL, ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        return;
    }

    loginFrame.update();
    if (loginFrame.checkLoginStatus()) {
        premia->change(PrimaryState::instance());
    }

    ImGui::End();
    SDL_RenderClear(premia->pRenderer);
}

/**
 * @brief Render to the screen
 * @author @scawful 
 * 
 */
void LoginState::draw()
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
