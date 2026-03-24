#include "premia/providers/schwab/order_provider.hpp"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Schwab/client.h"

namespace premia::providers::schwab {

namespace application = premia::core::application;
namespace pt = boost::property_tree;

namespace {

struct AccountSelection {
  std::string account_id;
  std::string account_hash;
};

auto ParseDouble(const std::string& value) -> double {
  try {
    return boost::lexical_cast<double>(value);
  } catch (const boost::bad_lexical_cast&) {
    return 0.0;
  }
}

auto FormatDouble(double value) -> std::string {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2) << value;
  return oss.str();
}

auto IsPlaceholderValue(const std::string& value) -> bool {
  return value.empty() || value.rfind("YOUR_", 0) == 0;
}

auto ReadConfigTree(const std::string& path, pt::ptree& tree) -> bool {
  std::ifstream file(path);
  if (!file.good()) {
    return false;
  }
  try {
    pt::read_json(file, tree);
  } catch (const std::exception&) {
    return false;
  }
  return true;
}

auto HasUsableConfig(const std::string& path) -> bool {
  pt::ptree tree;
  if (!ReadConfigTree(path, tree)) {
    return false;
  }
  const auto app_key = tree.get<std::string>("app_key", "");
  const auto app_secret = tree.get<std::string>("app_secret", "");
  return !IsPlaceholderValue(app_key) && !IsPlaceholderValue(app_secret);
}

auto LoadAuthorizedClient(::premia::schwab::Client& client,
                          const std::string& config_path,
                          const std::string& token_path) -> bool {
  if (!HasUsableConfig(config_path)) {
    return false;
  }
  if (!client.LoadConfig(config_path)) {
    return false;
  }
  client.LoadTokens(token_path);
  if (client.HasValidAccessToken()) {
    return true;
  }
  if (client.HasValidRefreshToken() && client.RefreshAccessToken()) {
    client.SaveTokens(token_path);
    return client.HasValidAccessToken();
  }
  return false;
}

auto EscapeJson(const std::string& value) -> std::string {
  std::string escaped;
  escaped.reserve(value.size());
  for (char ch : value) {
    switch (ch) {
      case '\\':
        escaped += "\\\\";
        break;
      case '"':
        escaped += "\\\"";
        break;
      case '\n':
        escaped += "\\n";
        break;
      case '\r':
        escaped += "\\r";
        break;
      case '\t':
        escaped += "\\t";
        break;
      default:
        escaped += ch;
        break;
    }
  }
  return escaped;
}

auto QuoteLastPrice(::premia::schwab::Client& client, const std::string& symbol)
    -> std::string {
  const auto response = client.GetQuote(symbol);
  if (response.empty()) {
    return "0.00";
  }

  std::istringstream stream(response);
  pt::ptree root;
  pt::read_json(stream, root);
  const auto* symbol_node = &root;
  if (auto child = root.get_child_optional(symbol)) {
    symbol_node = &child.get();
  }
  return FormatDouble(symbol_node->get<double>("quote.lastPrice",
                                                symbol_node->get<double>("lastPrice", 0.0)));
}

auto ResolveAccount(::premia::schwab::Client& client,
                    const std::string& requested_account_id) -> AccountSelection {
  if (!client.GetAccountNumbers()) {
    throw std::runtime_error("schwab order account bootstrap unavailable");
  }

  const auto& accounts = client.GetAllAccountHashes();
  if (accounts.empty()) {
    throw std::runtime_error("schwab returned no linked accounts for trading");
  }
  if (requested_account_id.empty()) {
    return {accounts.front().account_number, accounts.front().hash_value};
  }

  for (const auto& account : accounts) {
    if (account.account_number == requested_account_id ||
        account.hash_value == requested_account_id) {
      return {account.account_number, account.hash_value};
    }
  }
  throw std::runtime_error("requested schwab trading account was not found");
}

auto BuildPayload(const application::OrderIntentRequest& request) -> std::string {
  const auto quantity = request.quantity.empty() ? "1" : request.quantity;
  const auto asset_type = request.asset_type.empty() ? "EQUITY" : request.asset_type;
  const auto instruction = request.instruction.empty() ? "BUY" : request.instruction;
  const auto order_type = request.order_type.empty() ? "LIMIT" : request.order_type;
  const auto duration = request.duration.empty() ? "DAY" : request.duration;
  const auto session = request.session.empty() ? "NORMAL" : request.session;

  std::ostringstream payload;
  payload << "{";
  payload << "\"session\":\"" << EscapeJson(session) << "\",";
  payload << "\"duration\":\"" << EscapeJson(duration) << "\",";
  payload << "\"orderType\":\"" << EscapeJson(order_type) << "\",";
  if (!request.limit_price.empty() && order_type != "MARKET") {
    payload << "\"price\":\"" << EscapeJson(request.limit_price) << "\",";
  }
  payload << "\"orderStrategyType\":\"SINGLE\",";
  payload << "\"orderLegCollection\":[{";
  payload << "\"instruction\":\"" << EscapeJson(instruction) << "\",";
  payload << "\"quantity\":" << quantity << ",";
  payload << "\"instrument\":{";
  payload << "\"symbol\":\"" << EscapeJson(request.symbol) << "\",";
  payload << "\"assetType\":\"" << EscapeJson(asset_type) << "\"";
  payload << "}}]}";
  return payload.str();
}

auto OrderWindowTimestamp(int days_offset) -> std::string {
  const auto now = std::chrono::system_clock::now() +
                   std::chrono::hours(24 * days_offset);
  const std::time_t now_time = std::chrono::system_clock::to_time_t(now);
  std::tm utc_time{};
#if defined(_WIN32)
  gmtime_s(&utc_time, &now_time);
#else
  gmtime_r(&now_time, &utc_time);
#endif
  std::ostringstream oss;
  oss << std::put_time(&utc_time, "%Y-%m-%dT%H:%M:%S") << ".000Z";
  return oss.str();
}

auto ExtractOrderId(const std::string& location) -> std::string {
  if (location.empty()) {
    return "";
  }
  const auto slash = location.find_last_of('/');
  if (slash == std::string::npos || slash + 1 >= location.size()) {
    return location;
  }
  return location.substr(slash + 1);
}

auto ErrorMessage(const std::string& prefix, long status_code,
                  const std::string& body) -> std::string {
  std::ostringstream oss;
  oss << prefix << " (status " << status_code << ")";
  if (!body.empty()) {
    oss << ": " << body;
  }
  return oss.str();
}

auto ParseOrderRecord(const pt::ptree& order, const std::string& account_id)
    -> application::OrderRecordData {
  application::OrderRecordData data;
  data.order_id = order.get<std::string>("orderId", "");
  data.account_id = order.get<std::string>("accountNumber", account_id);
  data.symbol = "";
  data.asset_type = order.get<std::string>("assetType", "EQUITY");
  data.instruction = order.get<std::string>("instruction", "BUY");
  data.quantity = order.get<std::string>("quantity", "0");
  if (auto legs = order.get_child_optional("orderLegCollection")) {
    if (!legs->empty()) {
      const auto& first_leg = legs->front().second;
      data.instruction = first_leg.get<std::string>("instruction", data.instruction);
      data.quantity = first_leg.get<std::string>("quantity", data.quantity);
      if (auto instrument = first_leg.get_child_optional("instrument")) {
        data.symbol = instrument->get<std::string>("symbol", data.symbol);
        data.asset_type = instrument->get<std::string>("assetType", data.asset_type);
      }
    }
  }
  data.order_type = order.get<std::string>("orderType", "LIMIT");
  data.limit_price = order.get<std::string>("price", "0.00");
  data.mode = "live";
  data.status = order.get<std::string>("status", "UNKNOWN");
  data.submitted_at = order.get<std::string>("enteredTime", "");
  data.updated_at = order.get<std::string>("closeTime", data.submitted_at);
  data.message = order.get<std::string>("statusDescription", "");
  return data;
}

auto ParseOrderArray(const std::string& response, const std::string& account_id)
    -> std::vector<application::OrderRecordData> {
  std::vector<application::OrderRecordData> records;
  if (response.empty()) {
    return records;
  }
  std::istringstream stream(response);
  pt::ptree root;
  pt::read_json(stream, root);
  for (const auto& item : root) {
    records.push_back(ParseOrderRecord(item.second, account_id));
  }
  return records;
}

auto IsOpenOrderStatus(const std::string& status) -> bool {
  static const std::set<std::string> kOpenStatuses = {
      "NEW",          "QUEUED",          "WORKING",
      "ACCEPTED",     "PENDING_ACTIVATION", "AWAITING_PARENT_ORDER",
      "AWAITING_CONDITION", "AWAITING_STOP_CONDITION",
      "PENDING_ACKNOWLEDGEMENT", "AWAITING_RELEASE_TIME"};
  return kOpenStatuses.count(status) != 0;
}

}  // namespace

