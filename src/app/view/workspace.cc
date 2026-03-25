#include "workspace.h"

#include <SDL.h>
#include <imgui/imgui.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "premia/core/application/composition_root.hpp"

namespace premia {

namespace {

using core::application::ConnectionSummary;
using core::application::HoldingRow;
using core::application::Money;
using core::domain::ConnectionStatus;
using core::domain::Provider;
namespace pt = boost::property_tree;

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

auto AccountSuffix(const std::string& account_id) -> std::string {
  if (account_id.size() <= 4) {
    return account_id;
  }
  return account_id.substr(account_id.size() - 4);
}

void ApplyWorkspaceTheme() {
  auto& style = ImGui::GetStyle();
  style.WindowRounding = 14.0f;
  style.ChildRounding = 12.0f;
  style.FrameRounding = 10.0f;
  style.PopupRounding = 10.0f;
  style.ScrollbarRounding = 12.0f;
  style.GrabRounding = 10.0f;
  style.TabRounding = 10.0f;
  style.FrameBorderSize = 1.0f;
  style.ChildBorderSize = 1.0f;
  style.WindowPadding = ImVec2(16.0f, 14.0f);
  style.FramePadding = ImVec2(10.0f, 7.0f);
  style.ItemSpacing = ImVec2(10.0f, 8.0f);
  style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);

  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.10f, 0.13f, 1.0f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.13f, 0.17f, 1.0f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.14f, 0.18f, 0.98f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.22f, 0.26f, 0.33f, 0.95f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.0f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.24f, 0.30f, 1.0f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.23f, 0.28f, 0.35f, 1.0f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.18f, 0.24f, 0.31f, 1.0f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.33f, 0.43f, 1.0f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.29f, 0.40f, 0.52f, 1.0f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.18f, 0.24f, 0.31f, 0.95f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.27f, 0.37f, 0.48f, 1.0f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.31f, 0.43f, 0.56f, 1.0f);
  style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.14f, 0.17f, 0.22f, 1.0f);
  style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.24f, 0.28f, 0.35f, 1.0f);
  style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.18f, 0.21f, 0.27f, 1.0f);
  style.Colors[ImGuiCol_Tab] = ImVec4(0.16f, 0.20f, 0.27f, 1.0f);
  style.Colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.33f, 0.43f, 1.0f);
  style.Colors[ImGuiCol_TabActive] = ImVec4(0.27f, 0.37f, 0.48f, 1.0f);
  style.Colors[ImGuiCol_CheckMark] = AccentColor();
  style.Colors[ImGuiCol_SliderGrab] = AccentColor();
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.92f, 0.78f, 0.36f, 1.0f);
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

auto WorkspaceStatePath() -> std::filesystem::path {
  std::filesystem::path path;
  if (const char* override_path = std::getenv("PREMIA_RUNTIME_DIR");
      override_path != nullptr && override_path[0] != '\0') {
    path = std::filesystem::path(override_path);
#if defined(__APPLE__)
  } else if (const char* home = std::getenv("HOME"); home != nullptr && home[0] != '\0') {
    path = std::filesystem::path(home) / "Library/Application Support/Premia";
#elif defined(_WIN32)
  } else if (const char* appdata = std::getenv("APPDATA");
             appdata != nullptr && appdata[0] != '\0') {
    path = std::filesystem::path(appdata) / "Premia";
#else
  } else if (const char* xdg = std::getenv("XDG_STATE_HOME");
             xdg != nullptr && xdg[0] != '\0') {
    path = std::filesystem::path(xdg) / "premia";
  } else if (const char* home = std::getenv("HOME"); home != nullptr && home[0] != '\0') {
    path = std::filesystem::path(home) / ".local/state/premia";
#endif
  } else {
    path = std::filesystem::current_path() / ".premia-runtime";
  }
  std::error_code ec;
  std::filesystem::create_directories(path, ec);
  return path / "workspace_state.json";
}

}  // namespace

