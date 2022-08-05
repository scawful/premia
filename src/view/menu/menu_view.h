#ifndef MenuView_hpp
#define MenuView_hpp

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <string>

#include "gfx/IconsMaterialDesign.h"
#include "metatypes.h"
#include "model/model.h"
#include "view/view.h"

namespace premia {
class MenuView : public View {
 private:
  // Local Variables
  ImGuiStyle* ref = nullptr;
  bool initialized = false;
  bool about = false;

  // Premia Context Utilities
  EventMap events;
  Logger logger;

  void drawFileMenu() const;
  void drawTradeMenu() const;
  void drawChartsMenu() const;
  void drawAnalyzeMenu() const;
  void drawColumnOptions(int x) const;
  void drawViewMenu() const;
  void drawHelpMenu();
  void drawScreen();

 public:
  String getName() override;
  void addLogger(const Logger& logger) override;
  void addEvent(CRString, const EventHandler&) override;
  void update() override;
};
}  // namespace premia

#endif