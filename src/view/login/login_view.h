#ifndef LoginView_hpp
#define LoginView_hpp

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <string>

#include "singletons/TDA.hpp"
#include "gfx/IconsMaterialDesign.h"
#include "metatypes.h"
#include "model/model.h"
#include "view/view.h"

namespace premia {
class LoginView : public View {
 private:
  bool initialized = false;

  Logger logger;
  std::unordered_map<String, EventHandler> events;

  void drawScreen() const;

 public:
  String getName() override;
  void addLogger(const Logger& logger) override;
  void addEvent(CRString, const EventHandler&) override;
  void update() override;
};
}  // namespace premia

#endif