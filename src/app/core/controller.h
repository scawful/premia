#ifndef Controller_hpp
#define Controller_hpp

#include <SDL.h>
#include <imgui/backends/imgui_impl_sdl.h>
#include <imgui/backends/imgui_impl_sdlrenderer.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <implot/implot.h>
#include <implot/implot_internal.h>

#include <memory>
#include <string>

#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "view/chart/chart_view.h"
#include "view/console/console_view.h"
#include "view/login/login_view.h"
#include "view/options/option_chain.h"
#include "view/view_manager.h"
#include "view/workspace.h"

namespace premia {

class Controller {
 public:
  Controller() = default;

  bool isActive() const;

  absl::Status onEntry();
  void onInput();
  void onLoad();
  void doRender();
  void onExit();

 private:
  struct sdl_deleter {
    void operator()(SDL_Window *p) const { SDL_DestroyWindow(p); }
    void operator()(SDL_Renderer *p) const { SDL_DestroyRenderer(p); }
    void operator()(SDL_Texture *p) const { SDL_DestroyTexture(p); }
  };
  absl::Status CreatePremiaWindow();
  absl::Status CreatePremiaRenderer();
  absl::Status CreatePremiaGuiContext();
  void quit() { active = false; }

  bool active = false;
  Workspace workspace_;
  std::shared_ptr<SDL_Window> window_;
  std::shared_ptr<SDL_Renderer> renderer_;
};

}  // namespace premia

#endif