OrderProvider::OrderProvider(std::string config_path, std::string token_path)
    : config_path_(std::move(config_path)), token_path_(std::move(token_path)) {}

auto OrderProvider::PreviewOrder(const application::OrderIntentRequest& request)
    -> application::OrderPreviewData {
  ::premia::schwab::Client client;
  if (!LoadAuthorizedClient(client, config_path_, token_path_)) {
    throw std::runtime_error("schwab client unavailable");
  }

  const auto account = ResolveAccount(client, request.account_id);
  application::OrderIntentRequest normalized = request;
  if (normalized.order_type.empty()) {
    normalized.order_type = "LIMIT";
  }
  if (normalized.asset_type.empty()) {
    normalized.asset_type = "EQUITY";
  }
  if (normalized.instruction.empty()) {
    normalized.instruction = "BUY";
  }
  normalized.account_id = account.account_id;

  if (normalized.limit_price.empty() && normalized.order_type != "MARKET" &&
      normalized.asset_type == "EQUITY") {
    normalized.limit_price = QuoteLastPrice(client, normalized.symbol);
  }

  long status_code = 0;
  const auto response =
      client.PreviewOrder(account.account_hash, BuildPayload(normalized), &status_code);
  if (status_code < 200 || status_code >= 300) {
    throw std::runtime_error(
        ErrorMessage("schwab preview order failed", status_code, response));
  }

  const auto multiplier = normalized.asset_type == "OPTION" ? 100.0 : 1.0;
  const auto estimated_total = ParseDouble(normalized.quantity.empty() ? "1"
                                                                       : normalized.quantity) *
                               ParseDouble(normalized.limit_price) * multiplier;

  application::OrderPreviewData data;
  data.preview_id = "schwab_preview_" + normalized.symbol;
  data.account_id = account.account_id;
  data.symbol = normalized.symbol;
  data.asset_type = normalized.asset_type;
  data.instruction = normalized.instruction;
  data.quantity = normalized.quantity.empty() ? "1" : normalized.quantity;
  data.order_type = normalized.order_type;
  data.limit_price = normalized.limit_price;
  data.estimated_total = FormatDouble(estimated_total);
  data.mode = "live_preview";
  data.status = "preview";
  if (!request.confirm_live) {
    data.warnings.push_back(
        "Live submission requires confirmLive=true and an explicit UI opt-in.");
  }
  return data;
}

