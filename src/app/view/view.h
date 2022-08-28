#ifndef View_hpp
#define View_hpp

#include <SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <implot/implot.h>
#include <implot/implot_internal.h>

#include "premia.h"
// #include "core/HLXT.hpp"

namespace premia {
class View {
 public:
  View() = default;
  virtual ~View() = default;

  virtual std::string getName() = 0;
  virtual void addEvent(const std::string &, const EventHandler &) = 0;
  virtual void addLogger(const Logger &) = 0;
  virtual void Update() = 0;
};
}  // namespace premia

#endif