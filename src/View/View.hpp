#ifndef View_hpp
#define View_hpp

#include "Premia.hpp"
#include "EventInterface.hpp"
#include <SDL2/SDL.h>
#include "../libs/imgui/imgui.h"
#include "../libs/imgui/backends/imgui_impl_sdl.h"
#include "../libs/imgui/backends/imgui_impl_sdlrenderer.h"
#include "../libs/imgui/misc/cpp/imgui_stdlib.h"
#include "../libs/implot/implot.h"
#include "../libs/implot/implot_internal.h"

class View 
{
public:
    View()=default;
    virtual ~View()=default;

    virtual void addAuth(String, String) = 0;
    virtual void addEvent(String, const Premia::EventHandler &) = 0;
    virtual void addLogger(const Premia::ConsoleLogger &) = 0;
    virtual void update() = 0;
};

#endif