auto OrderProvider::SubmitOrder(const application::OrderIntentRequest& request)
    -> application::OrderSubmissionData {
  auto preview = PreviewOrder(request);

  application::OrderSubmissionData data;
  data.account_id = preview.account_id;
  data.symbol = preview.symbol;
  data.asset_type = preview.asset_type;
  data.instruction = preview.instruction;
  data.quantity = preview.quantity;
  data.order_type = preview.order_type;
  data.limit_price = preview.limit_price;
  data.submitted_at = CurrentUtcTimestamp();

  if (!request.confirm_live) {
    data.submission_id = preview.preview_id;
    data.mode = "live_preview";
    data.status = "not_submitted";
    data.message = "Enable confirmLive to submit a live Schwab order.";
    return data;
  }

  ::premia::schwab::Client client;
  if (!LoadAuthorizedClient(client, config_path_, token_path_)) {
    throw std::runtime_error("schwab client unavailable");
  }
  auto account = ResolveAccount(client, preview.account_id);

  application::OrderIntentRequest live_request = request;
  live_request.account_id = preview.account_id;
  live_request.limit_price = preview.limit_price;
  if (live_request.order_type.empty()) {
    live_request.order_type = preview.order_type;
  }
  if (live_request.asset_type.empty()) {
    live_request.asset_type = preview.asset_type;
  }
  if (live_request.instruction.empty()) {
    live_request.instruction = preview.instruction;
  }

  long status_code = 0;
  std::string location;
  if (!client.PlaceOrder(account.account_hash, BuildPayload(live_request), &location,
                         &status_code)) {
    throw std::runtime_error(
        ErrorMessage("schwab live order failed", status_code, location));
  }

  data.submission_id = ExtractOrderId(location);
  data.mode = "live";
  data.status = "submitted";
  data.message = "Live Schwab order submitted.";
  return data;
}

auto OrderProvider::CancelOrder(const application::OrderCancelRequest& request)
    -> application::OrderCancellationData {
  ::premia::schwab::Client client;
  if (!LoadAuthorizedClient(client, config_path_, token_path_)) {
    throw std::runtime_error("schwab client unavailable");
  }
  const auto account = ResolveAccount(client, request.account_id);

  application::OrderCancellationData data;
  data.order_id = request.order_id;
  data.account_id = account.account_id;
  data.cancelled_at = CurrentUtcTimestamp();

  if (!request.confirm_live) {
    data.mode = "live_preview";
    data.status = "not_cancelled";
    data.message = "Enable confirmLive to cancel a live Schwab order.";
    return data;
  }

  long status_code = 0;
  if (!client.CancelOrder(account.account_hash, request.order_id, &status_code)) {
    throw std::runtime_error(
        ErrorMessage("schwab cancel order failed", status_code, ""));
  }

  data.mode = "live";
  data.status = "cancelled";
  data.message = "Live Schwab order cancellation submitted.";
  return data;
}

