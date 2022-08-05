#ifndef Controller_hpp
#define Controller_hpp

#include <SDL.h>
#include <implot/implot.h>
#include <implot/implot_internal.h>

#include "model/model.h"
#include "view/chart/chart_view.h"
#include "view/console/console_view.h"
#include "view/login/login_view.h"
#include "view/options/option_chain.h"
#include "view/view_manager.h"

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