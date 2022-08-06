#ifndef LoginView_hpp
#define LoginView_hpp

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <string>

#include "core/TDA.hpp"
#include "view/core/IconsMaterialDesign.h"
#include "metatypes.h"
#include "model/model.h"
#include "view/view.h"

namespace premia {
class LoginView : public View {
 private:
  bool initialized = false;

  Logger logger;
  std::unordered_map<std::string, EventHandler> events;

  void DrawScreen() const;

 public:
  std::string getName() override;
  void addLogger(const Logger& logger) override;
  void addEvent(const std::string& , const EventHandler&) override;
  void Update() override;
};
}  // namespace premia

#endif