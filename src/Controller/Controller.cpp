#include "Controller.hpp"

/**
 * @brief Initializes the SDL_Window and SDL_Renderer
 *        Starts the ImGui and ImPlot contexts
 *        Loads default fonts and theme 
 *        Builds the GUI Frame 
 * 
 */
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
            SDL_WINDOW_RESIZABLE  |             // flags
            SDL_WINDOW_BORDERLESS 
        );
    }   

    if (window == nullptr) {
        SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        throw Premia::FatalException();
    } else {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (renderer == nullptr) {
            SDL_Log("SDL_CreateRenderer: %s\n", SDL_GetError());
            SDL_Quit();
            throw Premia::FatalException();
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

/**
 * @brief Initializes any events the ViewManager may need for modifying the Controller
 * 
 */
void 
Controller::initEvents()
{
    viewManager.addEventHandler("login", [this] () -> void { 
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
    
    viewManager.addEventHandler("goHome", [this] () -> void { viewManager.setCurrentView(std::make_shared<PrimaryView>()); });
    viewManager.addEventHandler("chartView", [this] () -> void { viewManager.setCurrentView(std::make_shared<ChartView>()); });
    viewManager.addEventHandler("linePlotView", [this] () -> void { viewManager.setCurrentView(std::make_shared<LinePlotChart>()); });
    viewManager.addEventHandler("optionChainView", [this] () -> void { viewManager.setCurrentView(std::make_shared<OptionChainView>()); });
    viewManager.addEventHandler("quit", [this] () -> void { this->quit(); });
}

/**
 * @brief Returns program runtime 
 * 
 * @return true 
 * @return false 
 */
bool 
Controller::isActive() const { 
    return active; 
}

/**
 * @brief Entry point of application
 * 
 */
void
Controller::onEntry() noexcept(false) {
    initWindow();
    initEvents();
    active = true;
}

/**
 * @brief Handles keyboard and mouse input via SDL_Events 
 * 
 */
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
                switch ( event.window.event )  {
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
void 
Controller::onLoad() const {
    this->viewManager.update();
}

/**
 * @brief Render current contents to the screen
 * 
 */
void
Controller::doRender()
{
    SDL_RenderClear(renderer);
    ImGui::Render();
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(renderer);
}

/**
 * @brief Cleanup all resources 
 * 
 */
void
Controller::onExit()
{
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