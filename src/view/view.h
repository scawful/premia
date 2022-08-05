#ifndef View_hpp
#define View_hpp

#include <SDL2/SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <implot/implot.h>
#include <implot/implot_internal.h>

#include "Interface/HLXT.hpp"
#include "Premia.hpp"

namespace premia {
class View {
 public:
  View() = default;
  virtual ~View() = default;

  virtual String getName() = 0;
  virtual void addEvent(CRString, const EventHandler &) = 0;
  virtual void addLogger(const Logger &) = 0;
  virtual void update() = 0;
};
}  // namespace premia

#endif