#ifndef SDLInclude_hpp
#define SDLInclude_hpp

#include <SDL2/SDL.h>

using SDLWindow = SDL_Window*;
using SDLPoint = const SDL_Point*;

constexpr size_t TITLE_BORDER = 20;
constexpr size_t RESIZE_BORDER = 2;

static SDL_HitTestResult WindowCallback(SDLWindow win, SDLPoint point, void* data);

/**
 * @brief Handles dragging the window by the title bar
 *        and resizing the window at its edges and corners
 * 
 * @param win 
 * @param pt 
 * @param data 
 * @return SDL_HitTestResult 
 */
static SDL_HitTestResult 
WindowCallback(SDLWindow win, SDLPoint pt, void* data)
{
    int w;
    int h;
    SDL_GetWindowSize(win, &w, &h);

    // dragging this pixel moves the window.
    if (pt->y < TITLE_BORDER && pt->x < (w - TITLE_BORDER)) {
        return SDL_HITTEST_DRAGGABLE;
    }

    #define RESIZE_HIT(name) { \
        return SDL_HITTEST_RESIZE_##name; \
    }
    
    // SDL_HITTEST_RESIZE_* - dragging this pixel resizes a specific edge (or edges) of the window. (Here * is one of: TOPLEFT, TOP, TOPRIGHT, RIGHT, BOTTOMRIGHT, BOTTOM, BOTTOMLEFT, LEFT).
    if (pt->x < RESIZE_BORDER && pt->y < RESIZE_BORDER) {
        RESIZE_HIT(TOPLEFT)
    } else if (pt->x > RESIZE_BORDER && pt->x < w - RESIZE_BORDER && pt->y < RESIZE_BORDER) {
        RESIZE_HIT(TOP)
    } else if (pt->x > w - RESIZE_BORDER && pt->y < RESIZE_BORDER) {
        RESIZE_HIT(TOPRIGHT)
    } else if (pt->x > w - RESIZE_BORDER && pt->y > RESIZE_BORDER && pt->y < h - RESIZE_BORDER) {
        RESIZE_HIT(RIGHT)
    } else if (pt->x > w - RESIZE_BORDER && pt->y > h - RESIZE_BORDER) {
        RESIZE_HIT(BOTTOMRIGHT)
    } else if (pt->x < w - RESIZE_BORDER && pt->x > RESIZE_BORDER && pt->y > h - RESIZE_BORDER) {
        RESIZE_HIT(BOTTOM)
    } else if (pt->x < RESIZE_BORDER && pt->y > h - RESIZE_BORDER) {
        RESIZE_HIT(BOTTOMLEFT)
    } else if (pt->x < RESIZE_BORDER && pt->y < h - RESIZE_BORDER && pt->y > RESIZE_BORDER) {
        RESIZE_HIT(LEFT)
    }

    // no action.
    return SDL_HITTEST_NORMAL;
}

#endif