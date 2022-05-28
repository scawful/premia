#ifndef LoginView_hpp
#define LoginView_hpp

#include "Model/Model.hpp"
#include "Virtual/View.hpp"

namespace Premia {
class LoginView : public View {
 private:
  bool initialized = false;

  Logger logger;
  std::unordered_map<String, EventHandler> events;

  void drawScreen() const;

 public:
  String getName() override;
  void addLogger(const Logger& logger) override;
  void addEvent(CRString, const EventHandler&) override;
  void update() override;
};
}  // namespace Premia

#endif