Workspace::Workspace() {
  WireEvents();
  LoadWorkspaceState();
}

void Workspace::ApplyLayoutPreset(const std::string& preset_id) {
  layout_preset_ = preset_id.empty() ? "Balanced" : preset_id;
  if (layout_preset_ == "Focus") {
    sidebar_width_ = 236.0f;
    right_rail_width_ = 300.0f;
  } else if (layout_preset_ == "Research") {
    sidebar_width_ = 332.0f;
    right_rail_width_ = 420.0f;
  } else if (layout_preset_ == "Balanced") {
    sidebar_width_ = 280.0f;
    right_rail_width_ = 360.0f;
  } else if (layout_preset_ == "Custom") {
    return;
  } else {
    layout_preset_ = "Balanced";
    sidebar_width_ = 280.0f;
    right_rail_width_ = 360.0f;
  }
}

void Workspace::LoadWorkspaceState() {
  const auto state_path = WorkspaceStatePath();
  if (!std::filesystem::exists(state_path)) {
    persisted_surface_key_ = CurrentSurfaceKey();
    persisted_account_id_ = active_account_id_;
    persisted_symbol_ = ticket_symbol_;
    persisted_chart_preset_ = chart_view_.GetActivePresetId();
    workspace_state_loaded_ = true;
    return;
  }

  try {
    pt::ptree tree;
    std::ifstream input(state_path);
    if (!input.good()) {
      throw std::runtime_error("workspace state unavailable");
    }
    pt::read_json(input, tree);
    active_account_id_ = tree.get<std::string>("activeAccountId", active_account_id_);
    ticket_symbol_ = tree.get<std::string>("focusedSymbol", ticket_symbol_);
    const auto surface_key = tree.get<std::string>("activeSurface", "overview");
    if (surface_key == "watchlists") {
      active_surface_ = Surface::kWatchlists;
    } else if (surface_key == "chart") {
      active_surface_ = Surface::kChart;
    } else if (surface_key == "options") {
      active_surface_ = Surface::kOptions;
    } else if (surface_key == "trade") {
      active_surface_ = Surface::kTrade;
    } else if (surface_key == "account") {
      active_surface_ = Surface::kAccount;
    } else {
      active_surface_ = Surface::kOverview;
    }

    const auto preset = tree.get<std::string>("chartPreset", "1Y");
    chart_view_.SetActivePresetId(preset);
    const auto layout_preset = tree.get<std::string>("layoutPreset", "Balanced");
    ApplyLayoutPreset(layout_preset);
    sidebar_width_ = tree.get<float>("sidebarWidth", sidebar_width_);
    right_rail_width_ = tree.get<float>("rightRailWidth", right_rail_width_);
    persisted_surface_key_ = surface_key;
    persisted_account_id_ = active_account_id_;
    persisted_symbol_ = ticket_symbol_;
    persisted_chart_preset_ = chart_view_.GetActivePresetId();
    persisted_layout_preset_ = layout_preset_;
    persisted_sidebar_width_ = sidebar_width_;
    persisted_right_rail_width_ = right_rail_width_;
  } catch (const std::exception&) {
    persisted_surface_key_ = CurrentSurfaceKey();
    persisted_account_id_ = active_account_id_;
    persisted_symbol_ = ticket_symbol_;
    persisted_chart_preset_ = chart_view_.GetActivePresetId();
    persisted_layout_preset_ = layout_preset_;
    persisted_sidebar_width_ = sidebar_width_;
    persisted_right_rail_width_ = right_rail_width_;
  }

  workspace_state_loaded_ = true;
}

