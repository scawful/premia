#ifndef Controller_hpp
#define Controller_hpp

#include <SDL.h>
#include <implot/implot.h>
#include <implot/implot_internal.h>

#include "view/chart/chart_view.h"
#include "view/console/console_view.h"
#include "view/login/login_view.h"
#include "view/options/option_chain.h"
#include "view/workspace.h"

namespace premia {

class Controller {
 public:
  Controller() = default;

  bool isActive() const;

  void onEntry();
  void onInput();
  void onLoad();
  void doRender();
  void onExit();

 private:
  void quit() { active = false; }

  bool active = false;

  Workspace workspace_;
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;

  void initWindow();
};

}  // namespace premia

#endif