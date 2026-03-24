#include "option_chain.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <implot/implot.h>
#include <implot/implot_internal.h>

#include <string>

#include "metatypes.h"
#include "premia/core/application/composition_root.hpp"
#include "model/options/options_model.h"
#include "view/core/IconsMaterialDesign.h"
#include "view/view.h"

namespace premia {

namespace {

auto StrategyValue(int index) -> const char* {
  switch (index) {
    case 0:
      return "SINGLE";
    case 1:
      return "ANALYTICAL";
    case 2:
      return "COVERED";
    case 3:
      return "VERTICAL";
    case 4:
      return "CALENDAR";
    case 5:
      return "STRANGLE";
    case 6:
      return "STRADDLE";
    case 7:
      return "BUTTERFLY";
    case 8:
      return "CONDOR";
    case 9:
      return "DIAGONAL";
    case 10:
      return "COLLAR";
    case 11:
      return "ROLL";
    default:
      return "SINGLE";
  }
}

}  // namespace

void OptionChainView::FetchOptionChain() {
  if (symbol.empty() || strike_count_.empty()) {
    return;
  }
  model.fetchOptionChain(symbol, strike_count_, StrategyValue(current_strategy_),
                         "ALL", "ALL", "ALL");
  model.calculateGammaExposure();
  current_expiration_index_ = 0;
  pending_refresh_ = false;
}

void OptionChainView::DrawSearch() {
  if (ImGui::BeginTable("SearchTable", 4, ImGuiTableFlags_SizingStretchProp,
                        ImVec2(ImGui::GetContentRegionAvail().x, 0.f))) {
    ImGui::TableSetupScrollFreeze(0, 1);
    ImGui::TableSetupColumn("Symbol", ImGuiTableColumnFlags_None);
    ImGui::TableSetupColumn("Strikes", ImGuiTableColumnFlags_None);
    ImGui::TableSetupColumn("Strategy", ImGuiTableColumnFlags_None);
    ImGui::TableSetupColumn("---", ImGuiTableColumnFlags_None);

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(50.f);
    ImGui::InputText("##symbol", &symbol, ImGuiInputTextFlags_CharsUppercase);
    if (ImGui::IsItemDeactivatedAfterEdit() && !symbol.empty() &&
        symbol_change_handler_) {
      symbol_change_handler_(symbol);
    }
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(50.f);
    ImGui::InputText("##strikeCount", &strike_count_,
                     ImGuiInputTextFlags_CharsDecimal);
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(75.f);
    ImGui::Combo("##strategy", &current_strategy_,
                 "SINGLE\0ANALYTICAL\0COVERED\0VERTICAL\0CALENDAR\0STRANGLE\0ST"
                 "RADDLE\0BUTTERFLY\0CONDOR\0DIAGONAL\0COLLAR\0ROLL\0");
    ImGui::TableNextColumn();
    if (ImGui::Button(ICON_MD_QUERY_STATS,
                      ImVec2(ImGui::GetContentRegionAvail().x, 0.f)) &&
        !strike_count_.empty()) {
      pending_refresh_ = true;
      if (symbol_change_handler_) {
        symbol_change_handler_(symbol);
      }
    }
    ImGui::EndTable();
  }

  ImGui::Separator();
}

void OptionChainView::DrawChain() {
  static bool select_options[] = {false};
  static ImGuiTableFlags flags =
      ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg |
      ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
      ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
      ImGuiTableFlags_Hideable | ImGuiTableFlags_SizingStretchProp;

  const auto& snapshot = model.getCoreSnapshot();
  if (snapshot.expirations.empty()) {
    ImGui::TextDisabled("No option-chain expirations available.");
    return;
  }
  if (current_expiration_index_ >= static_cast<int>(snapshot.expirations.size())) {
    current_expiration_index_ = 0;
  }

  ImGui::Text("Gamma at Expiry $%.0f",
              model.getGammaAtExpiry(current_expiration_index_));
  if (ImGui::BeginCombo("Expiration Date",
                        snapshot.expirations[current_expiration_index_].label.c_str(),
                        ImGuiComboFlags_None)) {
    for (int n = 0; n < snapshot.expirations.size(); n++) {
      const bool is_selected = (current_expiration_index_ == n);
      if (ImGui::Selectable(snapshot.expirations[n].label.c_str(), is_selected))
        current_expiration_index_ = n;

      // Set the initial focus when opening the combo (scrolling + keyboard
      // navigation focus)
      if (is_selected) ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }

  if (ImGui::BeginTable("OptionChainTable", 19, flags,
                        ImGui::GetContentRegionAvail())) {
    ImGui::TableSetupScrollFreeze(0, 1);  // Make top row always visible

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
    clipper.Begin(
        static_cast<int>(snapshot.expirations[current_expiration_index_].rows.size()));
    while (clipper.Step()) {
      for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
        const auto& option_row =
            snapshot.expirations[current_expiration_index_].rows[row];
        ImGui::TableNextRow();
        for (int column = 0; column < 19; column++) {
          ImGui::TableSetColumnIndex(column);
          switch (column) {
            case 0:
              ImGui::Text("%s", option_row.call_bid.c_str());
              break;
            case 1:
              ImGui::Text("%s", option_row.call_ask.c_str());
              break;
            case 2:
              ImGui::Text("%s", option_row.call_last.c_str());
              break;
            case 3:
              ImGui::Text("%s", option_row.call_change.c_str());
              break;
            case 4:
              ImGui::Text("%s", option_row.call_delta.c_str());
              break;
            case 5:
              ImGui::Text("%s", option_row.call_gamma.c_str());
              break;
            case 6:
              ImGui::Text("%s", option_row.call_theta.c_str());
              break;
            case 7:
              ImGui::Text("%s", option_row.call_vega.c_str());
              break;
            case 8:
              ImGui::Text("%s", option_row.call_open_interest.c_str());
              break;
            case 9:
              ImGui::Selectable(option_row.strike.c_str(), &select_options[row]);
              break;
            case 10:
              ImGui::Text("%s", option_row.put_bid.c_str());
              break;
            case 11:
              ImGui::Text("%s", option_row.put_ask.c_str());
              break;
            case 12:
              ImGui::Text("%s", option_row.put_last.c_str());
              break;
            case 13:
              ImGui::Text("%s", option_row.put_change.c_str());
              break;
            case 14:
              ImGui::Text("%s", option_row.put_delta.c_str());
              break;
            case 15:
              ImGui::Text("%s", option_row.put_gamma.c_str());
              break;
            case 16:
              ImGui::Text("%s", option_row.put_theta.c_str());
              break;
            case 17:
              ImGui::Text("%s", option_row.put_vega.c_str());
              break;
            case 18:
              ImGui::Text("%s", option_row.put_open_interest.c_str());
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

void OptionChainView::DrawUnderlying() {
  const auto& snapshot = model.getCoreSnapshot();
  if (ImGui::TreeNode("Underlying")) {
    ImGui::Text("%s | %s", snapshot.description.c_str(), snapshot.symbol.c_str());
    ImGui::Text("Bid: %s | Ask: %s", snapshot.bid.c_str(), snapshot.ask.c_str());
    ImGui::Text("Open: %s | Close: %s", snapshot.open_price.c_str(),
                snapshot.close_price.c_str());
    ImGui::Text("High: %s | Low: %s", snapshot.high_price.c_str(),
                snapshot.low_price.c_str());
    ImGui::Text("Total Volume: %s", snapshot.total_volume.c_str());
    ImGui::Text("Implied Volatility: %s", snapshot.volatility.c_str());
    ImGui::Text("Naive Gamma Exposure: $%.2f", model.getGammaExposure());
    ImGui::Text("Skew-Adjusted Gamma Exposure: N/A");
    ImGui::Text("GEX Flip Point: N/A");
    ImGui::Text("Distance to Flip: N/A");
    ImGui::TreePop();
  }
  ImGui::Separator();

  ImGuiStyle& style = ImGui::GetStyle();
  ImGui::PushStyleVar(
      ImGuiStyleVar_FramePadding,
      ImVec2(style.FramePadding.x, (float)(int)(style.FramePadding.y * 0.60f)));
  ImGui::PushStyleVar(
      ImGuiStyleVar_ItemSpacing,
      ImVec2(style.ItemSpacing.x, (float)(int)(style.ItemSpacing.y * 0.60f)));

  if (ImPlot::BeginSubplots("##priceHistoryChart", 3, 2, ImVec2(-1, 475.f),
                            ImPlotSubplotFlags_LinkAllX)) {
    auto size = (int)model.getDatetimeEpochArray().size();
    auto func = [](void* data, int idx) -> ImPlotPoint {
      GEXEpochPair const* dataPair = (GEXEpochPair*)data;
      double const* epochArray = dataPair->epochArray;
      double const* gammaArray = dataPair->gammaArray;
      return ImPlotPoint(epochArray[idx], gammaArray[idx]);
    };

    // ImPlot::GetStyle().UseLocalTime = true;
    // if (ImPlot::BeginPlot("Gamma Exposure", ImVec2(-1, -1))) {
    //   ImPlot::SetupLegend(ImPlotLocation_NorthEast, ImPlotLegendFlags_None);
    //   ImPlot::SetupAxisFormat(ImAxis_Y1, "$%.0f");
    //   ImPlot::SetupAxes("Date", "Price", dateFlags, priceFlags);
    //   GEXEpochPair dataPairing(model.getDatetimeEpochArray().data(),
    //                            model.getGammaAtExpiryList().data());
    //   ImPlot::PlotLineG("##totalGex", func, &dataPairing, size);
    //   ImPlot::EndPlot();
    // }

    // if (ImPlot::BeginPlot("Vanna Exposure", ImVec2(-1, -1))) {
    //   ImPlot::SetupAxisFormat(ImAxis_Y1, "%.4f");
    //   ImPlot::SetupAxes("Date", "Price", dateFlags, priceFlags);
    //   GEXEpochPair vannaPair(model.getDatetimeEpochArray().data(),
    //                          model.getNaiveVannaExposureList().data());
    //   ImPlot::PlotLineG("##vanna", func, &vannaPair, size);
    //   ImPlot::EndPlot();
    // }

    // if (ImPlot::BeginPlot("Call GEX", ImVec2(-1, -1))) {
    //   ImPlot::SetupAxisFormat(ImAxis_Y1, "$%.0f");
    //   ImPlot::SetupAxes("Date", "Price", dateFlags, priceFlags);
    //   GEXEpochPair callGamma(model.getDatetimeEpochArray().data(),
    //                          model.getCallGammaAtExpiryList().data());
    //   ImPlot::PlotLineG("##call", func, &callGamma, size);
    //   ImPlot::EndPlot();
    // }

    // if (ImPlot::BeginPlot("Vega Exposure", ImVec2(-1, -1))) {
    //   ImPlot::SetupAxisFormat(ImAxis_Y1, "%.2f");
    //   ImPlot::SetupAxes("Date", "Price", dateFlags, priceFlags);
    //   GEXEpochPair volPair(model.getDatetimeEpochArray().data(),
    //                        model.getVegaExposureArray().data());
    //   ImPlot::PlotLineG("##vega", func, &volPair, size);
    //   ImPlot::EndPlot();
    // }

    // if (ImPlot::BeginPlot("Put GEX", ImVec2(-1, -1))) {
    //   ImPlot::SetupAxisFormat(ImAxis_Y1, "$%.0f");
    //   ImPlot::SetupAxes("Date", "Price", dateFlags, priceFlags);
    //   GEXEpochPair putGamma(model.getDatetimeEpochArray().data(),
    //                         model.getPutGammaAtExpiryList().data());
    //   ImPlot::PlotLineG("##put", func, &putGamma, size);
    //   ImPlot::EndPlot();
    // }

    // if (ImPlot::BeginPlot("Volga Exposure", ImVec2(-1, -1))) {
    //   ImPlot::SetupAxisFormat(ImAxis_Y1, "%.2f");
    //   ImPlot::SetupAxes("Date", "Price", dateFlags, priceFlags);
    //   GEXEpochPair volgaPair(model.getDatetimeEpochArray().data(),
    //                          model.getVolgaExposureArray().data());
    //   ImPlot::PlotLineG("##volga", func, &volgaPair, size);
    //   ImPlot::EndPlot();
    // }

    ImPlot::EndSubplots();
  }
  ImGui::PopStyleVar(2);
  ImGui::Separator();
}

std::string OptionChainView::getName() { return "Option Chain"; }

void OptionChainView::addLogger(const Logger& newLogger) {
  this->logger = newLogger;
}

void OptionChainView::addEvent(const std::string& key,
                               const EventHandler& event) {
  this->events[key] = event;
}

void OptionChainView::Update() {
  if (pending_refresh_) {
    FetchOptionChain();
  }
  if (ImGui::Button("Refresh Core Option Snapshot")) {
    pending_refresh_ = true;
  }

  if (model.isActive()) {
    if (ImGui::BeginTable("OptionChainTable", 1,
                          ImGuiTableFlags_NoBordersInBody)) {
      ImGui::TableSetupColumn("Option Chain",
                              ImGuiTableColumnFlags_WidthStretch);
      ImGui::TableNextColumn();
      DrawSearch();
      DrawUnderlying();
      DrawChain();
      ImGui::EndTable();
    }
  } else {
    DrawSearch();
    DrawCoreOptionPreview();
  }
}

void OptionChainView::DrawCoreOptionPreview() {
  const auto snapshot = core::application::CompositionRoot::Instance()
                             .Options()
                             .GetOptionChainSnapshot(symbol.empty() ? "SPY" : symbol,
                                                     strike_count_.empty() ? "8"
                                                                           : strike_count_,
                                                     StrategyValue(current_strategy_),
                                                     "ALL", "ALL", "ALL");

  ImGui::Separator();
  ImGui::Text("Core Options Preview");
  ImGui::TextDisabled(
      "This fallback view is driven by premia_core option contracts.");
  ImGui::Text("%s | %s", snapshot.description.c_str(), snapshot.symbol.c_str());
  ImGui::Text("Bid: %s | Ask: %s | Vol: %s", snapshot.bid.c_str(),
              snapshot.ask.c_str(), snapshot.total_volume.c_str());
  ImGui::Text("Volatility: %s | Gamma Exposure: %s",
              snapshot.volatility.c_str(), snapshot.gamma_exposure.c_str());

  if (!snapshot.expirations.empty()) {
    const auto& expiry = snapshot.expirations.front();
    ImGui::Separator();
    ImGui::Text("Expiry: %s | Gamma @ Expiry: %s", expiry.label.c_str(),
                expiry.gamma_at_expiry.c_str());

    if (ImGui::BeginTable("CoreOptionPreview", 7,
                          ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                              ImGuiTableFlags_SizingStretchSame)) {
      ImGui::TableSetupColumn("Strike");
      ImGui::TableSetupColumn("Call Bid");
      ImGui::TableSetupColumn("Call Ask");
      ImGui::TableSetupColumn("Call OI");
      ImGui::TableSetupColumn("Put Bid");
      ImGui::TableSetupColumn("Put Ask");
      ImGui::TableSetupColumn("Put OI");
      ImGui::TableHeadersRow();

      for (const auto& row : expiry.rows) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%s", row.strike.c_str());
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%s", row.call_bid.c_str());
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%s", row.call_ask.c_str());
        ImGui::TableSetColumnIndex(3);
        ImGui::Text("%s", row.call_open_interest.c_str());
        ImGui::TableSetColumnIndex(4);
        ImGui::Text("%s", row.put_bid.c_str());
        ImGui::TableSetColumnIndex(5);
        ImGui::Text("%s", row.put_ask.c_str());
        ImGui::TableSetColumnIndex(6);
        ImGui::Text("%s", row.put_open_interest.c_str());
      }

      ImGui::EndTable();
    }
  }
}

void OptionChainView::SetSymbol(const std::string& next_symbol) {
  if (next_symbol.empty() || symbol == next_symbol) {
    return;
  }
  symbol = next_symbol;
  pending_refresh_ = true;
}

void OptionChainView::SetSymbolChangeHandler(
    const std::function<void(const std::string&)>& handler) {
  symbol_change_handler_ = handler;
}
}  // namespace premia