void Workspace::PersistWorkspaceStateIfNeeded() {
  if (!workspace_state_loaded_) {
    return;
  }

  const auto surface_key = CurrentSurfaceKey();
  const auto chart_preset = chart_view_.GetActivePresetId();
  const auto state_path = WorkspaceStatePath();
  if (std::filesystem::exists(state_path) &&
      surface_key == persisted_surface_key_ &&
      active_account_id_ == persisted_account_id_ &&
      ticket_symbol_ == persisted_symbol_ &&
      chart_preset == persisted_chart_preset_ &&
      layout_preset_ == persisted_layout_preset_ &&
      std::abs(sidebar_width_ - persisted_sidebar_width_) < 0.5f &&
      std::abs(right_rail_width_ - persisted_right_rail_width_) < 0.5f) {
    return;
  }

  pt::ptree tree;
  tree.put("activeAccountId", active_account_id_);
  tree.put("focusedSymbol", ticket_symbol_);
  tree.put("activeSurface", surface_key);
  tree.put("chartPreset", chart_preset);
  tree.put("layoutPreset", layout_preset_);
  tree.put("sidebarWidth", sidebar_width_);
  tree.put("rightRailWidth", right_rail_width_);
  std::ofstream output(state_path);
  pt::write_json(output, tree);

  persisted_surface_key_ = surface_key;
  persisted_account_id_ = active_account_id_;
  persisted_symbol_ = ticket_symbol_;
  persisted_chart_preset_ = chart_preset;
  persisted_layout_preset_ = layout_preset_;
  persisted_sidebar_width_ = sidebar_width_;
  persisted_right_rail_width_ = right_rail_width_;
}

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
    account_view_.SetActiveAccountId(active_account_id_);
    chart_view_.SetActiveAccountId(active_account_id_);
    open_orders_ = root.Orders().GetOpenOrders(active_account_id_);
    order_history_ = root.Orders().GetOrderHistory(active_account_id_);
    if (ticket_symbol_.empty()) {
      if (!home_data_.top_holdings.empty()) {
        ticket_symbol_ = home_data_.top_holdings.front().symbol;
      } else if (!account_detail_.positions.empty()) {
        ticket_symbol_ = account_detail_.positions.front().symbol;
      } else {
        ticket_symbol_ = "AAPL";
      }
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

auto Workspace::CurrentSurfaceKey() const -> std::string {
  switch (active_surface_) {
    case Surface::kOverview:
      return "overview";
    case Surface::kWatchlists:
      return "watchlists";
    case Surface::kChart:
      return "chart";
    case Surface::kOptions:
      return "options";
    case Surface::kTrade:
      return "trade";
    case Surface::kAccount:
      return "account";
  }
  return "overview";
}

auto Workspace::ActiveSurfaceIndex() const -> int {
  switch (active_surface_) {
    case Surface::kOverview:
      return 0;
    case Surface::kWatchlists:
      return 1;
    case Surface::kChart:
      return 2;
    case Surface::kOptions:
      return 3;
    case Surface::kTrade:
      return 4;
    case Surface::kAccount:
      return 5;
  }
  return 0;
}

void Workspace::OpenCommandPalette() {
  command_symbol_ = ticket_symbol_;
  command_surface_index_ = ActiveSurfaceIndex();
  command_account_index_ = 0;
  for (int index = 0; index < static_cast<int>(brokerage_accounts_.size()); ++index) {
    if (brokerage_accounts_[index].account_id == active_account_id_) {
      command_account_index_ = index;
      break;
    }
  }
  command_layout_index_ = layout_preset_ == "Focus"
                              ? 0
                              : (layout_preset_ == "Balanced"
                                     ? 1
                                     : (layout_preset_ == "Research" ? 2 : 3));
  command_palette_open_ = true;
}

void Workspace::HandleKeyboardShortcuts() {
  auto& io = ImGui::GetIO();
  if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_K, false)) {
    OpenCommandPalette();
  }
  if (ImGui::IsKeyPressed(ImGuiKey_F1, false)) {
    shortcut_help_open_ = true;
  }
  if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_R, false)) {
    RefreshWorkspaceData();
  }
  if (!io.KeyCtrl || io.WantTextInput) {
    return;
  }

  if (ImGui::IsKeyPressed(ImGuiKey_1, false)) {
    active_surface_ = Surface::kOverview;
  } else if (ImGui::IsKeyPressed(ImGuiKey_2, false)) {
    active_surface_ = Surface::kWatchlists;
  } else if (ImGui::IsKeyPressed(ImGuiKey_3, false)) {
    active_surface_ = Surface::kChart;
  } else if (ImGui::IsKeyPressed(ImGuiKey_4, false)) {
    active_surface_ = Surface::kOptions;
  } else if (ImGui::IsKeyPressed(ImGuiKey_5, false)) {
    active_surface_ = Surface::kTrade;
  } else if (ImGui::IsKeyPressed(ImGuiKey_6, false)) {
    active_surface_ = Surface::kAccount;
  }
}

