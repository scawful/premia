#ifndef Controller_hpp
#define Controller_hpp

#include "Library/ImPlot.hpp"
#include "Library/SDL.hpp"
#include "Model/Model.hpp"
#include "Style.hpp"
#include "View/Chart/ChartView.hpp"
#include "View/Console/ConsoleView.hpp"
#include "View/Login/LoginView.hpp"
#include "View/Options/OptionChain.hpp"
#include "View/ViewManager.hpp"

namespace Premia {
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
}  // namespace Premia

#endif