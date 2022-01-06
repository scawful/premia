//  StreamState Class
#include "StreamState.hpp"
#include "StartState.hpp"
#include "OptionState.hpp"

StreamState StreamState::m_StreamState;

void StreamState::set_instrument( std::string ticker )
{
    ticker_symbol = ticker;
}

void StreamState::init(Manager *premia)
{
    this->premia = premia;
    title_string = "Live Quotes";
    mainMenu.import_manager(premia);
    mainMenu.set_title(title_string);

    for ( auto v: request_fields )
    {
        v = false;
    }

    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui::StyleColorsClassic();
}

void StreamState::cleanup()
{
    SDL_Log("StreamState Cleanup\n");
}

void StreamState::pause()
{
    SDL_Log("StreamState Pause\n");
}

void StreamState::resume()
{
    SDL_Log("StreamState Resume\n");
}

void StreamState::handleEvents()
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

void StreamState::update()
{    
    mainMenu.update();

    ImGui::Text( "LEVEL ONE QUOTE (7:30am – 8pm EST)" );
    ImGui::SameLine();

    static char buf[64] = "";
    ImGui::Text("Symbol: ");
    ImGui::SameLine(); ImGui::InputText("##symbol", buf, 64, ImGuiInputTextFlags_CharsUppercase );
    ImGui::SameLine(); 
    if ( ImGui::Button("Search") )
    {
        if ( strcmp(buf, "") != 0 )
        {
            ticker_symbol = buf;
        }
    }

    ImGui::SameLine();
    if ( ImGui::Button("Customize Fields") )
    {
        ImGui::OpenPopup("Fields");
    }

    if ( ImGui::BeginPopupModal("Fields", NULL, ImGuiWindowFlags_AlwaysAutoResize ) )
    {
        ImGui::BeginChild("child", ImVec2(200, 120), true);
        for ( int i = 0; i < 52; i++ )
        {
            ImGui::Checkbox(quote_fields[i], &request_fields[i]);
        }
        ImGui::EndChild();

        if ( ImGui::Button("Close", ImVec2(120, 0)) ) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }

    if ( premia->tda_client.is_session_logged_in() )
    {
        if ( ImGui::Button("Logout") )
        {
            premia->tda_client.send_logout_request();
        }
    }
    else
    {
        if ( ImGui::Button("Login") )
        {
            std::string fields = "";
            for ( int i = 0; i < 53; i++ )
            {
                if ( request_fields[i] == true )
                    fields += std::to_string(i) + ",";
            }
            fields.replace(fields.end(), fields.end(), "");

            SDL_Log("Stream ticker %s with fields: %s", ticker_symbol.c_str(), fields.c_str() );

            premia->tda_client.start_session( ticker_symbol, fields );
        }
    }
    ImGui::SameLine();
    if ( ImGui::Button("Interrupt Session") )
    {
        premia->tda_client.send_interrupt_signal();
    }

    ImGui::BulletText("NASDAQ (Quotes and Trades)");
    ImGui::BulletText("OTCBB (Quotes and Trades)");
    ImGui::BulletText("Listed (NYSE, AMEX, Pacific Quotes and Trades)");
    ImGui::BulletText("Pinks (Quotes only)");
    ImGui::BulletText("Mutual Fund (No quotes)");
    ImGui::BulletText("Indices (Trades only)");
    ImGui::BulletText("Indicators");
    ImGui::Spacing();

    // ImPlot::GetStyle().UseLocalTime = true;
    // ImPlot::SetNextPlotFormatY("$%.2f");
    // ImPlot::SetNextAxesLimits((1609740000000 * 0.001), (1625267160000 * 0.001), 0, 100, ImGuiCond_Once);

    // if (ImPlot::BeginPlot("Candlestick Chart",NULL,NULL, ImVec2(-1,0),0,
    //                         ImPlotAxisFlags_Time,
    //                         ImPlotAxisFlags_AutoFit|ImPlotAxisFlags_RangeFit|ImPlotAxisFlags_LockMax)) 
    // {
    //     //createCandleChart( 0.25, 218, bullCol, bearCol, tooltip );
    //     ImPlot::EndPlot();
    // }


    ImGui::End();

    SDL_RenderClear( premia->pRenderer );
}

void StreamState::draw()
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
