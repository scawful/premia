//  DemoState Class
#include "DemoState.hpp"
#include "StartState.hpp"

DemoState DemoState::m_DemoState;

void DemoState::init(Manager *premia)
{
    this->premia = premia;
    ImGui::CreateContext();
    ImGui::StyleColorsClassic();
}

void DemoState::cleanup()
{
    SDL_Log("DemoState Cleanup\n");
}

void DemoState::pause()
{
    SDL_Log("DemoState Pause\n");
}

void DemoState::resume()
{
    SDL_Log("DemoState Resume\n");
}

void DemoState::handleEvents()
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
                        //premia->quit();
                        break;
                    case SDLK_UP:
                        premia->change( StartState::instance() );
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
                break;
            } 
            
            case SDL_WINDOWEVENT:
                switch ( event.window.event ) 
                {
                    case SDL_WINDOWEVENT_CLOSE:   // exit game
                        //premia->quit();
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

void DemoState::update()
{
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(200, 200));
    
    // hide demo windows unless necessary
    ImGui::ShowDemoWindow();
    ImPlot::ShowDemoWindow();
    
    SDL_RenderClear(premia->pRenderer);
}

void DemoState::draw()
{
    // fill window bounds
    int w = 1920, h = 1080;
    SDL_SetRenderDrawColor( premia->pRenderer, 0, 0, 0, 0 );
    SDL_GetWindowSize( premia->pWindow, &w, &h );
    SDL_Rect f = { 0, 0, 1920, 1080 };
    SDL_RenderFillRect( premia->pRenderer, &f );

    ImGui::Render();
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(premia->pRenderer);
}
