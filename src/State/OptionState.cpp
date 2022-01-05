//  OptionState Class
#include "OptionState.hpp"
#include "StartState.hpp"
#include "QuoteState.hpp"
#include "Layout/Menu.hpp"

OptionState OptionState::m_OptionState;
static bool select_options[] = {false};
static int last_select = 0;

void OptionState::init(Manager *premia)
{
    this->premia = premia;
    tda_data_interface = boost::make_shared<tda::TDAmeritrade>(tda::OPTION_CHAIN);
    tda_data_interface->set_option_chain_parameters( "TLT", "ALL", "50", true, "SINGLE", "ALL", "ALL", "ALL" );
    optionChainData = tda_data_interface->createOptionChain( "TLT" );
    optionsDateTimeObj = optionChainData->getOptionsDateTimeObj();

    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui::StyleColorsClassic();
}

void OptionState::cleanup()
{
    TTF_CloseFont( tickerFont );

    SDL_Log("OptionState Cleanup\n");
}

void OptionState::pause()
{
    SDL_Log("OptionState Pause\n");
}

void OptionState::resume()
{
    SDL_Log("OptionState Resume\n");
}

void OptionState::handleEvents()
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
                        premia->change( QuoteState::instance() );
                        break;
                    case SDLK_RIGHT:
                        premia->change( StartState::instance() );
                        break;
                    default:
                        break;
                }
                break;

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

void OptionState::update()
{    
    std::string title_string = "Option Chain: " + optionChainData->getOptionChainDataVariable("symbol");
    draw_imgui_menu( premia, tda_data_interface, title_string );

    ImGui::Text( "%s (%s) [B: %s  A: %s]", optionChainData->getOptionChainDataVariable("symbol").c_str(), optionChainData->getUnderlyingDataVariable("markPercentChange").c_str(), optionChainData->getUnderlyingDataVariable("bid").c_str(), optionChainData->getUnderlyingDataVariable("ask").c_str() );
    ImGui::Spacing();

    ImGui::SetNextItemWidth( 200.f );

    static int current_item = 0;
    std::vector<const char*> datetime_array;
    std::vector<tda::OptionsDateTimeObj> temp_vec = optionsDateTimeObj;
    for ( int i = 0; i < temp_vec.size(); i++) 
    {
        datetime_array.push_back(temp_vec[i].datetime.data());
    }

    ImGui::Combo("Expiration Date", &current_item, datetime_array.data(), datetime_array.size());

    static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_SizingFixedFit;

    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

    // When using ScrollX or ScrollY we need to specify a size for our table container!
    // Otherwise by default the table will fit all available space, like a BeginChild() call.
    ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 15);
    if (ImGui::BeginTable("table_scrolly", 6, flags, outer_size))
    {
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Strike", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Bid", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Ask", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Last", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Net Chng", ImGuiTableColumnFlags_None);

        ImGui::TableHeadersRow();

        // Demonstrate using clipper for large vertical lists
        static bool select_option = false;

        ImGuiListClipper clipper;
        clipper.Begin(optionsDateTimeObj[current_item].strikePriceObj.size());
        while (clipper.Step())
        {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 6; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    switch( column )
                    {
                        case 0:
                            ImGui::Text("%s", optionsDateTimeObj[current_item].strikePriceObj[row].raw_option["putCall"].c_str() );
                            break;
                        case 1:
                            ImGui::Selectable(optionsDateTimeObj[current_item].strikePriceObj[row].raw_option["strikePrice"].c_str(), &select_options[row]);
                            //ImGui::Text("%s", optionsDateTimeObj[current_item].strikePriceObj[row].raw_option["strikePrice"].c_str() );
                            break;
                        case 2:
                            ImGui::Text("%s", optionsDateTimeObj[current_item].strikePriceObj[row].raw_option["bid"].c_str() );
                            break;
                        case 3:
                            ImGui::Text("%s", optionsDateTimeObj[current_item].strikePriceObj[row].raw_option["ask"].c_str() );
                            break;
                        case 4:
                            ImGui::Text("%s", optionsDateTimeObj[current_item].strikePriceObj[row].raw_option["last"].c_str() );
                            break;
                        case 5:
                            ImGui::Text("%s", optionsDateTimeObj[current_item].strikePriceObj[row].raw_option["netChange"].c_str() );
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

    for ( int i = 0; i < optionsDateTimeObj[current_item].strikePriceObj.size(); i++ )
    {
        if ( select_options[i] == true && last_select == 0 )
        {
            last_select = i;
            ImGui::Text("%s", optionsDateTimeObj[current_item].strikePriceObj[i].raw_option["description"].c_str() );
        }
        else if ( select_options[i] == true && last_select > 0 )
        {
            select_options[last_select] = false;
            last_select = i;
            ImGui::Text("%s", optionsDateTimeObj[current_item].strikePriceObj[i].raw_option["description"].c_str() );
        }
        else if ( select_options[i] == false && last_select == i )
        {
            last_select = 0;
        }
    }

    ImGui::Text("Last: %d", last_select );

    ImGui::End();

    SDL_RenderClear( premia->pRenderer );
}

void OptionState::draw()
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
