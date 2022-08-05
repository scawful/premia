#include "controller.h"

#include <SDL.h>
#include <imgui/backends/imgui_impl_sdl.h>
#include <imgui/backends/imgui_impl_sdlrenderer.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <implot/implot.h>
#include <implot/implot_internal.h>

#include "model/model.h"
#include "view/chart/chart_view.h"
#include "view/console/console_view.h"
#include "view/login/login_view.h"
#include "view/options/option_chain.h"
#include "view/view_manager.h"

namespace premia {

constexpr size_t LOGIN_WIDTH = 220;
constexpr size_t LOGIN_HEIGHT = 200;
constexpr size_t SCREEN_WIDTH = 1200;
constexpr size_t SCREEN_HEIGHT = 800;

static void ColorsPremia(ImGuiStyle* dst = nullptr) {
  ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
  ImVec4* colors = style->Colors;

  style->WindowPadding.x = 5.0f;
  style->WindowPadding.y = 10.0f;
  style->FramePadding.x = 5.0f;
  style->FramePadding.y = 5.0f;
  style->CellPadding.x = 5.0f;
  style->CellPadding.y = 5.0f;
  style->ItemSpacing.x = 15.f;
  style->ItemSpacing.y = 10.0f;
  style->ItemInnerSpacing.x = 8.0f;
  style->TouchExtraPadding.x = 4.0f;
  style->IndentSpacing = 0.0f;
  style->ScrollbarSize = 13.0f;
  style->GrabMinSize = 11.0f;

  // borders
  style->WindowBorderSize = 1;
  style->ChildBorderSize = 1;
  style->PopupBorderSize = 1;
  style->FrameBorderSize = 1;
  style->TabBorderSize = 1;

  // rounding
  style->WindowRounding = 0;
  style->ChildRounding = 0;
  style->FrameRounding = 10;
  style->PopupRounding = 0;
  style->ScrollbarRounding = 6;
  style->GrabRounding = 6;
  style->LogSliderDeadzone = 5;
  style->TabRounding = 4;

  // alignment
  style->WindowTitleAlign.x = 0.50f;
  style->WindowTitleAlign.y = 0.50f;
  style->ButtonTextAlign.x = 0.50f;
  style->ButtonTextAlign.y = 0.50f;
  // style->SelectableTextAlign.

  colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
  colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
  colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
  colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
  colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
  colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
  colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
  colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
  colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
  colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
  colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
  colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
  colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
  colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
  colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
  colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
  colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
  colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
  colors[ImGuiCol_Tab] =
      ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
  colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
  colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive],
                                      colors[ImGuiCol_TitleBgActive], 0.60f);
  colors[ImGuiCol_TabUnfocused] =
      ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
  colors[ImGuiCol_TabUnfocusedActive] =
      ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
  colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
  colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
  colors[ImGuiCol_TableBorderStrong] =
      ImVec4(0.31f, 0.31f, 0.35f, 1.00f);  // Prefer using Alpha=1.0 here
  colors[ImGuiCol_TableBorderLight] =
      ImVec4(0.23f, 0.23f, 0.25f, 1.00f);  // Prefer using Alpha=1.0 here
  colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
  colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
  colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
  colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

constexpr size_t TITLE_BORDER = 20;
constexpr size_t RESIZE_BORDER = 3;

static SDL_HitTestResult WindowCallback(SDL_Window* win, const SDL_Point* point,
                                        void* data);

/**
 * @brief Handles dragging the window by the title bar
 *        and resizing the window at its edges and corners
 *
 * @param win
 * @param pt
 * @param data
 * @return SDL_HitTestResult
 */
