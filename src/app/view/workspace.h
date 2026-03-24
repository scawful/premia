#ifndef Workspace_hpp
#define Workspace_hpp

#include <optional>
#include <string>
#include <vector>

#include "premia/core/application/screen_models.hpp"
#include "premia/core/application/workflow_models.hpp"
#include "view/account/account_view.h"
#include "view/chart/chart_view.h"
#include "view/console/console_view.h"
#include "view/menu/menu_view.h"
#include "view/options/option_chain.h"
#include "view/watchlist/watchlist_view.h"

namespace premia {

class Workspace {
 public:
  Workspace();

  void Update();

 private:
  enum class Surface {
    kOverview,
    kWatchlists,
    kChart,
    kOptions,
    kTrade,
    kAccount,
  };

  void WireEvents();
  void LoadWorkspaceState();
  void PersistWorkspaceStateIfNeeded();
  void RefreshWorkspaceData();
  void RefreshTradeQuote();
  void HandleKeyboardShortcuts();
  void OpenCommandPalette();
  void DrawCommandPalette();
  void DrawShortcutHelpOverlay();
  void ApplyLayoutPreset(const std::string& preset_id);
  void SelectSymbol(const std::string& symbol);
  void SelectOptionContract(const std::string& symbol,
                            const std::string& contract_symbol,
                            const std::string& strike,
                            bool is_call);
  void SelectOrder(const core::application::OrderRecordData& order);
  void DrawHeader();
  void DrawSidebar();
  void DrawMainSurface();
  void DrawRightRail();
  void DrawOverview();
  void DrawTradeDesk();
  void DrawBrokerageAccountsPanel();
  void DrawQuickActionsPanel();
  void DrawSelectedOrderCard();
  void DrawLayoutPanel();
  void DrawLinkedSymbolCard();
  void DrawTradingStatusCard();
  void DrawCompactAccountCard();
  void DrawQuickTradeTicket(bool expanded);
  void DrawOrdersTable(
      const char* id,
      const std::vector<core::application::OrderRecordData>& orders,
      int max_rows,
      bool selectable = false);

  auto BuildOrderIntent() const -> core::application::OrderIntentRequest;
  auto CurrentSurfaceKey() const -> std::string;
  auto ActiveSurfaceIndex() const -> int;
  auto ActiveAccountSource() const -> std::string;
  auto ActiveMarketDataSource() const -> std::string;
  auto PreferredTradingVenue() const -> std::string;
  auto IsProviderConnected(core::domain::Provider provider) const -> bool;
  auto RuntimeBasePath() const -> std::string;

  Surface active_surface_ = Surface::kOverview;
  bool data_loaded_ = false;
  bool events_wired_ = false;
  std::string workspace_message_;
  std::string last_refresh_at_;
  std::string active_account_id_;
  std::string selected_order_id_;
  std::string persisted_surface_key_;
  std::string persisted_account_id_;
  std::string persisted_symbol_;
  std::string persisted_chart_preset_;
  std::string persisted_layout_preset_;
  float persisted_sidebar_width_ = 280.0f;
  float persisted_right_rail_width_ = 360.0f;
  bool workspace_state_loaded_ = false;
  bool command_palette_open_ = false;
  bool shortcut_help_open_ = false;
  std::string command_symbol_;
  int command_surface_index_ = 0;
  int command_account_index_ = 0;
  int command_layout_index_ = 0;

  MenuView menu_view_;
  ConsoleView console_view_;
  WatchlistView watchlist_view_;
  AccountView account_view_;
  ChartView chart_view_;
  OptionChainView option_chain_view_;

  core::application::HomeScreenData home_data_;
  core::application::AccountDetail account_detail_;
  std::vector<core::application::BrokerageAccountSummary> brokerage_accounts_;
  std::vector<core::application::OrderRecordData> open_orders_;
  std::vector<core::application::OrderRecordData> order_history_;
  std::optional<core::application::QuoteDetail> trade_quote_;
  std::optional<core::application::OrderPreviewData> latest_preview_;
  std::optional<core::application::OrderSubmissionData> latest_submission_;

  std::string ticket_symbol_ = "AAPL";
  std::string selected_option_strike_;
  std::string selected_option_contract_symbol_;
  std::string ticket_quantity_ = "1";
  std::string ticket_limit_price_;
  std::string layout_preset_ = "Balanced";
  int ticket_instruction_ = 0;
  int ticket_order_type_ = 0;
  int ticket_asset_type_ = 0;
  bool selected_option_is_call_ = true;
  bool live_trade_enabled_ = false;
  float sidebar_width_ = 280.0f;
  float right_rail_width_ = 360.0f;
};

}  // namespace premia

#endif
