#ifndef Controller_hpp
#define Controller_hpp

#include <SDL.h>
#include <implot/implot.h>
#include <implot/implot_internal.h>

#include "model/model.h"
#include "view/Chart/chart_view.h"
#include "view/Console/console_view.h"
#include "view/Login/login_view.h"
#include "view/Options/OptionChain.hpp"
#include "view/ViewManager.hpp"

namespace premia {
class Controller {
 private:
  bool active = false;
  void quit() { active = false; }

  Model model;
  ViewManager viewManager;
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;

  void initWindow();
  void initEvents();

 public:
  Controller() = default;

  bool isActive() const;

  void onEntry();
  void onInput();
  void onLoad() const;
  void doRender();
  void onExit();
};
}  // namespace premia

#endif