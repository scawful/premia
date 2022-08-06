#include "core/controller.h"

int main(int argc, char *argv[]) {
  premia::Controller controller;
  if (controller.onEntry().ok()) {
    return EXIT_FAILURE;
  }
  while (controller.isActive()) {
    controller.onInput();
    controller.onLoad();
    controller.doRender();
  }
  controller.onExit();
  return EXIT_SUCCESS;
}