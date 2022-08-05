#ifndef PrimaryView_hpp
#define PrimaryView_hpp

#include "view/menu/menu_view.h"
#include "view/view.h"

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