void Workspace::DrawCommandPalette() {
  if (command_palette_open_) {
    ImGui::OpenPopup("Command Palette");
    command_palette_open_ = false;
  }

  const ImVec2 display = ImGui::GetIO().DisplaySize;
  ImGui::SetNextWindowSize(ImVec2(540.0f, 0.0f), ImGuiCond_Appearing);
  ImGui::SetNextWindowPos(ImVec2(display.x * 0.5f, display.y * 0.18f),
                          ImGuiCond_Appearing, ImVec2(0.5f, 0.0f));

  if (!ImGui::BeginPopupModal("Command Palette", nullptr,
                              ImGuiWindowFlags_AlwaysAutoResize)) {
    return;
  }

  static const char* kSurfaceLabels[] = {"Overview",  "Watchlists", "Charts",
                                         "Options",   "Trade Desk", "Account"};
  static const char* kLayoutLabels[] = {"Focus", "Balanced", "Research",
                                        "Custom"};

  ImGui::TextDisabled("Ctrl+1..6 switches surfaces. Ctrl+R refreshes. Ctrl+K reopens this palette.");
  ImGui::Separator();
  ImGui::InputTextWithHint("##commandSymbol", "Focused symbol (e.g. AAPL)",
                           &command_symbol_, ImGuiInputTextFlags_CharsUppercase);
  ImGui::Combo("Surface", &command_surface_index_, kSurfaceLabels,
               IM_ARRAYSIZE(kSurfaceLabels));

  if (!brokerage_accounts_.empty()) {
    std::vector<const char*> account_names;
    account_names.reserve(brokerage_accounts_.size());
    for (const auto& account : brokerage_accounts_) {
      account_names.push_back(account.display_name.c_str());
    }
    ImGui::Combo("Account", &command_account_index_, account_names.data(),
                 static_cast<int>(account_names.size()));
  }

  ImGui::Combo("Layout", &command_layout_index_, kLayoutLabels,
               IM_ARRAYSIZE(kLayoutLabels));

  if (ImGui::Button("Apply", ImVec2(140.0f, 0.0f))) {
    const std::string requested_account_id =
        brokerage_accounts_.empty()
            ? std::string()
            : brokerage_accounts_[std::clamp(command_account_index_, 0,
                                             static_cast<int>(brokerage_accounts_.size()) - 1)]
                  .account_id;
    if (!requested_account_id.empty() && requested_account_id != active_account_id_) {
      active_account_id_ = requested_account_id;
      RefreshWorkspaceData();
    }

    if (command_layout_index_ >= 0 && command_layout_index_ < 4) {
      ApplyLayoutPreset(kLayoutLabels[command_layout_index_]);
    }

    if (!command_symbol_.empty()) {
      SelectSymbol(command_symbol_);
    }
    active_surface_ = static_cast<Surface>(std::clamp(command_surface_index_, 0, 5));
    workspace_message_ = "Applied command palette changes.";
    ImGui::CloseCurrentPopup();
  }
  ImGui::SameLine();
  if (ImGui::Button("Cancel", ImVec2(140.0f, 0.0f))) {
    ImGui::CloseCurrentPopup();
  }

  ImGui::EndPopup();
}

