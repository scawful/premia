#ifndef Model_hpp
#define Model_hpp

#include "core/TDA.hpp"
#include "core/TWS.hpp"
#include "premia.h"

namespace premia {
class Model {
 private:
  EventMap callbacks;
  Logger consoleLogger;

 public:
  Model() = default;
  ~Model() = default;

  void addLogger(const Logger& logger);
  void addEventHandler(const std::string &key, const EventHandler& handler);

  Logger& getLogger();
};
}  // namespace premia

#endif