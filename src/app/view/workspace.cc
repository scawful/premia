#include "workspace.h"

#include <SDL.h>
#include <imgui/imgui.h>

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

#include "premia/core/application/composition_root.hpp"

namespace premia {

namespace {

using core::application::ConnectionSummary;
using core::application::HoldingRow;
using core::application::Money;
using core::domain::ConnectionStatus;
using core::domain::Provider;

constexpr ImGuiWindowFlags kMainEditorFlags =
    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus |
    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove;

constexpr float kSidebarWidth = 280.0f;
constexpr float kRightRailWidth = 360.0f;

auto AccentColor() -> ImVec4 { return ImVec4(0.83f, 0.69f, 0.28f, 1.0f); }

auto PositiveColor() -> ImVec4 { return ImVec4(0.24f, 0.78f, 0.55f, 1.0f); }

auto NegativeColor() -> ImVec4 { return ImVec4(0.89f, 0.34f, 0.36f, 1.0f); }

auto MutedColor() -> ImVec4 { return ImVec4(0.58f, 0.63f, 0.71f, 1.0f); }

auto ProviderColor(Provider provider) -> ImVec4 {
  switch (provider) {
    case Provider::kSchwab:
      return ImVec4(0.34f, 0.69f, 0.98f, 1.0f);
    case Provider::kIBKR:
      return AccentColor();
    case Provider::kPlaid:
      return ImVec4(0.32f, 0.75f, 0.64f, 1.0f);
    case Provider::kTDA:
      return ImVec4(0.47f, 0.70f, 0.90f, 1.0f);
    case Provider::kInternal:
      return MutedColor();
  }
  return MutedColor();
}

auto StatusColor(ConnectionStatus status) -> ImVec4 {
  switch (status) {
    case ConnectionStatus::kConnected:
      return PositiveColor();
    case ConnectionStatus::kConnecting:
      return AccentColor();
    case ConnectionStatus::kDegraded:
      return ImVec4(0.93f, 0.59f, 0.26f, 1.0f);
    case ConnectionStatus::kReauthRequired:
      return NegativeColor();
    case ConnectionStatus::kNotConnected:
      return MutedColor();
  }
  return MutedColor();
}

auto PrettyStatus(ConnectionStatus status) -> const char* {
  switch (status) {
    case ConnectionStatus::kConnected:
      return "Connected";
    case ConnectionStatus::kConnecting:
      return "Connecting";
    case ConnectionStatus::kNotConnected:
      return "Not connected";
    case ConnectionStatus::kDegraded:
      return "Degraded";
    case ConnectionStatus::kReauthRequired:
      return "Needs reauth";
  }
  return "Unknown";
}

auto FormatMoney(const Money& money) -> std::string {
  if (money.amount.empty()) {
    return "--";
  }
  return "$" + money.amount;
}

auto ParseSignedAmount(const std::string& amount) -> double {
  try {
    return std::stod(amount);
  } catch (...) {
    return 0.0;
  }
}

auto ChangeColor(const std::string& amount) -> ImVec4 {
  return ParseSignedAmount(amount) < 0.0 ? NegativeColor() : PositiveColor();
}

void DrawMetricCard(const char* id, const std::string& label,
                    const std::string& value, const std::string& note,
                    const ImVec4& value_color) {
  ImGui::BeginChild(id, ImVec2(0.0f, 92.0f), true);
  ImGui::TextDisabled("%s", label.c_str());
  ImGui::PushStyleColor(ImGuiCol_Text, value_color);
  ImGui::Text("%s", value.c_str());
  ImGui::PopStyleColor();
  ImGui::TextWrapped("%s", note.c_str());
  ImGui::EndChild();
}

auto CurrentTimeLabel() -> std::string {
  const auto now = std::chrono::system_clock::now();
  const std::time_t now_time = std::chrono::system_clock::to_time_t(now);
  std::tm local_time{};
#if defined(_WIN32)
  localtime_s(&local_time, &now_time);
#else
  localtime_r(&now_time, &local_time);
#endif
  std::ostringstream oss;
  oss << std::put_time(&local_time, "%H:%M:%S");
  return oss.str();
}

void RequestQuit() {
  SDL_Event event{};
  event.type = SDL_QUIT;
  SDL_PushEvent(&event);
}

void ToggleFullscreen() {
  SDL_Window* window = SDL_GetKeyboardFocus();
  if (window == nullptr) {
    window = SDL_GetMouseFocus();
  }
  if (window == nullptr) {
    return;
  }
  const Uint32 flags = SDL_GetWindowFlags(window);
  const bool is_fullscreen = (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0;
  SDL_SetWindowFullscreen(window, is_fullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void NewMasterFrame() {
  const ImGuiIO& io = ImGui::GetIO();
  ImGui::NewFrame();
  ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
  ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);

  if (!ImGui::Begin("##PremiaWorkspace", nullptr, kMainEditorFlags)) {
    ImGui::End();
  }
}

}  // namespace

Workspace::Workspace() { WireEvents(); }

void Workspace::WireEvents() {
  if (events_wired_) {
    return;
  }

  menu_view_.addEvent("consoleView", [this]() { console_view_.Update(); });
  menu_view_.addEvent("goHome", [this]() { active_surface_ = Surface::kOverview; });
  menu_view_.addEvent("watchlistView",
                      [this]() { active_surface_ = Surface::kWatchlists; });
  menu_view_.addEvent("chartView", [this]() { active_surface_ = Surface::kChart; });
  menu_view_.addEvent("linePlotView", [this]() { active_surface_ = Surface::kChart; });
  menu_view_.addEvent("optionChainView",
                      [this]() { active_surface_ = Surface::kOptions; });
  menu_view_.addEvent("optionChainLeftCol",
                      [this]() { active_surface_ = Surface::kOptions; });
  menu_view_.addEvent("optionChainRightCol",
                      [this]() { active_surface_ = Surface::kOptions; });
  menu_view_.addEvent("tradeDeskView", [this]() { active_surface_ = Surface::kTrade; });
  menu_view_.addEvent("accountView", [this]() { active_surface_ = Surface::kAccount; });
  menu_view_.addEvent("moversUpView", [this]() { active_surface_ = Surface::kChart; });
  menu_view_.addEvent("moversDownView",
                      [this]() { active_surface_ = Surface::kChart; });
  menu_view_.addEvent("toggleFullscreenMode", []() { ToggleFullscreen(); });
  menu_view_.addEvent("quit", []() { RequestQuit(); });

  watchlist_view_.SetSymbolSelectionHandler([this](const std::string& symbol) {
    SelectSymbol(symbol);
    workspace_message_ =
        "Linked symbol from watchlists into charts, options, and trade.";
  });
  account_view_.SetSymbolSelectionHandler([this](const std::string& symbol) {
    SelectSymbol(symbol);
    active_surface_ = Surface::kTrade;
    workspace_message_ =
        "Linked account holding into charts, options, and the trade desk.";
  });
  chart_view_.SetSymbolChangeHandler([this](const std::string& symbol) {
    SelectSymbol(symbol);
    workspace_message_ = "Linked chart symbol into options and trade.";
  });
  option_chain_view_.SetSymbolChangeHandler([this](const std::string& symbol) {
    SelectSymbol(symbol);
    workspace_message_ = "Linked options symbol into charts and trade.";
  });
  option_chain_view_.SetStrikeSelectionHandler(
      [this](const std::string& symbol, const std::string& contract_symbol,
             const std::string& strike, bool is_call) {
        if (contract_symbol.empty()) {
          SelectSymbol(symbol);
          selected_option_strike_ = strike;
          active_surface_ = Surface::kTrade;
          workspace_message_ =
              "Linked option strike into the trade desk for review.";
          return;
        }
        SelectOptionContract(symbol, contract_symbol, strike, is_call);
      });

  events_wired_ = true;
}

void Workspace::SelectSymbol(const std::string& symbol) {
  if (symbol.empty()) {
    return;
  }
  if (ticket_symbol_ != symbol) {
    ticket_limit_price_.clear();
    latest_preview_.reset();
    selected_option_strike_.clear();
    selected_option_contract_symbol_.clear();
  }
  ticket_symbol_ = symbol;
  account_view_.SetSelectedSymbol(symbol);
  watchlist_view_.SetSelectedSymbol(symbol);
  chart_view_.SetTickerSymbol(symbol);
  option_chain_view_.SetSymbol(symbol);
  RefreshTradeQuote();
}

void Workspace::SelectOptionContract(const std::string& symbol,
                                     const std::string& contract_symbol,
                                     const std::string& strike,
                                     bool is_call) {
  SelectSymbol(symbol);
  selected_option_strike_ = strike;
  selected_option_contract_symbol_ = contract_symbol;
  selected_option_is_call_ = is_call;
  ticket_asset_type_ = 1;
  ticket_instruction_ = 0;
  ticket_quantity_ = "1";
  active_surface_ = Surface::kTrade;
  workspace_message_ = std::string("Loaded ") +
                       (is_call ? "call" : "put") +
                       " contract into the trade desk.";
}

void Workspace::SelectOrder(const core::application::OrderRecordData& order) {
  selected_order_id_ = order.order_id;
  ticket_quantity_ = order.quantity;
  ticket_limit_price_ = order.limit_price;
  ticket_order_type_ = order.order_type == "MARKET" ? 1 : 0;
  ticket_asset_type_ = order.asset_type == "OPTION" ? 1 : 0;
  if (ticket_asset_type_ == 1) {
    selected_option_contract_symbol_ = order.symbol;
  }
  ticket_instruction_ = (order.instruction == "SELL" ||
                         order.instruction == "SELL_TO_CLOSE")
                            ? 1
                            : 0;
  SelectSymbol(ticket_asset_type_ == 1 ? ticket_symbol_ : order.symbol);
  if (ticket_asset_type_ == 1) {
    selected_option_contract_symbol_ = order.symbol;
  }
  workspace_message_ = "Loaded the selected order into the trade ticket.";
}

void Workspace::RefreshTradeQuote() {
  try {
    const auto symbol = ticket_symbol_.empty() ? std::string("AAPL") : ticket_symbol_;
    trade_quote_ = core::application::CompositionRoot::Instance()
                       .MarketData()
                       .GetQuoteDetail(symbol);
    if (ticket_order_type_ == 0 && ticket_limit_price_.empty()) {
      ticket_limit_price_ = trade_quote_->quote.last_price.amount;
    }
  } catch (const std::exception& ex) {
    trade_quote_.reset();
    workspace_message_ = std::string("Quote refresh failed: ") + ex.what();
  }
}

void Workspace::RefreshWorkspaceData() {
  try {
    auto& root = core::application::CompositionRoot::Instance();
    brokerage_accounts_ = root.AppService().ListBrokerageAccounts();
    if (!active_account_id_.empty()) {
      const auto account_it = std::find_if(
          brokerage_accounts_.begin(), brokerage_accounts_.end(),
          [this](const core::application::BrokerageAccountSummary& account) {
            return account.account_id == active_account_id_;
          });
      if (account_it == brokerage_accounts_.end()) {
        active_account_id_.clear();
      }
    }
    home_data_ = root.AppService().GetHomeScreenDataForAccount(active_account_id_);
    account_detail_ = root.AppService().GetAccountDetailForAccount(active_account_id_);
    if (active_account_id_.empty()) {
      active_account_id_ = account_detail_.account_id;
    }
    open_orders_ = root.Orders().GetOpenOrders(active_account_id_);
    order_history_ = root.Orders().GetOrderHistory(active_account_id_);
    if (!home_data_.top_holdings.empty()) {
      ticket_symbol_ = home_data_.top_holdings.front().symbol;
    } else if (!account_detail_.positions.empty()) {
      ticket_symbol_ = account_detail_.positions.front().symbol;
    }
    if (ticket_symbol_.empty()) {
      ticket_symbol_ = "AAPL";
    }
    SelectSymbol(ticket_symbol_);
    data_loaded_ = true;
    last_refresh_at_ = CurrentTimeLabel();
    workspace_message_ = "Workspace synced with provider-backed data.";
  } catch (const std::exception& ex) {
    data_loaded_ = false;
    workspace_message_ = std::string("Workspace refresh failed: ") + ex.what();
  }
}

auto Workspace::BuildOrderIntent() const -> core::application::OrderIntentRequest {
  core::application::OrderIntentRequest request;
  request.account_id = active_account_id_.empty() ? (account_detail_.account_id.empty()
                                                         ? "local_acc"
                                                         : account_detail_.account_id)
                                                  : active_account_id_;
  request.asset_type = ticket_asset_type_ == 0 ? "EQUITY" : "OPTION";
  request.symbol = request.asset_type == "OPTION" &&
                           !selected_option_contract_symbol_.empty()
                       ? selected_option_contract_symbol_
                       : (ticket_symbol_.empty() ? "AAPL" : ticket_symbol_);
  request.instruction = request.asset_type == "OPTION"
                            ? (ticket_instruction_ == 0 ? "BUY_TO_OPEN"
                                                        : "SELL_TO_CLOSE")
                            : (ticket_instruction_ == 0 ? "BUY" : "SELL");
  request.quantity = ticket_quantity_.empty() ? "1" : ticket_quantity_;
  request.order_type = ticket_order_type_ == 0 ? "LIMIT" : "MARKET";
  request.limit_price = ticket_order_type_ == 0 ? ticket_limit_price_ : "";
  request.duration = "DAY";
  request.session = "NORMAL";
  request.confirm_live = live_trade_enabled_ &&
                         IsProviderConnected(Provider::kSchwab) &&
                         (request.asset_type == "EQUITY" ||
                          !selected_option_contract_symbol_.empty());
  return request;
}

auto Workspace::ActiveAccountSource() const -> std::string {
  for (const auto& account : brokerage_accounts_) {
    if (!active_account_id_.empty() && account.account_id == active_account_id_) {
      return account.display_name;
    }
  }
  for (const auto& connection : home_data_.connections) {
    if (connection.provider == Provider::kSchwab &&
        connection.status == ConnectionStatus::kConnected) {
      return connection.display_name;
    }
  }
  for (const auto& connection : home_data_.connections) {
    if (connection.provider == Provider::kIBKR &&
        connection.status == ConnectionStatus::kConnected) {
      return connection.display_name;
    }
  }
  if (!account_detail_.account_id.empty() && account_detail_.account_id.rfind("U", 0) == 0) {
    return "Interactive Brokers";
  }
  return "Local Preview";
}

auto Workspace::ActiveMarketDataSource() const -> std::string {
  for (const auto& connection : home_data_.connections) {
    if (connection.provider == Provider::kSchwab &&
        connection.status == ConnectionStatus::kConnected) {
      return connection.display_name;
    }
  }
  return "Local Preview";
}

auto Workspace::PreferredTradingVenue() const -> std::string {
  if (IsProviderConnected(Provider::kSchwab)) {
    return "Charles Schwab desk (live quotes, simulated execution)";
  }
  if (IsProviderConnected(Provider::kIBKR)) {
    return "Interactive Brokers account desk (simulation only)";
  }
  return "Local simulation";
}

auto Workspace::IsProviderConnected(Provider provider) const -> bool {
  return std::any_of(home_data_.connections.begin(), home_data_.connections.end(),
                     [provider](const ConnectionSummary& connection) {
                       return connection.provider == provider &&
                              connection.status == ConnectionStatus::kConnected;
                     });
}

auto Workspace::RuntimeBasePath() const -> std::string {
  if (char* base = SDL_GetBasePath()) {
    std::string base_path(base);
    SDL_free(base);
    return base_path;
  }
  return "unknown";
}

void Workspace::DrawHeader() {
  ImGui::BeginChild("WorkspaceHeader", ImVec2(0.0f, 94.0f), true);

  ImGui::Text("Premia Desktop");
  ImGui::TextDisabled(
      "Schwab and IBKR-aware workspace for charts, account monitoring, and trading flows.");

  if (ImGui::Button("Refresh Workspace", ImVec2(170.0f, 0.0f))) {
    RefreshWorkspaceData();
  }
  ImGui::SameLine();
  if (!brokerage_accounts_.empty()) {
    std::vector<const char*> account_names;
    int active_index = 0;
    account_names.reserve(brokerage_accounts_.size());
    for (int index = 0; index < static_cast<int>(brokerage_accounts_.size()); ++index) {
      account_names.push_back(brokerage_accounts_[index].display_name.c_str());
      if (brokerage_accounts_[index].account_id == active_account_id_) {
        active_index = index;
      }
    }
    ImGui::SetNextItemWidth(220.0f);
    if (ImGui::Combo("##activeAccount", &active_index, account_names.data(),
                     static_cast<int>(account_names.size()))) {
      active_account_id_ = brokerage_accounts_[active_index].account_id;
      RefreshWorkspaceData();
    }
    ImGui::SameLine();
  }
  ImGui::TextDisabled("Last refresh: %s", last_refresh_at_.empty() ? "not yet"
                                                                    : last_refresh_at_.c_str());

  ImGui::TextWrapped("%s", workspace_message_.c_str());
  ImGui::SameLine();
  ImGui::TextDisabled("Focused symbol: %s", ticket_symbol_.c_str());
  ImGui::TextDisabled("Binary base path: %s", RuntimeBasePath().c_str());

  ImGui::EndChild();
}

void Workspace::DrawSidebar() {
  ImGui::BeginChild("WorkspaceSidebar", ImVec2(0.0f, 0.0f), true);
  ImGui::Text("Broker Focus");
  ImGui::TextColored(AccentColor(), "%s", ActiveAccountSource().c_str());
  ImGui::TextDisabled("Account ID: %s",
                      account_detail_.account_id.empty() ? "-"
                                                         : account_detail_.account_id.c_str());
  if (brokerage_accounts_.size() > 1) {
    ImGui::TextDisabled("Linked Schwab accounts: %d",
                        static_cast<int>(brokerage_accounts_.size()));
  }
  ImGui::Separator();

  struct NavItem {
    const char* label;
    Surface surface;
  };

  const NavItem nav_items[] = {
      {"Overview", Surface::kOverview},
      {"Watchlists", Surface::kWatchlists},
      {"Charts", Surface::kChart},
      {"Options", Surface::kOptions},
      {"Trade Desk", Surface::kTrade},
      {"Account", Surface::kAccount},
  };

  for (const auto& item : nav_items) {
    const bool selected = active_surface_ == item.surface;
    if (ImGui::Selectable(item.label, selected, 0, ImVec2(-FLT_MIN, 0.0f))) {
      active_surface_ = item.surface;
    }
  }

  ImGui::Separator();
  ImGui::Text("Focused Symbol");
  ImGui::TextColored(ProviderColor(Provider::kSchwab), "%s", ticket_symbol_.c_str());
  if (ImGui::Button("Open Chart", ImVec2(-FLT_MIN, 0.0f))) {
    active_surface_ = Surface::kChart;
  }
  if (ImGui::Button("Open Options", ImVec2(-FLT_MIN, 0.0f))) {
    active_surface_ = Surface::kOptions;
  }
  if (ImGui::Button("Open Trade Desk", ImVec2(-FLT_MIN, 0.0f))) {
    active_surface_ = Surface::kTrade;
  }

  ImGui::Separator();
  ImGui::Text("Connections");
  for (const auto& connection : home_data_.connections) {
    ImGui::PushStyleColor(ImGuiCol_Text, ProviderColor(connection.provider));
    ImGui::Text("%s", connection.display_name.c_str());
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::TextColored(StatusColor(connection.status), "%s",
                       PrettyStatus(connection.status));
    if (!connection.capabilities.empty()) {
      std::string capability_line;
      for (const auto& capability : connection.capabilities) {
        if (!capability.second) {
          continue;
        }
        if (!capability_line.empty()) {
          capability_line += " - ";
        }
        capability_line += capability.first;
      }
      if (capability_line.empty()) {
        capability_line = "No active capabilities";
      }
      ImGui::TextDisabled("%s", capability_line.c_str());
    }
    ImGui::Spacing();
  }

  ImGui::Separator();
  ImGui::Text("Launch Check");
  ImGui::TextWrapped(
      "SketchyBar currently points at build-arch-next/bin/premia. This panel shows the active binary base path so stale launches are easier to spot.");
  ImGui::EndChild();
}

void Workspace::DrawOverview() {
  ImGui::Text("Overview");
  ImGui::TextDisabled("Current account source: %s", ActiveAccountSource().c_str());
  ImGui::Separator();

  if (ImGui::BeginTable("OverviewMetrics", 4,
                        ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInnerV)) {
    ImGui::TableNextColumn();
    DrawMetricCard("PortfolioValueCard", "Portfolio Value",
                   FormatMoney(home_data_.portfolio.total_value),
                   "Top-line account value from the current brokerage source.",
                   AccentColor());
    ImGui::TableNextColumn();
    DrawMetricCard("CashCard", "Cash", FormatMoney(home_data_.portfolio.cash),
                   "Immediately available cash balance.", PositiveColor());
    ImGui::TableNextColumn();
    DrawMetricCard("BuyingPowerCard", "Buying Power",
                   FormatMoney(home_data_.portfolio.buying_power),
                   "Buying power currently surfaced through the shared account service.",
                   ProviderColor(Provider::kSchwab));
    ImGui::TableNextColumn();
    DrawMetricCard("HoldingsCountCard", "Holdings",
                   std::to_string(home_data_.portfolio.holdings_count),
                   "Positions currently visible in the provider-backed account snapshot.",
                   ProviderColor(Provider::kIBKR));
    ImGui::EndTable();
  }

  ImGui::Spacing();

  if (ImGui::BeginTable("OverviewMain", 2,
                        ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInnerV)) {
    ImGui::TableNextColumn();
    ImGui::BeginChild("TopHoldingsPanel", ImVec2(0.0f, 0.0f), true);
    ImGui::Text("Top Holdings");
    if (home_data_.top_holdings.empty()) {
      ImGui::TextDisabled("No holdings are visible in the current account snapshot.");
    } else if (ImGui::BeginTable("OverviewHoldings", 4,
                                 ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                     ImGuiTableFlags_SizingStretchSame)) {
      ImGui::TableSetupColumn("Symbol");
      ImGui::TableSetupColumn("Quantity");
      ImGui::TableSetupColumn("Value");
      ImGui::TableSetupColumn("Day Change");
      ImGui::TableHeadersRow();
      for (const auto& holding : home_data_.top_holdings) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        if (ImGui::Selectable(holding.symbol.c_str(), ticket_symbol_ == holding.symbol,
                              ImGuiSelectableFlags_SpanAllColumns)) {
          SelectSymbol(holding.symbol);
          workspace_message_ =
              "Selected a top holding and linked it across the workspace.";
        }
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%s", holding.quantity.c_str());
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%s", FormatMoney(holding.market_value).c_str());
        ImGui::TableSetColumnIndex(3);
        ImGui::TextColored(ChangeColor(holding.day_change.absolute.amount), "%s (%s%%)",
                           FormatMoney(holding.day_change.absolute).c_str(),
                           holding.day_change.percent.c_str());
      }
      ImGui::EndTable();
    }
    ImGui::EndChild();

    ImGui::TableNextColumn();
    ImGui::BeginChild("OverviewWatchlistsPanel", ImVec2(0.0f, 0.0f), true);
    ImGui::Text("Watchlists and Market Context");
    ImGui::TextDisabled("Market session: %s", home_data_.market.session.c_str());
    ImGui::TextDisabled("Next transition: %s",
                        home_data_.market.next_transition_at.empty()
                            ? "-"
                            : home_data_.market.next_transition_at.c_str());
    ImGui::Separator();
    if (home_data_.watchlists.empty()) {
      ImGui::TextDisabled("No normalized watchlists are currently available.");
    } else {
      for (const auto& watchlist : home_data_.watchlists) {
        ImGui::BulletText("%s (%d symbols)", watchlist.name.c_str(),
                          watchlist.instrument_count);
      }
    }
    ImGui::Separator();
    ImGui::TextWrapped(
        "Market data and options currently prefer Schwab when connected. Account snapshots can fall through to IBKR, and desktop trading now runs in preview mode by default with an explicit live Schwab gate.");
    ImGui::EndChild();
    ImGui::EndTable();
  }
}

void Workspace::DrawLinkedSymbolCard() {
  ImGui::BeginChild("LinkedSymbolCard", ImVec2(0.0f, 156.0f), true);
  ImGui::Text("Linked Symbol");
  ImGui::TextColored(AccentColor(), "%s", ticket_symbol_.c_str());
  ImGui::TextDisabled("Quotes: %s", ActiveMarketDataSource().c_str());
  if (trade_quote_.has_value()) {
    ImGui::TextWrapped("%s", trade_quote_->instrument.name.c_str());
    ImGui::Text("Last: %s", FormatMoney(trade_quote_->quote.last_price).c_str());
    ImGui::Text("Bid / Ask: %s / %s", FormatMoney(trade_quote_->quote.bid).c_str(),
                FormatMoney(trade_quote_->quote.ask).c_str());
  } else {
    ImGui::TextDisabled("Quote unavailable.");
  }
  if (!selected_option_strike_.empty()) {
    ImGui::TextDisabled("Selected strike: %s", selected_option_strike_.c_str());
  }
  if (ImGui::Button("Chart", ImVec2(100.0f, 0.0f))) {
    active_surface_ = Surface::kChart;
  }
  ImGui::SameLine();
  if (ImGui::Button("Options", ImVec2(100.0f, 0.0f))) {
    active_surface_ = Surface::kOptions;
  }
  ImGui::SameLine();
  if (ImGui::Button("Trade", ImVec2(100.0f, 0.0f))) {
    active_surface_ = Surface::kTrade;
  }
  ImGui::EndChild();
}

void Workspace::DrawTradingStatusCard() {
  ImGui::BeginChild("TradingStatusCard", ImVec2(0.0f, 132.0f), true);
  ImGui::Text("Trading Status");
  ImGui::Text("Account Source: %s", ActiveAccountSource().c_str());
  ImGui::Text("Quote Source: %s", ActiveMarketDataSource().c_str());
  ImGui::TextWrapped("Route Target: %s", PreferredTradingVenue().c_str());
  ImGui::TextDisabled("Live routing armed: %s", live_trade_enabled_ ? "yes" : "no");
  ImGui::TextColored(AccentColor(),
                     "Preview is the default path; live Schwab submit requires the explicit checkbox.");
  ImGui::EndChild();
}

void Workspace::DrawOrdersTable(
    const char* id, const std::vector<core::application::OrderRecordData>& orders,
    int max_rows, bool selectable) {
  if (orders.empty()) {
    ImGui::TextDisabled("No orders available.");
    return;
  }

  if (ImGui::BeginTable(id, 5, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                  ImGuiTableFlags_SizingStretchSame)) {
    ImGui::TableSetupColumn("Symbol");
    ImGui::TableSetupColumn("Side");
    ImGui::TableSetupColumn("Qty");
    ImGui::TableSetupColumn("Status");
    ImGui::TableSetupColumn("Mode");
    ImGui::TableHeadersRow();

    const int row_count = std::min<int>(max_rows, static_cast<int>(orders.size()));
    for (int index = 0; index < row_count; ++index) {
      const auto& order = orders[index];
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      if (selectable) {
        const bool is_selected = selected_order_id_ == order.order_id;
        if (ImGui::Selectable(order.symbol.c_str(), is_selected,
                              ImGuiSelectableFlags_SpanAllColumns)) {
          SelectOrder(order);
        }
      } else {
        ImGui::Text("%s", order.symbol.c_str());
      }
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%s", order.instruction.c_str());
      ImGui::TableSetColumnIndex(2);
      ImGui::Text("%s", order.quantity.c_str());
      ImGui::TableSetColumnIndex(3);
      ImGui::Text("%s", order.status.c_str());
      ImGui::TableSetColumnIndex(4);
      ImGui::Text("%s", order.mode.c_str());
    }
    ImGui::EndTable();
  }
}

void Workspace::DrawQuickTradeTicket(bool expanded) {
  ImGui::Text(expanded ? "Trade Ticket" : "Quick Ticket");
  ImGui::TextDisabled(
      "Schwab is the preferred quote and route target when connected, but live submission stays gated behind an explicit desktop checkbox.");
  ImGui::Text("Routing: %s", PreferredTradingVenue().c_str());
  ImGui::Text("Quote Source: %s", ActiveMarketDataSource().c_str());
  ImGui::Separator();

  ImGui::InputText("Symbol", &ticket_symbol_, ImGuiInputTextFlags_CharsUppercase);
  if (ImGui::IsItemDeactivatedAfterEdit()) {
    SelectSymbol(ticket_symbol_);
  }
  ImGui::InputText("Quantity", &ticket_quantity_, ImGuiInputTextFlags_CharsDecimal);
  ImGui::Combo("Side", &ticket_instruction_, "BUY\0SELL\0");
  ImGui::Combo("Order Type", &ticket_order_type_, "LIMIT\0MARKET\0");
  ImGui::Combo("Asset Type", &ticket_asset_type_, "EQUITY\0OPTION\0");
  if (IsProviderConnected(Provider::kSchwab)) {
    ImGui::Checkbox("Enable live Schwab submit", &live_trade_enabled_);
    if (ticket_asset_type_ != 0 && selected_option_contract_symbol_.empty()) {
      ImGui::TextDisabled(
          "Select a call or put contract from the option chain before arming live option routing.");
    }
  }

  if (ticket_order_type_ == 0) {
    ImGui::InputText("Limit Price", &ticket_limit_price_,
                     ImGuiInputTextFlags_CharsDecimal);
  } else {
    ImGui::TextDisabled("Market orders use the provider preview estimate.");
  }

  if (ImGui::Button("Refresh Quote", ImVec2(-FLT_MIN, 0.0f))) {
    RefreshTradeQuote();
  }

  if (trade_quote_.has_value()) {
    ImGui::Spacing();
    ImGui::BeginChild(expanded ? "ExpandedTradeQuote" : "CompactTradeQuote",
                      ImVec2(0.0f, expanded ? 116.0f : 96.0f), true);
    ImGui::Text("%s", trade_quote_->instrument.symbol.c_str());
    ImGui::TextDisabled("%s", trade_quote_->instrument.name.c_str());
    ImGui::Text("Last: %s", FormatMoney(trade_quote_->quote.last_price).c_str());
    ImGui::Text("Bid / Ask: %s / %s", FormatMoney(trade_quote_->quote.bid).c_str(),
                FormatMoney(trade_quote_->quote.ask).c_str());
    ImGui::TextDisabled("Updated: %s",
                        trade_quote_->quote.updated_at.empty()
                            ? "-"
                            : trade_quote_->quote.updated_at.c_str());
    if (!selected_option_strike_.empty()) {
      ImGui::TextDisabled("Selected strike: %s", selected_option_strike_.c_str());
    }
    if (!selected_option_contract_symbol_.empty()) {
      ImGui::TextDisabled("Contract: %s", selected_option_contract_symbol_.c_str());
    }
    ImGui::EndChild();
  }

  const char* preview_label = IsProviderConnected(Provider::kSchwab)
                                  ? "Preview Schwab Ticket"
                                  : "Preview Ticket";
  const char* submit_label = live_trade_enabled_ &&
                                     IsProviderConnected(Provider::kSchwab)
                                 ? "Submit Live to Schwab"
                                 : (IsProviderConnected(Provider::kSchwab)
                                        ? "Stage Schwab Order"
                                        : "Submit Simulated");

  if (ImGui::Button(preview_label, ImVec2(-FLT_MIN / 2.0f, 0.0f))) {
    if (ticket_asset_type_ == 1 && selected_option_contract_symbol_.empty()) {
      workspace_message_ =
          "Select a specific call or put contract from the option chain before previewing an option order.";
      return;
    }
    try {
      RefreshTradeQuote();
      latest_preview_ =
          core::application::CompositionRoot::Instance().Orders().PreviewOrder(
              BuildOrderIntent());
      latest_submission_.reset();
      workspace_message_ = IsProviderConnected(Provider::kSchwab)
                               ? "Built a Schwab-first trade preview using live quote data."
                               : "Generated a simulated desktop order preview.";
    } catch (const std::exception& ex) {
      workspace_message_ = std::string("Order preview failed: ") + ex.what();
    }
  }
  if (expanded) {
    ImGui::SameLine();
  }
  if (ImGui::Button(submit_label, ImVec2(expanded ? -FLT_MIN : -FLT_MIN, 0.0f))) {
    if (ticket_asset_type_ == 1 && selected_option_contract_symbol_.empty()) {
      workspace_message_ =
          "Select a specific call or put contract from the option chain before submitting an option order.";
      return;
    }
    try {
      RefreshTradeQuote();
      latest_submission_ =
          core::application::CompositionRoot::Instance().Orders().SubmitOrder(
              BuildOrderIntent());
      latest_preview_.reset();
      RefreshWorkspaceData();
      workspace_message_ = IsProviderConnected(Provider::kSchwab)
                               ? (live_trade_enabled_
                                      ? "Submitted a live-gated Schwab order from the desktop trade desk."
                                      : "Stored a Schwab-routed desktop ticket in staged mode.")
                               : latest_submission_->message;
    } catch (const std::exception& ex) {
      workspace_message_ = std::string("Order submission failed: ") + ex.what();
    }
  }

  if (latest_preview_.has_value()) {
    ImGui::Separator();
    ImGui::Text("Preview");
    ImGui::Text("Estimated Total: $%s", latest_preview_->estimated_total.c_str());
    ImGui::Text("Mode: %s", latest_preview_->mode.c_str());
    ImGui::Text("Status: %s", latest_preview_->status.c_str());
    for (const auto& warning : latest_preview_->warnings) {
      ImGui::TextColored(AccentColor(), "%s", warning.c_str());
    }
  }

  if (latest_submission_.has_value()) {
    ImGui::Separator();
    ImGui::Text("Latest Submission");
    ImGui::Text("Order ID: %s", latest_submission_->submission_id.c_str());
    ImGui::Text("Status: %s", latest_submission_->status.c_str());
    ImGui::Text("Mode: %s", latest_submission_->mode.c_str());
    ImGui::TextWrapped("%s", latest_submission_->message.c_str());
  }
}

void Workspace::DrawTradeDesk() {
  ImGui::Text("Trade Desk");
  ImGui::TextDisabled(
      "This desk keeps a Schwab-first trading workflow visible today while defaulting to preview mode until you arm live Schwab submission.");
  ImGui::Separator();

  if (ImGui::BeginTable("TradeDeskLayout", 2,
                        ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInnerV)) {
    ImGui::TableNextColumn();
    ImGui::BeginChild("TradeDeskTicketPanel", ImVec2(0.0f, 0.0f), true);
    DrawTradingStatusCard();
    ImGui::Spacing();
    DrawQuickTradeTicket(true);
    ImGui::EndChild();

    ImGui::TableNextColumn();
    ImGui::BeginChild("TradeDeskOrdersPanel", ImVec2(0.0f, 0.0f), true);
    ImGui::Text("Open Orders");
    DrawOrdersTable("TradeDeskOpenOrders", open_orders_, 8, true);
    if (!selected_order_id_.empty()) {
      ImGui::Separator();
      ImGui::TextDisabled("Selected order: %s", selected_order_id_.c_str());
      if (ImGui::Button("Cancel Selected Order", ImVec2(-FLT_MIN, 0.0f))) {
        try {
          const auto result = core::application::CompositionRoot::Instance().Orders().
              CancelOrder({active_account_id_.empty() ? account_detail_.account_id
                                                      : active_account_id_,
                           selected_order_id_,
                           live_trade_enabled_ && IsProviderConnected(Provider::kSchwab)});
          RefreshWorkspaceData();
          workspace_message_ = result.message;
        } catch (const std::exception& ex) {
          workspace_message_ = std::string("Order cancellation failed: ") + ex.what();
        }
      }
      if (ImGui::Button("Replace Selected With Ticket", ImVec2(-FLT_MIN, 0.0f))) {
        try {
          const auto result = core::application::CompositionRoot::Instance().Orders().
              ReplaceOrder({selected_order_id_, BuildOrderIntent()});
          RefreshWorkspaceData();
          workspace_message_ = result.message;
        } catch (const std::exception& ex) {
          workspace_message_ = std::string("Order replace failed: ") + ex.what();
        }
      }
    }
    ImGui::Separator();
    ImGui::Text("Order History");
    DrawOrdersTable("TradeDeskOrderHistory", order_history_, 10);
    ImGui::EndChild();
    ImGui::EndTable();
  }
}

void Workspace::DrawCompactAccountCard() {
  ImGui::BeginChild("CompactAccountCard", ImVec2(0.0f, 150.0f), true);
  ImGui::Text("Account Snapshot");
  ImGui::TextColored(IsProviderConnected(Provider::kSchwab)
                         ? ProviderColor(Provider::kSchwab)
                         : ProviderColor(Provider::kIBKR),
                     "%s",
                     ActiveAccountSource().c_str());
  ImGui::TextDisabled("Account ID: %s",
                      account_detail_.account_id.empty() ? "-"
                                                         : account_detail_.account_id.c_str());
  ImGui::Separator();
  ImGui::Text("Portfolio Value: %s", FormatMoney(home_data_.portfolio.total_value).c_str());
  ImGui::Text("Cash: %s", FormatMoney(account_detail_.cash).c_str());
  ImGui::Text("Buying Power: %s", FormatMoney(account_detail_.buying_power).c_str());
  ImGui::Text("Positions: %d", static_cast<int>(account_detail_.positions.size()));
  ImGui::EndChild();
}

void Workspace::DrawMainSurface() {
  ImGui::BeginChild("WorkspaceMainSurface", ImVec2(0.0f, 0.0f), false);
  switch (active_surface_) {
    case Surface::kOverview:
      DrawOverview();
      break;
    case Surface::kWatchlists:
      watchlist_view_.Update();
      break;
    case Surface::kChart:
      chart_view_.Update();
      break;
    case Surface::kOptions:
      option_chain_view_.Update();
      break;
    case Surface::kTrade:
      DrawTradeDesk();
      break;
    case Surface::kAccount:
      account_view_.Update();
      break;
  }
  ImGui::EndChild();
}

void Workspace::DrawRightRail() {
  ImGui::BeginChild("WorkspaceRightRail", ImVec2(0.0f, 0.0f), true);
  DrawLinkedSymbolCard();
  ImGui::Spacing();
  DrawTradingStatusCard();
  ImGui::Spacing();
  DrawCompactAccountCard();
  ImGui::Spacing();

  if (active_surface_ != Surface::kTrade) {
    DrawQuickTradeTicket(false);
    ImGui::Separator();
    ImGui::Text("Recent Orders");
    DrawOrdersTable("RightRailOrders", order_history_, 5);
  } else {
    ImGui::Text("Trade Notes");
    ImGui::TextWrapped(
        "Live Schwab order routing is available behind an explicit checkbox, while IBKR remains account-only and options still need contract-symbol normalization for safe live routing.");
  }

  ImGui::Separator();
  ImGui::Text("Runtime");
  ImGui::TextWrapped("%s", RuntimeBasePath().c_str());
  ImGui::EndChild();
}

void Workspace::Update() {
  NewMasterFrame();
  WireEvents();
  if (!data_loaded_) {
    RefreshWorkspaceData();
  }

  menu_view_.Update();
  DrawHeader();

  if (ImGui::BeginTable("WorkspaceLayout", 3,
                        ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV,
                        ImVec2(0.0f, 0.0f))) {
    ImGui::TableSetupColumn("Sidebar", ImGuiTableColumnFlags_WidthFixed,
                            kSidebarWidth);
    ImGui::TableSetupColumn("Main", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("RightRail", ImGuiTableColumnFlags_WidthFixed,
                            kRightRailWidth);
    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    DrawSidebar();

    ImGui::TableSetColumnIndex(1);
    DrawMainSurface();

    ImGui::TableSetColumnIndex(2);
    DrawRightRail();

    ImGui::EndTable();
  }

  ImGui::End();
}

}  // namespace premia
