//  StartState Class
#include "StartState.hpp"
#include "QuoteState.hpp"
#include "OptionState.hpp"
#include "DemoState.hpp"
#include "Portfolio/Positions.hpp"
#include "Layout/Menu.hpp"

StartState StartState::m_StartState;

void StartState::init( SDL_Renderer *pRenderer, SDL_Window *pWindow )
{
    this->pRenderer = pRenderer;
    this->pWindow = pWindow;
    tda_data_interface = boost::make_shared<tda::TDAmeritrade>(tda::GET_QUOTE);
    // 497912311 236520988
    account_data = tda_data_interface->createAccount( "236520988" );

    // cbp_data_interface = boost::make_shared<cbp::CoinbasePro>();
    // cbp_account_data = cbp_data_interface->list_accounts();

    // cbp_products["ETH"] = cbp_data_interface->get_product_ticker( "ETH" );
    // cbp_products["XTZ"] = cbp_data_interface->get_product_ticker( "XTZ" );
    // cbp_products["USD"] = cbp_data_interface->get_product_ticker( "USD" );

    // float eth_balance;
    // for ( auto& crypto_position_it: cbp_account_data->get_position("ETH") )
    // {
    //     if ( crypto_position_it.first == "available")
    //         eth_balance = boost::lexical_cast<float>(crypto_position_it.second);
    // }

    // float xtz_balance;
    // for ( auto& crypto_position_it: cbp_account_data->get_position("XTZ") )
    // {
    //     if ( crypto_position_it.first == "available")
    //         xtz_balance = boost::lexical_cast<float>(crypto_position_it.second);
    // }

    // float usd_balance = boost::lexical_cast<float>(cbp_account_data->get_position("USD")["available"]);

    // //std::cout << "ETH: " << eth_balance << ", XTZ: " << xtz_balance << std::endl;

    // float deposit_usd = cbp_data_interface->get_deposits();
    // float temp = (eth_balance * cbp_products["ETH"]->get_current_price()) + (xtz_balance * cbp_products["XTZ"]->get_current_price()) - usd_balance;
    // //std::cout << "Calc " << temp << std::endl;
    // _profit_loss = (temp - deposit_usd) / deposit_usd;

    ImGui::CreateContext();
	ImGuiSDL::Initialize(pRenderer, 782, 543);
    ImGui::StyleColorsClassic();
}

void StartState::cleanup()
{
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

void StartState::update( Manager* premia )
{
    draw_imgui_menu( premia, tda_data_interface, "Home" );

    // Place Order Button ---------------------------------------------------------------------------------
    if ( ImGui::Button("Quick Order", ImVec2(120, 30)) )
        ImGui::OpenPopup("Quick Order");

    ImGui::SameLine();
    if ( ImGui::Button("Load Responses from WebSocket", ImVec2(220, 30)) )
    {
        for ( auto response : tda_data_interface->get_session_responses() )
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
                quotes[ new_ticker ] = tda_data_interface->createQuote( new_ticker );
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

            ImGui::Text("%s - %s", quotes[new_ticker]->getQuoteVariable("symbol").c_str(), quotes[new_ticker]->getQuoteVariable("description").c_str() );
            ImGui::Text("Bid: %s - Size: %s", quotes[new_ticker]->getQuoteVariable("bidPrice").c_str(), quotes[new_ticker]->getQuoteVariable("bidSize").c_str() );
            ImGui::Text("Ask: %s - Size: %s", quotes[new_ticker]->getQuoteVariable("askPrice").c_str(), quotes[new_ticker]->getQuoteVariable("askSize").c_str() );
            ImGui::Separator();

            static float order_quantity = 10.f;
            ImGui::InputFloat("Quanitity", &order_quantity, 10.f);

            double security_cost = 0.0;
            std::string bidPrice = quotes[new_ticker]->getQuoteVariable("bidPrice");
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

    
    // ImGui::Spacing();
    // ImGui::Separator();
    // ImGui::Text("Coinbase Pro Accounts || P/L: %.2f", _profit_loss);
    // ImGui::Separator();
    // for ( auto& crypto_position_it: cbp_account_data->get_position("ETH") )
    // {
    //     if ( crypto_position_it.first == "currency" || crypto_position_it.first == "available")
    //         ImGui::Text("%s - %s", crypto_position_it.first.c_str(), crypto_position_it.second.c_str() );
    // }
    // ImGui::Separator();
    // for ( auto& crypto_position_it: cbp_account_data->get_position("XTZ") )
    // {
    //     if ( crypto_position_it.first == "currency" || crypto_position_it.first == "available")
    //         ImGui::Text("%s - %s", crypto_position_it.first.c_str(), crypto_position_it.second.c_str() );
    // }

    ImGui::End();    
    SDL_RenderClear(this->pRenderer);
}

void StartState::draw( Manager* game )
{
    // fill window bounds
    int w = 1920, h = 1080;
    SDL_SetRenderDrawColor( this->pRenderer, 55, 55, 55, 0 );
    SDL_GetWindowSize( this->pWindow, &w, &h );
    SDL_Rect f = {0, 0, 1920, 1080};
    SDL_RenderFillRect( this->pRenderer, &f );

    ImGui::Render();
    ImGuiSDL::Render(ImGui::GetDrawData());

    SDL_RenderPresent(pRenderer);
}
