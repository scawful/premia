#ifndef Model_hpp
#define Model_hpp

#include "singletons/TDA.hpp"
#include "singletons/TWS.hpp"
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
  void addEventHandler(CRString key, const EventHandler& handler);

  Logger& getLogger();
};
}  // namespace premia

#endif