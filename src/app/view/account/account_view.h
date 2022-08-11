#ifndef AccountView_hpp
#define AccountView_hpp

#include "core/HLXT.hpp"
#include "model/options/options_model.h"
#include "view/view.h"

namespace premia {
class AccountView : public View {
 private:
  bool isInit = false;
  bool isLoggedIn = false;
  EventMap events;
  Logger logger;
  // -----------------------------
  std::vector<std::string> account_ids_std;
  std::vector<const char*> account_ids;
  std::string default_account;

  std::unordered_map<std::string, tda::Quote> quotes;

  tda::Account account_data;
  std::vector<std::string> positions_vector;

  void initPositions();
  void load_account(const std::string &account);
  void load_all_accounts();
  void Draw_symbol_string(const std::string &symbol);
  void Draw_balance_string(const std::string &variable);
  void Draw_positions();

  // ------------------------------
  void DrawAccountPane();

 public:
  std::string getName() override;
  void addLogger(const Logger& logger) override;
  void addEvent(const std::string &key, const EventHandler& event) override;
  void Update() override;

 private:
  ImGuiTableFlags positionFlags =
      ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable |
      ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
      ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
      ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
};
}  // namespace premia

#endif