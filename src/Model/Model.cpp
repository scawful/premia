#include "Model.hpp"

namespace premia {

void Model::addLogger(const Logger& newLogger) { consoleLogger = newLogger; }

Logger& Model::getLogger() { return consoleLogger; }

void Model::addEventHandler(CRString key, const EventHandler& handler) {
  callbacks[key] = handler;
}
}  // namespace premia
