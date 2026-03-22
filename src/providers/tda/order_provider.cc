#include "premia/providers/tda/order_provider.hpp"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "TDAmeritrade/client.h"
#include "TDAmeritrade/parser.h"

namespace premia::providers::tda {

namespace application = premia::core::application;
namespace pt = boost::property_tree;

namespace {

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

}  // namespace

OrderProvider::OrderProvider(std::string config_path)
    : config_path_(std::move(config_path)) {}

auto OrderProvider::HasUsableConfig() const -> bool {
  pt::ptree tree;
  if (!ReadConfigTree(config_path_, tree)) {
    return false;
  }
  const auto consumer_key = tree.get<std::string>("consumer_key", "");
  const auto refresh_token = tree.get<std::string>("refresh_token", "");
  return !IsPlaceholderValue(consumer_key) && !IsPlaceholderValue(refresh_token);
}

auto OrderProvider::ResolveAccountId(::premia::tda::Client& client,
                                     const std::string& requested_account_id) const
    -> std::string {
  if (!requested_account_id.empty()) {
    return requested_account_id;
  }
  (void)client.get_all_accounts();
  const auto account_ids = client.get_all_account_ids();
  if (account_ids.empty()) {
    throw std::runtime_error("tda returned no account ids");
  }
  return account_ids.front();
}

auto OrderProvider::BuildPayload(const application::OrderIntentRequest& request,
                                 const std::string& account_id) const -> std::string {
  (void)account_id;
  pt::ptree root;
  root.put("orderType", request.order_type.empty() ? "LIMIT" : request.order_type);
  root.put("session", request.session.empty() ? "NORMAL" : request.session);
  root.put("duration", request.duration.empty() ? "DAY" : request.duration);
  root.put("orderStrategyType", "SINGLE");
  if (!request.limit_price.empty()) {
    root.put("price", request.limit_price);
  }
  if (request.asset_type == "OPTION") {
    root.put("complexOrderStrategyType", "NONE");
  }

  pt::ptree legs;
  pt::ptree leg;
  leg.put("instruction", request.instruction);
  leg.put("quantity", request.quantity);
  pt::ptree instrument;
  instrument.put("symbol", request.symbol);
  instrument.put("assetType", request.asset_type);
  leg.add_child("instrument", instrument);
  legs.push_back({"", leg});
  root.add_child("orderLegCollection", legs);

  std::ostringstream oss;
  pt::write_json(oss, root, false);
  return oss.str();
}

auto OrderProvider::PreviewOrder(const application::OrderIntentRequest& request)
    -> application::OrderPreviewData {
  if (!HasUsableConfig()) {
    throw std::runtime_error("tda order config unavailable");
  }

  pt::ptree tree;
  if (!ReadConfigTree(config_path_, tree)) {
    throw std::runtime_error("unable to read tda order config");
  }

  ::premia::tda::Client client;
  ::premia::tda::Parser parser;
  client.addAuth(tree.get<std::string>("consumer_key"),
                 tree.get<std::string>("refresh_token"));
  client.fetch_access_token();
  const auto account_id = ResolveAccountId(client, request.account_id);

  std::string limit_price = request.limit_price;
  if (limit_price.empty() && request.asset_type == "EQUITY") {
    auto quote = parser.parse_quote(parser.read_response(client.get_quote(request.symbol)));
    limit_price = quote.getQuoteVariable("lastPrice");
  }
  if (limit_price.empty()) {
    limit_price = "0.00";
  }

  const auto multiplier = request.asset_type == "OPTION" ? 100.0 : 1.0;
  const auto estimated_total = ParseDouble(request.quantity) *
                               ParseDouble(limit_price) * multiplier;

  application::OrderPreviewData data;
  data.preview_id = "tda_preview_" + request.symbol;
  data.account_id = account_id;
  data.symbol = request.symbol;
  data.asset_type = request.asset_type;
  data.instruction = request.instruction;
  data.quantity = request.quantity;
  data.order_type = request.order_type;
  data.limit_price = limit_price;
  data.estimated_total = FormatDouble(estimated_total);
  data.mode = request.confirm_live ? "live" : "preview";
  data.status = "preview";
  if (!request.confirm_live) {
    data.warnings.push_back("Live submission requires confirmLive=true.");
  }
  return data;
}

auto OrderProvider::SubmitOrder(const application::OrderIntentRequest& request)
    -> application::OrderSubmissionData {
  auto preview = PreviewOrder(request);
  application::OrderSubmissionData data;
  data.submission_id = "tda_submission_" + request.symbol;
  data.account_id = preview.account_id;
  data.symbol = preview.symbol;
  data.asset_type = preview.asset_type;
  data.instruction = preview.instruction;
  data.quantity = preview.quantity;
  data.order_type = preview.order_type;
  data.limit_price = preview.limit_price;
  data.submitted_at = CurrentUtcTimestamp();

  if (!request.confirm_live) {
    data.mode = "preview";
    data.status = "not_submitted";
    data.message = "Set confirmLive=true to submit a live order.";
    return data;
  }

  pt::ptree tree;
  ReadConfigTree(config_path_, tree);
  ::premia::tda::Client client;
  client.addAuth(tree.get<std::string>("consumer_key"),
                 tree.get<std::string>("refresh_token"));
  client.fetch_access_token();
  const auto payload = BuildPayload(request, preview.account_id);
  client.place_order_payload(preview.account_id, payload);
  data.mode = "live";
  data.status = "submitted";
  data.message = "Live order submitted to TDA.";
  return data;
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

}  // namespace premia::providers::tda