void Workspace::DrawShortcutHelpOverlay() {
  if (shortcut_help_open_) {
    ImGui::OpenPopup("Desktop Shortcuts");
    shortcut_help_open_ = false;
  }

  ImGui::SetNextWindowSize(ImVec2(560.0f, 0.0f), ImGuiCond_Appearing);
  if (!ImGui::BeginPopupModal("Desktop Shortcuts", nullptr,
                              ImGuiWindowFlags_AlwaysAutoResize)) {
    return;
  }

  ImGui::Text("Navigation");
  ImGui::Separator();
  ImGui::BulletText("Ctrl+1  Overview");
  ImGui::BulletText("Ctrl+2  Watchlists");
  ImGui::BulletText("Ctrl+3  Charts");
  ImGui::BulletText("Ctrl+4  Options");
  ImGui::BulletText("Ctrl+5  Trade Desk");
  ImGui::BulletText("Ctrl+6  Account");
  ImGui::BulletText("Ctrl+R  Refresh workspace");
  ImGui::BulletText("Ctrl+K  Open command palette");
  ImGui::BulletText("F1      Show this help");

  ImGui::Spacing();
  ImGui::Text("Tips");
  ImGui::Separator();
  ImGui::BulletText("Drag the divider bars between columns to resize the layout");
  ImGui::BulletText("Use layout presets in the sidebar to snap back to a known layout");
  ImGui::BulletText("Selecting holdings, watchlist rows, or option contracts keeps the symbol linked across the workspace");

  if (ImGui::Button("Close", ImVec2(140.0f, 0.0f))) {
    ImGui::CloseCurrentPopup();
  }
  ImGui::EndPopup();
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
    return "Charles Schwab desk (live quotes, preview-first execution)";
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
  if (ImGui::Button("Palette", ImVec2(96.0f, 0.0f))) {
    OpenCommandPalette();
  }
  ImGui::SameLine();
  if (ImGui::Button("Shortcuts", ImVec2(110.0f, 0.0f))) {
    shortcut_help_open_ = true;
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
  DrawLayoutPanel();
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

  if (!brokerage_accounts_.empty()) {
    DrawBrokerageAccountsPanel();
    ImGui::Spacing();
  }

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
    ImGui::Spacing();
    DrawQuickActionsPanel();
    ImGui::EndChild();
    ImGui::EndTable();
  }
}

void Workspace::DrawBrokerageAccountsPanel() {
  ImGui::BeginChild("BrokerageAccountsPanel", ImVec2(0.0f, 148.0f), true);
  ImGui::Text("Linked Brokerage Accounts");
  ImGui::TextDisabled(
      "Switching accounts keeps the same desktop workspace while updating portfolio, holdings, and order state.");
  ImGui::Separator();

  const int column_count = std::min<int>(3, std::max<int>(1, brokerage_accounts_.size()));
  double max_value = 0.0;
  for (const auto& account : brokerage_accounts_) {
    max_value = std::max(max_value, ParseSignedAmount(account.total_value.amount));
  }
  if (ImGui::BeginTable("BrokerageAccountsTable", column_count,
                        ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInnerV)) {
    int column_index = 0;
    for (const auto& account : brokerage_accounts_) {
      if (column_index == 0) {
        ImGui::TableNextRow();
      }
      ImGui::TableSetColumnIndex(column_index);
      ImGui::BeginChild(("AccountCard" + account.account_id).c_str(),
                        ImVec2(0.0f, 96.0f), true);
      const bool is_active = account.account_id == active_account_id_;
      ImGui::TextColored(ProviderColor(account.provider), "%s",
                         account.display_name.c_str());
      ImGui::TextDisabled("Account ending %s", AccountSuffix(account.account_id).c_str());
      ImGui::Text("Value: %s", FormatMoney(account.total_value).c_str());
      const float ratio = max_value <= 0.0 ? 0.0f
                                           : static_cast<float>(
                                                 ParseSignedAmount(account.total_value.amount) /
                                                 max_value);
      ImGui::ProgressBar(ratio, ImVec2(-FLT_MIN, 0.0f),
                         (std::to_string(account.holdings_count) + " holdings").c_str());
      ImGui::TextDisabled("Holdings: %d", account.holdings_count);
      if (is_active) {
        ImGui::TextColored(PositiveColor(), "Active workspace account");
      } else if (ImGui::Button(("Switch##" + account.account_id).c_str(),
                               ImVec2(-FLT_MIN, 0.0f))) {
        active_account_id_ = account.account_id;
        RefreshWorkspaceData();
      }
      ImGui::EndChild();
      column_index = (column_index + 1) % column_count;
    }
    ImGui::EndTable();
  }

  ImGui::EndChild();
}

