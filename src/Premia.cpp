#define SDL_MAIN_HANDLED
#include "Controller/Controller.hpp"

int main(int argc, char *argv[]) {
  Premia::Controller controller;
  controller.onEntry();
  while (controller.isActive()) {
    controller.onInput();
    controller.onLoad();
    controller.doRender();
  }
  controller.onExit();
  return EXIT_SUCCESS;
}