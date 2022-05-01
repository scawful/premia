#include "OptionChain.hpp"

static bool select_options[] = {false};
static int last_select = 0;

void OptionChainView::drawSearch()
{
    static char ticker[128] = "";
    ImGui::InputText("Symbol", ticker, IM_ARRAYSIZE(ticker));
    if (ImGui::Button("Fetch")) {
        model.fetchOptionChain( ticker, "ALL", "50", true, "SINGLE", "ALL", "ALL", "ALL" );
        model.calculateGammaExposure();
    }
}

void OptionChainView::drawChain()
{
    static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_SizingStretchProp;

    if (ImGui::BeginTable("table_scrolly", 19, flags, ImGui::GetContentRegionAvail())) {
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible

        ImGui::TableSetupColumn("Bid", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Ask", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Last", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Chng", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Delta", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Gamma", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Theta", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Vega", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Open Int", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Strike", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Bid", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Ask", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Last", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Chng", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Delta", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Gamma", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Theta", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Vega", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Open Int", ImGuiTableColumnFlags_None);
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin((int) model.getOptionsDateTimeObj(0).strikePriceObj.size());
        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                ImGui::TableNextRow();
                for (int column = 0; column < 19; column++) {
                    ImGui::TableSetColumnIndex(column);
                    switch(column) {
                        case 0:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(0).strikePriceObj[row].raw_option["bid"].c_str() );
                            break;
                        case 1:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(0).strikePriceObj[row].raw_option["ask"].c_str() );
                            break;
                        case 2:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(0).strikePriceObj[row].raw_option["last"].c_str() );
                            break;
                        case 3:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(0).strikePriceObj[row].raw_option["netChange"].c_str() );
                            break; 
                        case 4:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(0).strikePriceObj[row].raw_option["delta"].c_str() );
                            break;
                        case 5:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(0).strikePriceObj[row].raw_option["gamma"].c_str() );
                            break;
                        case 6:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(0).strikePriceObj[row].raw_option["theta"].c_str() );
                            break;
                        case 7:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(0).strikePriceObj[row].raw_option["vega"].c_str() );
                            break;  
                        case 8:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(0).strikePriceObj[row].raw_option["openInterest"].c_str() );
                            break;                              
                        case 9:
                            ImGui::Selectable(model.getOptionsDateTimeObj(0).strikePriceObj[row].raw_option["strikePrice"].c_str(), &select_options[row]);
                            break;
                        case 10:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(1).strikePriceObj[row].raw_option["bid"].c_str() );
                            break;
                        case 11:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(1).strikePriceObj[row].raw_option["ask"].c_str() );
                            break;
                        case 12:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(1).strikePriceObj[row].raw_option["last"].c_str() );
                            break;
                        case 13:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(1).strikePriceObj[row].raw_option["netChange"].c_str() );
                            break;  
                        case 14:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(1).strikePriceObj[row].raw_option["delta"].c_str() );
                            break;
                        case 15:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(1).strikePriceObj[row].raw_option["gamma"].c_str() );
                            break;
                        case 16:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(1).strikePriceObj[row].raw_option["theta"].c_str() );
                            break;
                        case 17:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(1).strikePriceObj[row].raw_option["vega"].c_str() );
                            break;  
                        case 18:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(1).strikePriceObj[row].raw_option["openInterest"].c_str() );
                            break;                                                        
                        default:
                            ImGui::Text("N/A %d,%d", column, row);
                            break;
                    }
                }
            }
        }
        ImGui::EndTable();
    }
}

