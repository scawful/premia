#ifndef OptionChainView_hpp
#define OptionChainView_hpp

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <implot/implot.h>
#include <implot/implot_internal.h>

#include <string>

#include "view/core/IconsMaterialDesign.h"
#include "metatypes.h"
#include "model/options/options_model.h"
#include "view/view.h"


namespace premia {
class OptionChainView : public View {
 private:
  struct GEXEpochPair {
    double* epochArray;
    double* gammaArray;
    GEXEpochPair(double* epoch, double* gamma)
        : epochArray(epoch), gammaArray(gamma) {}
  };
  std::string symbol = "SPY";
  std::string strike_count_ = "8";
  int current_strategy_ = 0;
  int current_expiration_index_ = 0;
  bool pending_refresh_ = true;
  std::function<void(const std::string&)> symbol_change_handler_;
  std::function<void(const std::string&, const std::string&)> strike_selection_handler_;
  EventMap events;
  Logger logger;
  OptionsModel model;

  void DrawSearch();
  void DrawChain();
  void DrawUnderlying();
  void DrawCoreOptionPreview();
  void FetchOptionChain();

 public:
  std::string getName() override;
  void addLogger(const Logger& logger) override;
  void addEvent(const std::string &key, const EventHandler& event) override;
  void SetSymbol(const std::string& next_symbol);
  void SetSymbolChangeHandler(
      const std::function<void(const std::string&)>& handler);
  void SetStrikeSelectionHandler(
      const std::function<void(const std::string&, const std::string&)>& handler);
  void Update() override;

 private:
  ImPlotAxisFlags dateFlags = ImPlotAxisFlags_NoLabel;
  ImPlotAxisFlags priceFlags = ImPlotAxisFlags_NoLabel |
                               ImPlotAxisFlags_AutoFit |
                               ImPlotAxisFlags_RangeFit;
};
}  // namespace premia

#endif
