#ifndef AccountView_hpp
#define AccountView_hpp

#include "model/account/account_model.h"
#include "view/view.h"

namespace premia {
class AccountView : public View {
 public:
  std::string getName() override;
  void addLogger(const Logger &logger) override;
  void addEvent(const std::string &key, const EventHandler &event) override;
  void Update() override;

 private:
  bool isInit = false;
  EventMap events;
  Logger logger;
  AccountModel core_model;
  void DrawCoreAccountPreview();
};
}  // namespace premia

#endif