auto OrderProvider::ReplaceOrder(const application::OrderReplaceRequest& request)
    -> application::OrderReplacementData {
  auto preview = PreviewOrder(request.replacement);

  application::OrderReplacementData data;
  data.replaced_order_id = request.order_id;
  data.account_id = preview.account_id;
  data.symbol = preview.symbol;
  data.asset_type = preview.asset_type;
  data.instruction = preview.instruction;
  data.quantity = preview.quantity;
  data.order_type = preview.order_type;
  data.limit_price = preview.limit_price;
  data.submitted_at = CurrentUtcTimestamp();

  if (!request.replacement.confirm_live) {
    data.replacement_id = preview.preview_id;
    data.mode = "live_preview";
    data.status = "not_replaced";
    data.message = "Enable confirmLive to replace a live Schwab order.";
    return data;
  }

  ::premia::schwab::Client client;
  if (!LoadAuthorizedClient(client, config_path_, token_path_)) {
    throw std::runtime_error("schwab client unavailable");
  }
  const auto account = ResolveAccount(client, preview.account_id);

  application::OrderIntentRequest live_request = request.replacement;
  live_request.account_id = preview.account_id;
  live_request.limit_price = preview.limit_price;
  if (live_request.order_type.empty()) {
    live_request.order_type = preview.order_type;
  }
  if (live_request.asset_type.empty()) {
    live_request.asset_type = preview.asset_type;
  }
  if (live_request.instruction.empty()) {
    live_request.instruction = preview.instruction;
  }

  long status_code = 0;
  std::string location;
  if (!client.ReplaceOrder(account.account_hash, request.order_id,
                           BuildPayload(live_request), &location, &status_code)) {
    throw std::runtime_error(
        ErrorMessage("schwab replace order failed", status_code, location));
  }

  data.replacement_id = ExtractOrderId(location);
  data.mode = "live";
  data.status = "replaced";
  data.message = "Live Schwab order replacement submitted.";
  return data;
}

auto OrderProvider::GetOpenOrders(const std::string& account_id) const
    -> std::vector<application::OrderRecordData> {
  ::premia::schwab::Client client;
  if (!LoadAuthorizedClient(client, config_path_, token_path_)) {
    throw std::runtime_error("schwab client unavailable");
  }
  const auto account = ResolveAccount(client, account_id);

  long status_code = 0;
  const auto response = client.GetOrdersForAccount(account.account_hash,
                                                   OrderWindowTimestamp(-60),
                                                   OrderWindowTimestamp(1), 300,
                                                   "", &status_code);
  if (status_code < 200 || status_code >= 300) {
    throw std::runtime_error(
        ErrorMessage("schwab open orders lookup failed", status_code, response));
  }

  std::vector<application::OrderRecordData> records;
  for (const auto& record : ParseOrderArray(response, account.account_id)) {
    if (IsOpenOrderStatus(record.status)) {
      records.push_back(record);
    }
  }
  return records;
}

auto OrderProvider::GetOrderHistory(const std::string& account_id) const
    -> std::vector<application::OrderRecordData> {
  ::premia::schwab::Client client;
  if (!LoadAuthorizedClient(client, config_path_, token_path_)) {
    throw std::runtime_error("schwab client unavailable");
  }
  const auto account = ResolveAccount(client, account_id);

  long status_code = 0;
  const auto response = client.GetOrdersForAccount(account.account_hash,
                                                   OrderWindowTimestamp(-60),
                                                   OrderWindowTimestamp(1), 300,
                                                   "", &status_code);
  if (status_code < 200 || status_code >= 300) {
    throw std::runtime_error(
        ErrorMessage("schwab order history lookup failed", status_code, response));
  }
  return ParseOrderArray(response, account.account_id);
}

auto OrderProvider::CurrentUtcTimestamp() const -> std::string {
  const auto now = std::chrono::system_clock::now();
  const std::time_t now_time = std::chrono::system_clock::to_time_t(now);
  std::tm utc_time{};
#if defined(_WIN32)
  gmtime_s(&utc_time, &now_time);
#else
  gmtime_r(&now_time, &utc_time);
#endif
  std::ostringstream oss;
  oss << std::put_time(&utc_time, "%Y-%m-%dT%H:%M:%SZ");
  return oss.str();
}

}  // namespace premia::providers::schwab