void Workspace::DrawQuickActionsPanel() {
  ImGui::BeginChild("QuickActionsPanel", ImVec2(0.0f, 134.0f), true);
  ImGui::Text("Quick Actions");
  ImGui::TextDisabled("Jump straight into the next likely workflow.");
  if (ImGui::Button("Refresh All", ImVec2(120.0f, 0.0f))) {
    RefreshWorkspaceData();
  }
  ImGui::SameLine();
  if (ImGui::Button("Trade Desk", ImVec2(120.0f, 0.0f))) {
    active_surface_ = Surface::kTrade;
  }
  ImGui::SameLine();
  if (ImGui::Button("Account", ImVec2(120.0f, 0.0f))) {
    active_surface_ = Surface::kAccount;
  }
  if (ImGui::Button("Charts", ImVec2(120.0f, 0.0f))) {
    active_surface_ = Surface::kChart;
  }
  ImGui::SameLine();
  if (ImGui::Button("Options", ImVec2(120.0f, 0.0f))) {
    active_surface_ = Surface::kOptions;
  }
  ImGui::SameLine();
  if (ImGui::Button("Watchlists", ImVec2(120.0f, 0.0f))) {
    active_surface_ = Surface::kWatchlists;
  }
  ImGui::TextWrapped("Focused symbol `%s` stays linked across chart, options, and ticket surfaces.",
                     ticket_symbol_.c_str());
  ImGui::EndChild();
}