void OptionChainView::drawGreeks()
{
    ImGui::Text( "%s (%s) [B: %s  A: %s]", model.getOptionChainData().getOptionChainDataVariable("symbol").c_str(), model.getOptionChainData().getUnderlyingDataVariable("markPercentChange").c_str(), model.getOptionChainData().getUnderlyingDataVariable("bid").c_str(), model.getOptionChainData().getUnderlyingDataVariable("ask").c_str() );
    ImGui::Spacing();
    ImGui::SetNextItemWidth(200.f);

    static int current_item = 0;
    if (ImGui::BeginCombo("Expiration Date", model.getDateTime(0).c_str(), ImGuiComboFlags_None))
    {
        for (int n = 0; n < model.getDateTimeArray().size(); n++)
        {
            const bool is_selected = (current_item == n);
            if (ImGui::Selectable(model.getDateTime(n).c_str(), is_selected))
                current_item = n;

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::Separator();
    ImGui::Text("Implied Volatility: %s", model.getOptionChainData().getOptionChainDataVariable("volatility").c_str());
    ImGui::Text("Naive Gamma Exposure: %.2f", model.getGammaExposure());
    ImGui::Text("Skew-Adjusted Gamma Exposure: ");
    ImGui::Text("GEX Flip Point: ");
    ImGui::Text("Distance to Flip: ");
    ImGui::Text("Call Skew: ");
    ImGui::Text("$ GEX @ Next Expiry: ");
    ImGui::Separator();
    ImGui::Text("Lambda Elasticity: ");
}

void OptionChainView::drawOptionChain()
{
    std::string title_string = "Option Chain: " + model.getOptionChainData().getOptionChainDataVariable("symbol");

    ImGui::Text( "%s (%s) [B: %s  A: %s]", model.getOptionChainData().getOptionChainDataVariable("symbol").c_str(), model.getOptionChainData().getUnderlyingDataVariable("markPercentChange").c_str(), model.getOptionChainData().getUnderlyingDataVariable("bid").c_str(), model.getOptionChainData().getUnderlyingDataVariable("ask").c_str() );
    ImGui::Spacing();
    ImGui::SetNextItemWidth(200.f);

    static int current_item = 0;
    ImGui::Combo("Expiration Date", &current_item, model.getDateTimeArray().data(), (int) model.getDateTimeArray().size());

    static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_SizingFixedFit;

    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

    // When using ScrollX or ScrollY we need to specify a size for our table container!
    // Otherwise by default the table will fit all available space, like a BeginChild() call.
    auto outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * (float) model.getOptionsDateTimeObj(current_item).strikePriceObj.size());
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
        clipper.Begin((int) model.getOptionsDateTimeObj(current_item).strikePriceObj.size());
        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                ImGui::TableNextRow();
                for (int column = 0; column < 16; column++) {
                    ImGui::TableSetColumnIndex(column);
                    switch( column ) {
                        case 0:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(current_item).strikePriceObj[row].raw_option["putCall"].c_str() );
                            break;
                        case 1:
                            ImGui::Selectable(model.getOptionsDateTimeObj(current_item).strikePriceObj[row].raw_option["strikePrice"].c_str(), &select_options[row]);
                            break;
                        case 2:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(current_item).strikePriceObj[row].raw_option["bid"].c_str() );
                            break;
                        case 3:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(current_item).strikePriceObj[row].raw_option["ask"].c_str() );
                            break;
                        case 4:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(current_item).strikePriceObj[row].raw_option["last"].c_str() );
                            break;
                        case 5:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(current_item).strikePriceObj[row].raw_option["netChange"].c_str() );
                            break;
                        case 6:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(current_item).strikePriceObj[row].raw_option["volatility"].c_str() );
                            break;
                        case 7:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(current_item).strikePriceObj[row].raw_option["delta"].c_str() );
                            break;
                        case 8:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(current_item).strikePriceObj[row].raw_option["gamma"].c_str() );
                            break;
                        case 9:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(current_item).strikePriceObj[row].raw_option["theta"].c_str() );
                            break;
                        case 10:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(current_item).strikePriceObj[row].raw_option["vega"].c_str() );
                            break; 
                        case 11:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(current_item).strikePriceObj[row].raw_option["rho"].c_str() );
                            break;   
                        case 12:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(current_item).strikePriceObj[row].raw_option["timeValue"].c_str() );
                            break;    
                        case 13:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(current_item).strikePriceObj[row].raw_option["openInterest"].c_str() );
                            break;       
                        case 14:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(current_item).strikePriceObj[row].raw_option["theoreticalOptionValue"].c_str() );
                            break;    
                        case 15:
                            ImGui::Text("%s", model.getOptionsDateTimeObj(current_item).strikePriceObj[row].raw_option["theoreticalVolatility"].c_str() );
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

    for ( int i = 0; i < model.getOptionsDateTimeObj(current_item).strikePriceObj.size(); i++ )
    {
        if ( select_options[i] == true && last_select == 0 ) {
            last_select = i;
            ImGui::Text("%s", model.getOptionsDateTimeObj(current_item).strikePriceObj[i].raw_option["description"].c_str() );
        } else if ( select_options[i] == true && last_select > 0 ) {
            select_options[last_select] = false;
            last_select = i;
            ImGui::Text("%s", model.getOptionsDateTimeObj(current_item).strikePriceObj[i].raw_option["description"].c_str() );
        } else if ( select_options[i] == false && last_select == i ) {
            last_select = 0;
        }
    }

    ImGui::Text("Last: %d", last_select );
}

void OptionChainView::addAuth(const std::string & key , const std::string & token)
{
    
}

void OptionChainView::addLogger(const ConsoleLogger & newLogger)
{
    this->logger = newLogger;
}

void OptionChainView::addEvent(const std::string & key, const VoidEventHandler & event)
{
    this->events[key] = event;
}

void OptionChainView::update() 
{
    if (model.isActive()) {
        drawGreeks();
        drawChain();
        //drawOptionChain();
    } else {
        drawSearch();
    }
}
