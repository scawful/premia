#ifndef PrimaryView_hpp
#define PrimaryView_hpp

#include "View/Menu/MenuView.hpp"
#include "Virtual/View.hpp"

namespace premia {
class PrimaryView : public View {
 private:
  Logger logger;
  std::shared_ptr<View> menuView = std::make_shared<MenuView>();
  std::unordered_map<String, EventHandler> events;

  void drawInfoPane();
  void drawScreen();

 public:
  String getName() override;
  void addLogger(const Logger& logger) override;
  void addEvent(CRString key, const EventHandler& event) override;
  void update() override;
};
}  // namespace premia

#endif