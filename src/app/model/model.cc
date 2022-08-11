#include "model.h"

namespace premia {

void Model::addLogger(const Logger& newLogger) { consoleLogger = newLogger; }

Logger& Model::getLogger() { return consoleLogger; }

void Model::addEventHandler(const std::string &key, const EventHandler& handler) {
  callbacks[key] = handler;
}
}  // namespace premia
