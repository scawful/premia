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
  void RefreshWorkspaceData();
  void RefreshTradeQuote();
  void SelectSymbol(const std::string& symbol);
  void DrawHeader();
  void DrawSidebar();
  void DrawMainSurface();
  void DrawRightRail();
  void DrawOverview();
  void DrawTradeDesk();
  void DrawLinkedSymbolCard();
  void DrawTradingStatusCard();
  void DrawCompactAccountCard();
  void DrawQuickTradeTicket(bool expanded);
  void DrawOrdersTable(
      const char* id,
      const std::vector<core::application::OrderRecordData>& orders,
      int max_rows) const;

  auto BuildOrderIntent() const -> core::application::OrderIntentRequest;
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

  MenuView menu_view_;
  ConsoleView console_view_;
  WatchlistView watchlist_view_;
  AccountView account_view_;
  ChartView chart_view_;
  OptionChainView option_chain_view_;

  core::application::HomeScreenData home_data_;
  core::application::AccountDetail account_detail_;
  std::vector<core::application::OrderRecordData> open_orders_;
  std::vector<core::application::OrderRecordData> order_history_;
  std::optional<core::application::QuoteDetail> trade_quote_;
  std::optional<core::application::OrderPreviewData> latest_preview_;
  std::optional<core::application::OrderSubmissionData> latest_submission_;

  std::string ticket_symbol_ = "AAPL";
  std::string ticket_quantity_ = "1";
  std::string ticket_limit_price_;
  int ticket_instruction_ = 0;
  int ticket_order_type_ = 0;
  int ticket_asset_type_ = 0;
};

}  // namespace premia

#endif
