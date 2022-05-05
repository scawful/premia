#include "OptionChain.hpp"

void OptionChainView::drawSearch()
{
    static String ticker;
    static String count;
    static String strike;
    static int current_strategy = 0;

    if (ImGui::BeginTable("SearchTable", 4, ImGuiTableFlags_SizingStretchProp, ImVec2(ImGui::GetContentRegionAvail().x, 0.f))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Symbol", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Strikes", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Strategy", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("---", ImGuiTableColumnFlags_None);

        ImGui::TableNextColumn(); 
        ImGui::SetNextItemWidth(50.f);
        ImGui::InputText("##symbol", &ticker, ImGuiInputTextFlags_CharsUppercase);
        ImGui::TableNextColumn(); 
        ImGui::SetNextItemWidth(50.f);
        ImGui::InputText("##strikeCount", &count, ImGuiInputTextFlags_CharsDecimal);
        ImGui::TableNextColumn(); 
        ImGui::SetNextItemWidth(75.f);
        ImGui::Combo("##strategy", &current_strategy, "SINGLE\0ANALYTICAL\0COVERED\0VERTICAL\0CALENDAR\0STRANGLE\0STRADDLE\0BUTTERFLY\0CONDOR\0DIAGONAL\0COLLAR\0ROLL\0");
        ImGui::TableNextColumn();
        if (ImGui::Button("Fetch", ImVec2(ImGui::GetContentRegionAvail().x, 0.f)) && !count.empty()) {
            model.fetchOptionChain( ticker, count, "SINGLE", "ALL", "ALL", "ALL" );
            model.calculateGammaExposure();
        }
        ImGui::EndTable();
    }

    ImGui::Separator();
}

void OptionChainView::drawChain()
{
    static bool select_options[] = {false};
    static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_SizingStretchProp;

    static int current_item = 0;
    if (ImGui::BeginCombo("Expiration Date", model.getDateTime(current_item).c_str(), ImGuiComboFlags_None))
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

    if (ImGui::BeginTable("OptionChainTable", 19, flags, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.75f))) {
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible

        ImGui::TableSetupColumn("Bid", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Ask", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Last", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Chng", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Delta", ImGuiTableColumnFlags_DefaultHide);
        ImGui::TableSetupColumn("Gamma", ImGuiTableColumnFlags_DefaultHide);
        ImGui::TableSetupColumn("Theta", ImGuiTableColumnFlags_DefaultHide);
        ImGui::TableSetupColumn("Vega", ImGuiTableColumnFlags_DefaultHide);
        ImGui::TableSetupColumn("Open Int", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Strike", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Bid", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Ask", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Last", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Chng", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Delta", ImGuiTableColumnFlags_DefaultHide);
        ImGui::TableSetupColumn("Gamma", ImGuiTableColumnFlags_DefaultHide);
        ImGui::TableSetupColumn("Theta", ImGuiTableColumnFlags_DefaultHide);
        ImGui::TableSetupColumn("Vega", ImGuiTableColumnFlags_DefaultHide);
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
    ImGui::Text( "%s | %s (%s)", model.getOptionChainData().getUnderlyingDataVariable("description").c_str(), 
                                   model.getOptionChainData().getOptionChainDataVariable("symbol").c_str(), 
                                   model.getOptionChainData().getUnderlyingDataVariable("markPercentChange").c_str());

    ImGui::Text("Bid: %s | Ask: %s", model.getOptionChainData().getUnderlyingDataVariable("bid").c_str(), model.getOptionChainData().getUnderlyingDataVariable("ask").c_str());
    ImGui::Text("Open: %s | Close: %s", model.getOptionChainData().getUnderlyingDataVariable("openPrice").c_str(), model.getOptionChainData().getUnderlyingDataVariable("close").c_str());
    ImGui::Text("High: %s | Low: %s", model.getOptionChainData().getUnderlyingDataVariable("highPrice").c_str(), model.getOptionChainData().getUnderlyingDataVariable("lowPrice").c_str());
    ImGui::Text("Total Volume: %s", model.getOptionChainData().getUnderlyingDataVariable("totalVolume").c_str());
    ImGui::Text("Exchange: %s", model.getOptionChainData().getUnderlyingDataVariable("exchangeName").c_str());

    ImGui::Separator();
    ImGui::Text("Implied Volatility: %s", model.getOptionChainData().getOptionChainDataVariable("volatility").c_str());
    ImGui::Text("Naive Gamma Exposure: $%.2f", model.getGammaExposure());
    ImGui::Text("Skew-Adjusted Gamma Exposure: N/A");
    ImGui::Text("GEX Flip Point: N/A");
    ImGui::Text("Distance to Flip: N/A");
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
    if (ImPlot::BeginPlot("Risk Chart", ImVec2(-1, 225.f))) {
        ImPlot::PlotLine("Return", xs2, ys2, 10);
        ImPlot::EndPlot();
    }
}

String 
OptionChainView::getName() {
    return "Option Chain";
}

void OptionChainView::addLogger(const ConsoleLogger & newLogger)
{
    this->logger = newLogger;
}

void OptionChainView::addEvent(CRString key, const EventHandler & event)
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
