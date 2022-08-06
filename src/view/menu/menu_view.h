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
 public:
  MenuView() = default;
  std::string getName() override;
  void addLogger(const Logger& logger) override;
  void addEvent(const std::string&, const EventHandler&) override;
  void Update() override;

 private:
  void DrawFileMenu();
  void DrawTradeMenu();
  void DrawChartsMenu();
  void DrawAnalyzeMenu();
  void DrawColumnOptions(int x);
  void DrawViewMenu();
  void DrawHelpMenu();
  void DrawScreen();

  ImGuiStyle* ref = nullptr;
  bool initialized = false;
  bool about = false;

  EventMap events;
  Logger logger;
};
}  // namespace premia

#endif