static SDL_HitTestResult WindowCallback(SDL_Window* win, const SDL_Point* pt,
                                        void* data) {
  int w;
  int h;
  SDL_GetWindowSize(win, &w, &h);

  // dragging this pixel moves the window.
  if (pt->y < TITLE_BORDER && pt->x < (w - TITLE_BORDER)) {
    return SDL_HITTEST_DRAGGABLE;
  }

#define RESIZE_HIT(name) \
  { return SDL_HITTEST_RESIZE_##name; }

  // SDL_HITTEST_RESIZE_* - dragging this pixel resizes a specific edge (or
  // edges) of the window. (Here * is one of: TOPLEFT, TOP, TOPRIGHT, RIGHT,
  // BOTTOMRIGHT, BOTTOM, BOTTOMLEFT, LEFT).
  if (pt->x < RESIZE_BORDER && pt->y < RESIZE_BORDER) {
    RESIZE_HIT(TOPLEFT)
  } else if (pt->x > RESIZE_BORDER && pt->x < w - RESIZE_BORDER &&
             pt->y < RESIZE_BORDER) {
    RESIZE_HIT(TOP)
  } else if (pt->x > w - RESIZE_BORDER && pt->y < RESIZE_BORDER) {
    RESIZE_HIT(TOPRIGHT)
  } else if (pt->x > w - RESIZE_BORDER && pt->y > RESIZE_BORDER &&
             pt->y < h - RESIZE_BORDER) {
    RESIZE_HIT(RIGHT)
  } else if (pt->x > w - RESIZE_BORDER && pt->y > h - RESIZE_BORDER) {
    RESIZE_HIT(BOTTOMRIGHT)
  } else if (pt->x < w - RESIZE_BORDER && pt->x > RESIZE_BORDER &&
             pt->y > h - RESIZE_BORDER) {
    RESIZE_HIT(BOTTOM)
  } else if (pt->x < RESIZE_BORDER && pt->y > h - RESIZE_BORDER) {
    RESIZE_HIT(BOTTOMLEFT)
  } else if (pt->x < RESIZE_BORDER && pt->y < h - RESIZE_BORDER &&
             pt->y > RESIZE_BORDER) {
    RESIZE_HIT(LEFT)
  }

  // no action.
  return SDL_HITTEST_NORMAL;
}

/**
 * @brief Initializes the SDL_Window and SDL_Renderer
 *        Starts the ImGui and ImPlot contexts
 *        Loads default fonts and theme
 *        Builds the GUI Frame
 *
 */
void Controller::initWindow() {
  if (SDL_Init(SDL_INIT_EVERYTHING)) {
    SDL_Log("SDL_Init: %s\n", SDL_GetError());
  } else {
    window = SDL_CreateWindow("Premia",                 // window title
                              SDL_WINDOWPOS_UNDEFINED,  // initial x position
                              SDL_WINDOWPOS_UNDEFINED,  // initial y position
                              LOGIN_WIDTH,              // width, in pixels
                              LOGIN_HEIGHT,             // height, in pixels
                              SDL_WINDOW_RESIZABLE |    // flags
                                  SDL_WINDOW_BORDERLESS);
  }

  if (window == nullptr) {
    SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
    SDL_Quit();
    throw premia::FatalException();
  } else {
    renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
      SDL_Log("SDL_CreateRenderer: %s\n", SDL_GetError());
      SDL_Quit();
      throw premia::FatalException();
    } else {
      SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
      SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    }
  }

  // Create the ImGui and ImPlot contexts
  ImGui::CreateContext();
  ImPlot::CreateContext();

  // Initialize ImGui for SDL
  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer_Init(renderer);

  // Load available fonts
  const ImGuiIO& io = ImGui::GetIO();
  io.Fonts->AddFontFromFileTTF("assets/Cousine-Regular.ttf", 13.0f);
  // merge in icons from Google Material Design
  static const ImWchar icons_ranges[] = {ICON_MIN_MD, 0xf900, 0};
  ImFontConfig icons_config;
  icons_config.MergeMode = true;
  icons_config.GlyphOffset.y = 6.0f;
  icons_config.GlyphMinAdvanceX = 13.0f;
  icons_config.PixelSnapH = true;
  io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_MD, 18.0f, &icons_config,
                               icons_ranges);
  io.Fonts->AddFontFromFileTTF("assets/DroidSans.ttf", 13.0f);
  io.Fonts->AddFontFromFileTTF("assets/Karla-Regular.ttf", 12.0f);
  io.Fonts->AddFontFromFileTTF("assets/Roboto-Medium.ttf", 12.0f);

  // Build a new ImGui frame
  ImGui_ImplSDLRenderer_NewFrame();
  ImGui_ImplSDL2_NewFrame(window);

  SDL_SetWindowHitTest(this->window, &WindowCallback, nullptr);
  SDL_SetWindowResizable(this->window, SDL_TRUE);
  ColorsPremia();
}