void Workspace::DrawLayoutPanel() {
  ImGui::BeginChild("LayoutPanel", ImVec2(0.0f, 180.0f), true);
  ImGui::Text("Layout");
  ImGui::TextDisabled("Tune panel widths and save them into the desktop workspace state.");
  if (ImGui::Button("Focus", ImVec2(78.0f, 0.0f))) {
    ApplyLayoutPreset("Focus");
  }
  ImGui::SameLine();
  if (ImGui::Button("Balanced", ImVec2(88.0f, 0.0f))) {
    ApplyLayoutPreset("Balanced");
  }
  ImGui::SameLine();
  if (ImGui::Button("Research", ImVec2(86.0f, 0.0f))) {
    ApplyLayoutPreset("Research");
  }
  ImGui::TextDisabled("Preset: %s", layout_preset_.c_str());
  if (ImGui::SliderFloat("Sidebar Width", &sidebar_width_, 220.0f, 360.0f, "%.0f px")) {
    layout_preset_ = "Custom";
  }
  if (ImGui::SliderFloat("Right Rail", &right_rail_width_, 280.0f, 460.0f,
                         "%.0f px")) {
    layout_preset_ = "Custom";
  }
  ImGui::TextDisabled("Shortcuts: Ctrl+1..6 views, Ctrl+R refresh, Ctrl+K command palette");
  ImGui::EndChild();
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
  if (!selected_option_contract_symbol_.empty()) {
    ImGui::TextDisabled("Contract: %s", selected_option_contract_symbol_.c_str());
    ImGui::TextDisabled("Leg: %s", selected_option_is_call_ ? "Call" : "Put");
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

void Workspace::DrawTradingPlanCard() {
  ImGui::BeginChild("TradingPlanCard", ImVec2(0.0f, 210.0f), true);
  ImGui::Text("Trading Plan");
  ImGui::TextDisabled(
      "Shared chart annotations for the active account and focused symbol show up here so options and trade workflows stay aligned.");

  try {
    const auto chart = core::application::CompositionRoot::Instance()
                           .AppService()
                           .GetChartScreen(ticket_symbol_, "1M", "1D", true,
                                           active_account_id_);
    if (chart.annotations.empty()) {
      ImGui::TextDisabled("No saved annotations for %s.", ticket_symbol_.c_str());
    } else {
      for (const auto& annotation : chart.annotations) {
        ImGui::Separator();
        ImGui::Text("%s", annotation.label.c_str());
        ImGui::SameLine();
        ImGui::TextDisabled("%s", annotation.kind.c_str());
        ImGui::Text("$%s", annotation.price.c_str());
        if (ImGui::Button((std::string("Use as Limit##") + annotation.id).c_str(),
                          ImVec2(-FLT_MIN, 0.0f))) {
          ticket_limit_price_ = annotation.price;
          ticket_order_type_ = 0;
          workspace_message_ = "Loaded a trading plan level into the ticket limit price.";
        }
      }
    }
  } catch (const std::exception& ex) {
    ImGui::TextDisabled("Trading plan unavailable: %s", ex.what());
  }

  ImGui::EndChild();
}

void Workspace::DrawSelectedOrderCard() {
  ImGui::BeginChild("SelectedOrderCard", ImVec2(0.0f, 138.0f), true);
  ImGui::Text("Selected Order");

  auto find_order = [this]() -> const core::application::OrderRecordData* {
    auto in_open = std::find_if(open_orders_.begin(), open_orders_.end(),
                                [this](const core::application::OrderRecordData& order) {
                                  return order.order_id == selected_order_id_;
                                });
    if (in_open != open_orders_.end()) {
      return &(*in_open);
    }
    auto in_history = std::find_if(order_history_.begin(), order_history_.end(),
                                   [this](const core::application::OrderRecordData& order) {
                                     return order.order_id == selected_order_id_;
                                   });
    return in_history == order_history_.end() ? nullptr : &(*in_history);
  };

  const auto* order = find_order();
  if (order == nullptr) {
    ImGui::TextDisabled("Pick an open order to load it into the ticket and manage it.");
    ImGui::EndChild();
    return;
  }

  ImGui::Text("%s", order->symbol.c_str());
  ImGui::TextDisabled("Order ID: %s", order->order_id.c_str());
  ImGui::Text("%s %s %s", order->instruction.c_str(), order->quantity.c_str(),
              order->asset_type.c_str());
  ImGui::Text("Type: %s | Status: %s", order->order_type.c_str(),
              order->status.c_str());
  ImGui::TextDisabled("Mode: %s", order->mode.c_str());
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
    DrawTradingPlanCard();
    ImGui::Spacing();
    DrawQuickTradeTicket(true);
    ImGui::EndChild();

    ImGui::TableNextColumn();
    ImGui::BeginChild("TradeDeskOrdersPanel", ImVec2(0.0f, 0.0f), true);
    DrawSelectedOrderCard();
    ImGui::Spacing();
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
  DrawTradingPlanCard();
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
  ApplyWorkspaceTheme();
  PersistWorkspaceStateIfNeeded();
  NewMasterFrame();
  WireEvents();
  if (!data_loaded_) {
    RefreshWorkspaceData();
  }

  menu_view_.Update();
  HandleKeyboardShortcuts();
  DrawHeader();
  DrawCommandPalette();
  DrawShortcutHelpOverlay();

  constexpr float splitter_width = 8.0f;
  constexpr float min_sidebar_width = 220.0f;
  constexpr float min_main_width = 480.0f;
  constexpr float min_right_width = 280.0f;

  const ImVec2 layout_origin = ImGui::GetCursorScreenPos();
  const ImVec2 layout_size = ImGui::GetContentRegionAvail();
  const float max_side_total =
      std::max(min_sidebar_width + min_right_width,
               layout_size.x - min_main_width - splitter_width * 2.0f);
  if (sidebar_width_ + right_rail_width_ > max_side_total) {
    float overflow = sidebar_width_ + right_rail_width_ - max_side_total;
    const float right_reduction = std::min(overflow, right_rail_width_ - min_right_width);
    right_rail_width_ -= right_reduction;
    overflow -= right_reduction;
    const float left_reduction = std::min(overflow, sidebar_width_ - min_sidebar_width);
    sidebar_width_ -= left_reduction;
  }
  sidebar_width_ = std::max(min_sidebar_width, sidebar_width_);
  right_rail_width_ = std::max(min_right_width, right_rail_width_);
  float main_width =
      std::max(min_main_width,
               layout_size.x - sidebar_width_ - right_rail_width_ - splitter_width * 2.0f);

  ImGui::SetCursorScreenPos(layout_origin);
  ImGui::BeginChild("SidebarShell", ImVec2(sidebar_width_, layout_size.y), false,
                    ImGuiWindowFlags_NoScrollbar);
  DrawSidebar();
  ImGui::EndChild();

  ImGui::SetCursorScreenPos(
      ImVec2(layout_origin.x + sidebar_width_, layout_origin.y));
  ImGui::InvisibleButton("SidebarSplitter", ImVec2(splitter_width, layout_size.y));
  if (ImGui::IsItemHovered() || ImGui::IsItemActive()) {
    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
  }
  if (ImGui::IsItemActive()) {
    sidebar_width_ += ImGui::GetIO().MouseDelta.x;
    layout_preset_ = "Custom";
  }
  ImGui::GetWindowDrawList()->AddRectFilled(
      ImGui::GetItemRectMin(), ImGui::GetItemRectMax(),
      ImGui::GetColorU32(ImVec4(0.24f, 0.28f, 0.34f, 0.85f)), 4.0f);

  const float main_x = layout_origin.x + sidebar_width_ + splitter_width;
  main_width =
      std::max(min_main_width,
               layout_size.x - sidebar_width_ - right_rail_width_ - splitter_width * 2.0f);
  ImGui::SetCursorScreenPos(ImVec2(main_x, layout_origin.y));
  ImGui::BeginChild("MainShell", ImVec2(main_width, layout_size.y), false,
                    ImGuiWindowFlags_NoScrollbar);
  DrawMainSurface();
  ImGui::EndChild();

  const float right_splitter_x = main_x + main_width;
  ImGui::SetCursorScreenPos(ImVec2(right_splitter_x, layout_origin.y));
  ImGui::InvisibleButton("RightRailSplitter", ImVec2(splitter_width, layout_size.y));
  if (ImGui::IsItemHovered() || ImGui::IsItemActive()) {
    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
  }
  if (ImGui::IsItemActive()) {
    right_rail_width_ -= ImGui::GetIO().MouseDelta.x;
    layout_preset_ = "Custom";
  }
  ImGui::GetWindowDrawList()->AddRectFilled(
      ImGui::GetItemRectMin(), ImGui::GetItemRectMax(),
      ImGui::GetColorU32(ImVec4(0.24f, 0.28f, 0.34f, 0.85f)), 4.0f);

  const float right_x = right_splitter_x + splitter_width;
  ImGui::SetCursorScreenPos(ImVec2(right_x, layout_origin.y));
  ImGui::BeginChild("RightRailShell", ImVec2(right_rail_width_, layout_size.y), false,
                    ImGuiWindowFlags_NoScrollbar);
  DrawRightRail();
  ImGui::EndChild();

  ImGui::SetCursorScreenPos(
      ImVec2(layout_origin.x, layout_origin.y + layout_size.y));

  PersistWorkspaceStateIfNeeded();
  ImGui::End();
}

}  // namespace premia
