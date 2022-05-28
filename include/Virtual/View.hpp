#ifndef View_hpp
#define View_hpp

#include "Premia.hpp"
#include "Interface/HLXT.hpp"
#include "Library/SDL.hpp"
#include "Library/ImGui.hpp"
#include "Library/ImPlot.hpp"
#include "Library/Boost.hpp"

namespace Premia { 
class View {
 public:
  View()=default;
  virtual ~View()=default;

  virtual String getName() = 0;
  virtual void addEvent(CRString, const EventHandler &) = 0;
  virtual void addLogger(const Logger &) = 0;
  virtual void update() = 0;
};
}  // namespace Premia

#endif