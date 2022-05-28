#define SDL_MAIN_HANDLED
#include "Controller/Controller.hpp"

int main(int argc, char *argv[]) {
  Premia::Controller controller;
  Try {
    controller.onEntry();
    while (controller.isActive()) {
      controller.onInput();
      controller.onLoad();
      controller.doRender();
    }
  } catch (const Premia::FatalException &e) {
    std::cout << e.what() << std::endl;
  } finally { 
    controller.onExit(); 
  } Proceed;
  return EXIT_SUCCESS;
}