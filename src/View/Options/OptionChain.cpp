#include "OptionChain.hpp"

static bool select_options[] = {false};
static int last_select = 0;

void OptionChainView::drawSearch()
{
    static std::string ticker;
    static std::string count;
    static std::string strike;
    ImGui::InputText("Symbol", &ticker, ImGuiInputTextFlags_CharsUppercase);
    ImGui::InputText("Strike Count", &count);
    ImGui::InputText("Strike Price", &strike);
    if (ImGui::Button("Fetch") && !count.empty()) {
        model.fetchOptionChain( ticker, "ALL", count, true, "SINGLE", "ALL", "ALL", "ALL" );
        model.calculateGammaExposure();
    }

}

void OptionChainView::drawChain()
{
    static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_SizingStretchProp;

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
        clipper.Begin((int) model.getCallOptionObj(current_item).strikePriceObj.size());
        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                ImGui::TableNextRow();
                for (int column = 0; column < 19; column++) {
                    ImGui::TableSetColumnIndex(column);
                    switch(column) {
                        case 0:
                            ImGui::Text("%s", model.getCallOptionObj(current_item).strikePriceObj[row].raw_option["bid"].c_str() );
                            break;
                        case 1:
                            ImGui::Text("%s", model.getCallOptionObj(current_item).strikePriceObj[row].raw_option["ask"].c_str() );
                            break;
                        case 2:
                            ImGui::Text("%s", model.getCallOptionObj(current_item).strikePriceObj[row].raw_option["last"].c_str() );
                            break;
                        case 3:
                            ImGui::Text("%s", model.getCallOptionObj(current_item).strikePriceObj[row].raw_option["netChange"].c_str() );
                            break; 
                        case 4:
                            ImGui::Text("%s", model.getCallOptionObj(current_item).strikePriceObj[row].raw_option["delta"].c_str() );
                            break;
                        case 5:
                            ImGui::Text("%s", model.getCallOptionObj(current_item).strikePriceObj[row].raw_option["gamma"].c_str() );
                            break;
                        case 6:
                            ImGui::Text("%s", model.getCallOptionObj(current_item).strikePriceObj[row].raw_option["theta"].c_str() );
                            break;
                        case 7:
                            ImGui::Text("%s", model.getCallOptionObj(current_item).strikePriceObj[row].raw_option["vega"].c_str() );
                            break;  
                        case 8:
                            ImGui::Text("%s", model.getCallOptionObj(current_item).strikePriceObj[row].raw_option["openInterest"].c_str() );
                            break;                              
                        case 9:
                            ImGui::Selectable(model.getCallOptionObj(current_item).strikePriceObj[row].raw_option["strikePrice"].c_str(), &select_options[row]);
                            break;
                        case 10:
                            ImGui::Text("%s", model.getPutOptionObj(current_item).strikePriceObj[row].raw_option["bid"].c_str() );
                            break;
                        case 11:
                            ImGui::Text("%s", model.getPutOptionObj(current_item).strikePriceObj[row].raw_option["ask"].c_str() );
                            break;
                        case 12:
                            ImGui::Text("%s", model.getPutOptionObj(current_item).strikePriceObj[row].raw_option["last"].c_str() );
                            break;
                        case 13:
                            ImGui::Text("%s", model.getPutOptionObj(current_item).strikePriceObj[row].raw_option["netChange"].c_str() );
                            break;  
                        case 14:
                            ImGui::Text("%s", model.getPutOptionObj(current_item).strikePriceObj[row].raw_option["delta"].c_str() );
                            break;
                        case 15:
                            ImGui::Text("%s", model.getPutOptionObj(current_item).strikePriceObj[row].raw_option["gamma"].c_str() );
                            break;
                        case 16:
                            ImGui::Text("%s", model.getPutOptionObj(current_item).strikePriceObj[row].raw_option["theta"].c_str() );
                            break;
                        case 17:
                            ImGui::Text("%s", model.getPutOptionObj(current_item).strikePriceObj[row].raw_option["vega"].c_str() );
                            break;  
                        case 18:
                            ImGui::Text("%s", model.getPutOptionObj(current_item).strikePriceObj[row].raw_option["openInterest"].c_str() );
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

void OptionChainView::drawUnderlying()
{
    ImGui::Text( "%s | %s - (%s)", model.getOptionChainData().getUnderlyingDataVariable("description").c_str(), 
                                   model.getOptionChainData().getOptionChainDataVariable("symbol").c_str(), 
                                   model.getOptionChainData().getUnderlyingDataVariable("markPercentChange").c_str());

    ImGui::Text("Bid: %s | Ask: %s", model.getOptionChainData().getUnderlyingDataVariable("bid").c_str(), model.getOptionChainData().getUnderlyingDataVariable("ask").c_str());
    ImGui::Text("Open: %s | Close: %s", model.getOptionChainData().getUnderlyingDataVariable("open").c_str(), model.getOptionChainData().getUnderlyingDataVariable("close").c_str());
    ImGui::Text("High: %s | Low: %s", model.getOptionChainData().getUnderlyingDataVariable("highPrice").c_str(), model.getOptionChainData().getUnderlyingDataVariable("lowPrice").c_str());
    ImGui::Text("Total Volume: %s", model.getOptionChainData().getUnderlyingDataVariable("totalVolume").c_str());
    ImGui::Text("Exchange: %s", model.getOptionChainData().getUnderlyingDataVariable("exchangeName").c_str());

    ImGui::Separator();
    ImGui::Text("Implied Volatility: %s", model.getOptionChainData().getOptionChainDataVariable("volatility").c_str());
    ImGui::Text("Interest Rate: %s", model.getOptionChainData().getOptionChainDataVariable("interestRate").c_str());
    ImGui::Text("Naive Gamma Exposure: $%.2f", model.getGammaExposure());
    ImGui::Text("Skew-Adjusted Gamma Exposure: N/A");
    ImGui::Text("GEX Flip Point: N/A");
    ImGui::Text("Distance to Flip: N/A");
    ImGui::Text("Call Skew: N/A");
    ImGui::Text("$ GEX @ Next Expiry: N/A");
    ImGui::Separator();

    static double xs2[10], ys2[10];
    for (int i = 0; i < 10; ++i) {
        xs2[i] = i;
        if (i > 4) 
            ys2[i] = i + 1;
        else
            ys2[i] = 0;
    }
    if (ImPlot::BeginPlot("Risk Chart")) {
        ImPlot::PlotLine("Return", xs2, ys2, 10);
        ImPlot::EndPlot();
    }
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
        if (ImGui::BeginTable("OptionChainTable", 2, ImGuiTableFlags_NoBordersInBody)) {
            ImGui::TableSetupColumn("Underlying", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Option Chain", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextColumn(); drawSearch(); drawUnderlying();
            ImGui::TableNextColumn(); drawChain();
            ImGui::EndTable();
        }
    } else {
        drawSearch();
    }
}
