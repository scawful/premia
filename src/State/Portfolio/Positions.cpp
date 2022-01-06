#include "Positions.hpp"
#include "../QuoteState.hpp"
#include "../OptionState.hpp"
#include "../DemoState.hpp"

Positions Positions::m_Positions;

void Positions::load_account( std::string account_num )
{
    account_data = premia->tda_client.createAccount( account_num );

    if ( positions_vector.size() != 0 ) {
        positions_vector.clear();
    }

    for ( int i = 0; i < account_data.get_position_vector_size(); i++ )
    {
        for ( auto& position_it : account_data.get_position( i ) )
        {
            if ( position_it.first == "symbol" )
            {
                std::string str = position_it.second;
                positions_vector.push_back( str );
            }
        }
    }
}

void Positions::init(Manager *premia)
{
    this->premia = premia;
    mainMenu.import_manager(premia);
    account_ids_std = premia->tda_client.get_all_accounts();
    for ( std::string const& each_id : account_ids_std ) {
        account_ids.push_back(each_id.c_str());
    }
    default_account = account_ids_std.at(0);
    load_account(default_account);

    ImGui::CreateContext();
    ImGui::StyleColorsClassic();
}

void Positions::cleanup()
{
    SDL_Log("Positions Cleanup\n");
}

void Positions::pause()
{
    SDL_Log("Positions Pause\n");
}

void Positions::resume()
{
    SDL_Log("Positions Resume\n");
}

void Positions::handleEvents()
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

void Positions::update()
{
    mainMenu.update();

    // Load Account IDs
    static int n = 0;
    const char **accounts = account_ids.data();
    if ( ImGui::Button( "Change Account" ) ) {
        load_account( accounts[n] );
    } 
    ImGui::SameLine();
    ImGui::Combo("", &n,  accounts, 6); 
    
    // ImGui::Combo("Order Type", &n, "Limit\0Market\0Stop\0Stop Limit\0\0");
    ImGui::Text("TDAmeritrade Portfolio Information");
    ImGui::Separator();
    ImGui::Text( "Account ID: %s", account_data.get_account_variable("accountId").c_str() );
    ImGui::Text( "Net Liq: %s", account_data.get_balance_variable("liquidationValue").c_str() );
    ImGui::Text( "Available Funds: %s", account_data.get_balance_variable("availableFunds").c_str() );
    ImGui::Text( "Cash: %s", account_data.get_balance_variable("cashBalance").c_str() );

    ImGui::Separator();
    ImGui::Text("Positions");
    ImGui::Separator();
    ImGui::Spacing();

    static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
    ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 15);

    if (ImGui::BeginTable("table_scrolly", 7, flags, outer_size))
    {
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
        ImGui::TableSetupColumn("Symbol", ImGuiTableColumnFlags_WidthStretch );
        ImGui::TableSetupColumn("P/L Day", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableSetupColumn("P/L %", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableSetupColumn("Average Price",  ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableSetupColumn("Market Value",  ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableSetupColumn("Quantity", ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_NoSort );
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin( positions_vector.size() );
        while (clipper.Step())
        {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 7; column++)
                {
                    std::string symbol = positions_vector[row];
                    ImGui::TableSetColumnIndex(column);
                    switch( column )
                    {
                        case 0:
                            ImGui::Text("%s", symbol.c_str() );
                            break;
                        case 1:
                            ImGui::Text("%s", account_data.get_position_balances( symbol, "currentDayProfitLoss" ).c_str());
                            break;
                        case 2:
                            ImGui::Text("%s", account_data.get_position_balances( symbol, "currentDayProfitLossPercentage").c_str() );
                            break;
                        case 3:
                            ImGui::Text("%s", account_data.get_position_balances( symbol, "averagePrice" ).c_str());
                            break;
                        case 4:
                            ImGui::Text("%s", account_data.get_position_balances( symbol, "marketValue" ).c_str());
                            break;
                        case 5:
                            ImGui::Text("%s", account_data.get_position_balances( symbol, "longQuantity" ).c_str());
                            break;
                        case 6:
                            ImGui::SmallButton("Buy");
                            ImGui::SameLine();
                            ImGui::SmallButton("Sell");
                            ImGui::SameLine();
                            ImGui::SmallButton("Q");
                            break;
                        default:
                            ImGui::Text("Hello %d,%d", column, row);
                            break;
                    }
                }
            }
        }

        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::Separator();

    SDL_RenderClear(premia->pRenderer);
}

void Positions::draw()
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
