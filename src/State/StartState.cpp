//  StartState Class
#include "StartState.hpp"
#include "QuoteState.hpp"
#include "OptionState.hpp"
#include "DemoState.hpp"

StartState StartState::m_StartState;

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
    positionsFrame.import_manager(premia);
    positionsFrame.init_positions();
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
                    case SDLK_DOWN:
                        premia->change( DemoState::instance() );
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
    loginFrame.update();

    // Place Order Button ---------------------------------------------------------------------------------
    if ( ImGui::Button("Quick Order", ImVec2(120, 30)) )
        ImGui::OpenPopup("Quick Order");

    ImGui::SameLine();
    if ( ImGui::Button("Load Responses from WebSocket", ImVec2(220, 30)) )
    {
        for ( auto response : premia->tda_client.get_session_responses() )
        {
            SDL_Log("Response: %s", response.c_str() );
        }
    }

    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if ( ImGui::BeginPopupModal("Quick Order", NULL, ImGuiWindowFlags_AlwaysAutoResize) )
    {
        static std::string new_ticker = "";
        static bool active_instrument = false;
        static char buf[64] = "";
        ImGui::InputText("##symbol", buf, 64, ImGuiInputTextFlags_CharsUppercase );
        ImGui::SameLine(); 
        if ( ImGui::Button("Search") )
        {
            if ( strcmp(buf, "") != 0 )
            {
                new_ticker = std::string(buf);
                std::cout << "new ticker " << new_ticker << std::endl;
                quotes[ new_ticker ] = premia->tda_client.createQuote( new_ticker );
                active_instrument = true;
            }
            else
            {
                active_instrument = false;
            }
        }

        if ( active_instrument )
        {
            static int n = 0;
            ImGui::Combo("Order Type", &n, "Limit\0Market\0Stop\0Stop Limit\0\0");

            ImGui::Text("%s - %s", quotes[new_ticker].getQuoteVariable("symbol").c_str(), quotes[new_ticker].getQuoteVariable("description").c_str() );
            ImGui::Text("Bid: %s - Size: %s", quotes[new_ticker].getQuoteVariable("bidPrice").c_str(), quotes[new_ticker].getQuoteVariable("bidSize").c_str() );
            ImGui::Text("Ask: %s - Size: %s", quotes[new_ticker].getQuoteVariable("askPrice").c_str(), quotes[new_ticker].getQuoteVariable("askSize").c_str() );
            ImGui::Separator();

            static float order_quantity = 10.f;
            ImGui::InputFloat("Quanitity", &order_quantity, 10.f);

            double security_cost = 0.0;
            std::string bidPrice = quotes[new_ticker].getQuoteVariable("bidPrice");
            security_cost = std::stod( bidPrice );
            security_cost *= order_quantity;
            ImGui::Text("Cost: %lf", security_cost );
        }

        ImGui::Separator();
        ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

        if (ImGui::Button("Confirm", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }

    linePlot.update();
    positionsFrame.update();

    ImGui::End();    

    watchlistFrame.update();
    orderFrame.update();
    
    SDL_RenderClear(premia->pRenderer);
}

void StartState::draw()
{
    // fill window bounds
    int w = 1920, h = 1080;
    SDL_SetRenderDrawColor( premia->pRenderer, 55, 55, 55, 0 );
    SDL_GetWindowSize( premia->pWindow, &w, &h );
    SDL_Rect f = {0, 0, 1920, 1080};
    SDL_RenderFillRect( premia->pRenderer, &f );

    ImGui::Render();
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

    SDL_RenderPresent(premia->pRenderer);
}
