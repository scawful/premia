#include "Controller.hpp"

#define TITLE_BORDER 20
#define RESIZE_BORDER 5

void
Controller::initWindow()
{
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        SDL_Log("SDL_Init: %s\n", SDL_GetError());
    } else {
        window = SDL_CreateWindow(
            "Premia",                           // window title
            SDL_WINDOWPOS_UNDEFINED,            // initial x position
            SDL_WINDOWPOS_UNDEFINED,            // initial y position
            Style::LOGIN_WIDTH,                 // width, in pixels
            Style::LOGIN_HEIGHT,                // height, in pixels
            SDL_WINDOW_RESIZABLE                // flags
            | SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN 
        );
    }   

    if (window == nullptr) {
        SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
    } else {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (renderer == nullptr) {
            SDL_Log("SDL_CreateRenderer: %s\n", SDL_GetError());
            SDL_Quit();
        } else {
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        }
    }

    // Create the ImGui and ImPlot contexts
    ImGui::CreateContext();
    ImPlot::CreateContext();

    // Initialize ImGui for SDL 
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);

    const ImGuiIO & io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF("assets/Cousine-Regular.ttf", 14.0f);
    io.Fonts->AddFontFromFileTTF("assets/DroidSans.ttf", 14.0f);
    io.Fonts->AddFontFromFileTTF("assets/Karla-Regular.ttf", 14.0f);
    io.Fonts->AddFontFromFileTTF("assets/Roboto-Medium.ttf", 14.0f);

    // Build a new ImGui frame
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);  

    SDL_SetWindowHitTest(this->window, &WindowCallback, nullptr);
    SDL_SetWindowResizable(this->window, SDL_TRUE);
    Style::ColorsPremia();
}

void 
Controller::initCallbacks()
{
    viewManager.addEventHandler("login", [this] () -> void { \
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
        SDL_RenderPresent(renderer);
        SDL_SetWindowSize(window, Style::SCREEN_WIDTH, Style::SCREEN_HEIGHT);
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        ImGuiIO & io = ImGui::GetIO();
        io.KeyMap[ImGuiKey_Backspace] = SDL_GetScancodeFromKey(SDLK_BACKSPACE);
        io.KeyMap[ImGuiKey_Enter] = SDL_GetScancodeFromKey(SDLK_RETURN);
        io.KeyMap[ImGuiKey_UpArrow] = SDL_GetScancodeFromKey(SDLK_UP);
        io.KeyMap[ImGuiKey_DownArrow] = SDL_GetScancodeFromKey(SDLK_DOWN);
        io.KeyMap[ImGuiKey_Tab] = SDL_GetScancodeFromKey(SDLK_TAB);
        viewManager.setCurrentView(std::make_shared<PrimaryView>());
        viewManager.setLoggedIn(); 
        viewManager.transferEvents();
    });

    // viewManager.addEventHandler("toggleConsoleView", [this] () -> void { viewManager.setConsoleView(); });
    // viewManager.addEventHandler("toggleWatchlistView", [this] () -> void {viewManager.setWatchlistView(); });
    
    viewManager.addEventHandler("goHome", [this] () -> void { viewManager.setCurrentView(std::make_shared<PrimaryView>()); });
    viewManager.addEventHandler("chartView", [this] () -> void { viewManager.setCurrentView(std::make_shared<ChartView>()); });
    viewManager.addEventHandler("linePlotView", [this] () -> void { viewManager.setCurrentView(std::make_shared<LinePlotChart>()); });
    viewManager.addEventHandler("optionChainView", [this] () -> void { viewManager.setCurrentView(std::make_shared<OptionChainView>()); });
    viewManager.addEventHandler("quit", [this] () -> void { this->quit(); });
}

SDL_HitTestResult 
WindowCallback(SDLWindow win, SDLPoint pt, void* data)
{
    int w, h;
    SDL_GetWindowSize(win, &w, &h);

    // // dragging this pixel moves the window.
    if (pt->y < TITLE_BORDER && pt->x < (w - TITLE_BORDER)) {
        return SDL_HITTEST_DRAGGABLE;
    }

    #define REPORT_RESIZE_HIT(name) { \
        SDL_Log("HIT-TEST: RESIZE_" #name "\n"); \
        return SDL_HITTEST_RESIZE_##name; \
    }
    
    if (pt->x < RESIZE_BORDER && pt->y < RESIZE_BORDER) {
        REPORT_RESIZE_HIT(TOPLEFT);
    } else if (pt->x > RESIZE_BORDER && pt->x < w - RESIZE_BORDER && pt->y < RESIZE_BORDER) {
        REPORT_RESIZE_HIT(TOP);
    } else if (pt->x > w - RESIZE_BORDER && pt->y < RESIZE_BORDER) {
        REPORT_RESIZE_HIT(TOPRIGHT); 
    } else if (pt->x > w - RESIZE_BORDER && pt->y > RESIZE_BORDER && pt->y < h - RESIZE_BORDER) {
        REPORT_RESIZE_HIT(RIGHT);
    } else if (pt->x > w - RESIZE_BORDER && pt->y > h - RESIZE_BORDER) {
        REPORT_RESIZE_HIT(BOTTOMRIGHT);
    } else if (pt->x < w - RESIZE_BORDER && pt->x > RESIZE_BORDER && pt->y > h - RESIZE_BORDER) {
        REPORT_RESIZE_HIT(BOTTOM);
    } else if (pt->x < RESIZE_BORDER && pt->y > h - RESIZE_BORDER) {
        REPORT_RESIZE_HIT(BOTTOMLEFT);
    } else if (pt->x < RESIZE_BORDER && pt->y < h - RESIZE_BORDER && pt->y > RESIZE_BORDER) {
        REPORT_RESIZE_HIT(LEFT);
    }
    // SDL_HITTEST_RESIZE_* - dragging this pixel resizes a specific edge (or edges) of the window. (Here * is one of: TOPLEFT, TOP, TOPRIGHT, RIGHT, BOTTOMRIGHT, BOTTOM, BOTTOMLEFT, LEFT).
    // TOPLEFT, TOP, TOPRIGHT, RIGHT, BOTTOMRIGHT, BOTTOM, BOTTOMLEFT, LEFT

    // no action.
    return SDL_HITTEST_NORMAL;
}

Controller::Controller(const ViewManager & vm) 
    : viewManager(vm) { }

bool
Controller::isActive() const {
    return active;
}

void
Controller::onEntry()
{
    initWindow();
    initCallbacks();
    active = true;
}

void
Controller::onInput()
{
    int wheel = 0;
    SDL_Event event;
    ImGuiIO & io = ImGui::GetIO();

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                    case SDLK_DOWN:
                    case SDLK_RETURN:
                    case SDLK_BACKSPACE:
                        io.KeysDown[event.key.keysym.scancode] = (event.type == SDL_KEYDOWN);
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
                switch ( event.window.event ) 
                {
                    case SDL_WINDOWEVENT_CLOSE:   // exit game
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

void 
Controller::onLoad()
{
    this->viewManager.update();
}

void
Controller::doRender()
{
    SDL_RenderClear(renderer);
    int w = 1920;
    int h = 1080;
    SDL_SetRenderDrawColor(renderer, 55, 55, 55, 0);
    SDL_GetWindowSize(window, &w, &h);
    SDL_Rect f = {0, 0, 1920, 1080};
    SDL_RenderFillRect(renderer, &f);
    ImGui::Render();
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(renderer);
}

void
Controller::onExit()
{
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = nullptr;
    renderer = nullptr;
    SDL_Quit();
}