/**
 * @brief Initializes any events the ViewManager may need for modifying the
 * Controller
 *
 */
void Controller::initEvents() {
  viewManager.addEventHandler("login", [this]() -> void {
    SDL_SetWindowSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED);
    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Backspace] = SDL_GetScancodeFromKey(SDLK_BACKSPACE);
    io.KeyMap[ImGuiKey_Enter] = SDL_GetScancodeFromKey(SDLK_RETURN);
    io.KeyMap[ImGuiKey_UpArrow] = SDL_GetScancodeFromKey(SDLK_UP);
    io.KeyMap[ImGuiKey_DownArrow] = SDL_GetScancodeFromKey(SDLK_DOWN);
    io.KeyMap[ImGuiKey_Tab] = SDL_GetScancodeFromKey(SDLK_TAB);
    viewManager.setCurrentView(std::make_shared<PrimaryView>());
    viewManager.setLoggedIn();
  });
  viewManager.addEventHandler("quit", [this]() -> void { this->quit(); });
}

/**
 * @brief Returns program runtime
 *
 * @return true
 * @return false
 */
bool Controller::isActive() const { return active; }

/**
 * @brief Entry point of application
 *
 */
void Controller::onEntry() noexcept(false) {
  initWindow();
  initEvents();
  active = true;
}

/**
 * @brief Handles keyboard and mouse input via SDL_Events
 *
 */
void Controller::onInput() {
  int wheel = 0;
  SDL_Event event;
  ImGuiIO& io = ImGui::GetIO();

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case SDLK_UP:
          case SDLK_DOWN:
          case SDLK_RETURN:
          case SDLK_BACKSPACE:
          case SDLK_TAB:
            io.KeysDown[event.key.keysym.scancode] =
                (event.type == SDL_KEYDOWN);
            break;
          default:
            break;
        }
        break;

      case SDL_KEYUP: {
        int key = event.key.keysym.scancode;
        IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
        io.KeysDown[key] = (event.type == SDL_KEYDOWN);
        io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
        io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
        io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
        io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
        break;
      }
      case SDL_WINDOWEVENT:
        switch (event.window.event) {
          case SDL_WINDOWEVENT_CLOSE:
            active = false;
            break;
          case SDL_WINDOWEVENT_SIZE_CHANGED:
            io.DisplaySize.x = static_cast<float>(event.window.data1);
            io.DisplaySize.y = static_cast<float>(event.window.data2);
            break;
          default:
            break;
        }
        break;
      case SDL_TEXTINPUT:
        io.AddInputCharactersUTF8(event.text.text);
        break;
      case SDL_MOUSEWHEEL:
        wheel = event.wheel.y;
        break;
      default:
        break;
    }
  }

  int mouseX;
  int mouseY;
  const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
  io.DeltaTime = 1.0f / 60.0f;
  io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
  io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
  io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
  io.MouseWheel = static_cast<float>(wheel);
}

/**
 * @brief Refer to View group and ViewManager class
 *
 */
void Controller::onLoad() const { this->viewManager.update(); }

/**
 * @brief Render current contents to the screen
 *
 */
void Controller::doRender() {
  SDL_RenderClear(renderer);
  ImGui::Render();
  ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
  SDL_RenderPresent(renderer);
}

/**
 * @brief Cleanup all resources
 *
 */
void Controller::onExit() {
  ImGui_ImplSDLRenderer_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  renderer = nullptr;
  window = nullptr;
}
}  // namespace premia