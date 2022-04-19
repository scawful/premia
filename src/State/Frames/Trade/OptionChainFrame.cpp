#include "OptionChainFrame.hpp"

static bool select_options[] = {false};
static int last_select = 0;

void OptionChainFrame::draw_search()
{
    static char ticker[128] = "";
    ImGui::InputText("Symbol", ticker, IM_ARRAYSIZE(ticker));
    if (ImGui::Button("Fetch")) {
        optionChainData = premia->tda_interface.getOptionChain( ticker, "ALL", "50", true, "SINGLE", "ALL", "ALL", "ALL" );
        optionsDateTimeObj = optionChainData.getOptionsDateTimeObj();
        std::vector<tda::OptionsDateTimeObj> temp_vec = optionsDateTimeObj;
        for ( int i = 0; i < temp_vec.size(); i++) 
        {
            datetime_array.push_back(temp_vec[i].datetime.data());
        }
        isActive = true;
    }
}

/**
 * @brief Draw the option chain table 
 * @author @scawful
 * 
 */
void OptionChainFrame::draw_option_chain()
{
    std::string title_string = "Option Chain: " + optionChainData.getOptionChainDataVariable("symbol");

    ImGui::Text( "%s (%s) [B: %s  A: %s]", optionChainData.getOptionChainDataVariable("symbol").c_str(), optionChainData.getUnderlyingDataVariable("markPercentChange").c_str(), optionChainData.getUnderlyingDataVariable("bid").c_str(), optionChainData.getUnderlyingDataVariable("ask").c_str() );
    ImGui::Spacing();
    ImGui::SetNextItemWidth(200.f);

    static int current_item = 0;
    ImGui::Combo("Expiration Date", &current_item, datetime_array.data(), (int) datetime_array.size());

    static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_SizingFixedFit;

    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

    // When using ScrollX or ScrollY we need to specify a size for our table container!
    // Otherwise by default the table will fit all available space, like a BeginChild() call.
    auto outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * (float) optionsDateTimeObj[current_item].strikePriceObj.size());
    if (ImGui::BeginTable("table_scrolly", 16, flags, outer_size)) {
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Strike", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Bid", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Ask", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Last", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Chng", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Vol", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Delta", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Gamma", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Theta", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Vega", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Rho", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Time Val", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Open Int", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Theo Value", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Theo Vol", ImGuiTableColumnFlags_None);

        ImGui::TableHeadersRow();

        // Demonstrate using clipper for large vertical lists
        static bool select_option = false;

        ImGuiListClipper clipper;
        clipper.Begin((int) optionsDateTimeObj[current_item].strikePriceObj.size());
        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                ImGui::TableNextRow();
                for (int column = 0; column < 16; column++) {
                    ImGui::TableSetColumnIndex(column);
                    switch( column ) {
                        case 0:
                            ImGui::Text("%s", optionsDateTimeObj[current_item].strikePriceObj[row].raw_option["putCall"].c_str() );
                            break;
                        case 1:
                            ImGui::Selectable(optionsDateTimeObj[current_item].strikePriceObj[row].raw_option["strikePrice"].c_str(), &select_options[row]);
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
                        case 6:
                            ImGui::Text("%s", optionsDateTimeObj[current_item].strikePriceObj[row].raw_option["volatility"].c_str() );
                            break;
                        case 7:
                            ImGui::Text("%s", optionsDateTimeObj[current_item].strikePriceObj[row].raw_option["delta"].c_str() );
                            break;
                        case 8:
                            ImGui::Text("%s", optionsDateTimeObj[current_item].strikePriceObj[row].raw_option["gamma"].c_str() );
                            break;
                        case 9:
                            ImGui::Text("%s", optionsDateTimeObj[current_item].strikePriceObj[row].raw_option["theta"].c_str() );
                            break;
                        case 10:
                            ImGui::Text("%s", optionsDateTimeObj[current_item].strikePriceObj[row].raw_option["vega"].c_str() );
                            break; 
                        case 11:
                            ImGui::Text("%s", optionsDateTimeObj[current_item].strikePriceObj[row].raw_option["rho"].c_str() );
                            break;   
                        case 12:
                            ImGui::Text("%s", optionsDateTimeObj[current_item].strikePriceObj[row].raw_option["timeValue"].c_str() );
                            break;    
                        case 13:
                            ImGui::Text("%s", optionsDateTimeObj[current_item].strikePriceObj[row].raw_option["openInterest"].c_str() );
                            break;       
                        case 14:
                            ImGui::Text("%s", optionsDateTimeObj[current_item].strikePriceObj[row].raw_option["theoreticalOptionValue"].c_str() );
                            break;    
                        case 15:
                            ImGui::Text("%s", optionsDateTimeObj[current_item].strikePriceObj[row].raw_option["theoreticalVolatility"].c_str() );
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
        if ( select_options[i] == true && last_select == 0 ) {
            last_select = i;
            ImGui::Text("%s", optionsDateTimeObj[current_item].strikePriceObj[i].raw_option["description"].c_str() );
        } else if ( select_options[i] == true && last_select > 0 ) {
            select_options[last_select] = false;
            last_select = i;
            ImGui::Text("%s", optionsDateTimeObj[current_item].strikePriceObj[i].raw_option["description"].c_str() );
        } else if ( select_options[i] == false && last_select == i ) {
            last_select = 0;
        }
    }

    ImGui::Text("Last: %d", last_select );
}

OptionChainFrame::OptionChainFrame() : Frame()
{
    isActive = false;
}

void OptionChainFrame::load_data()
{
    static float progress = 0.0f;
    static float progress_dir = 1.0f;
    ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::Text("Progress Bar");
}

void OptionChainFrame::update()
{
    if (isActive) {
        draw_option_chain();
    } else {
        draw_search();
    }
}