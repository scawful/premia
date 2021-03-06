#ifndef OptionChainView_hpp
#define OptionChainView_hpp

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <string>

#include "Library/IconsMaterialDesign.h"
#include "Metatypes.hpp"
#include "Model/Options/OptionsModel.hpp"
#include "View/View.hpp"

namespace premia {
class OptionChainView : public View {
 private:
  struct GEXEpochPair {
    double* epochArray;
    double* gammaArray;
    GEXEpochPair(double* epoch, double* gamma)
        : epochArray(epoch), gammaArray(gamma) {}
  };
  String symbol;
  EventMap events;
  Logger logger;
  OptionsModel model;

  void drawSearch();
  void drawChain();
  void drawUnderlying();

 public:
  String getName() override;
  void addLogger(const Logger& logger) override;
  void addEvent(CRString key, const EventHandler& event) override;
  void update() override;

 private:
  ImPlotAxisFlags dateFlags = ImPlotAxisFlags_Time | ImPlotAxisFlags_NoLabel;
  ImPlotAxisFlags priceFlags = ImPlotAxisFlags_NoLabel |
                               ImPlotAxisFlags_AutoFit |
                               ImPlotAxisFlags_RangeFit;
};
}  // namespace premia

#endif