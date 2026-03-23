#include "premia/providers/local/order_provider.hpp"

#include <atomic>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace premia::providers::local {

namespace application = premia::core::application;
namespace pt = boost::property_tree;

namespace {

std::atomic<unsigned long long> g_order_counter{0};

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

auto MakeRuntimeId(const std::string& prefix) -> std::string {
  const auto ticks = std::chrono::steady_clock::now().time_since_epoch().count();
  return prefix + "_" + std::to_string(ticks);
}

auto DefaultPriceForSymbol(const std::string& symbol) -> std::string {
  if (symbol == "AAPL") return "217.00";
  if (symbol == "MSFT") return "420.10";
  if (symbol == "SPY") return "519.50";
  return "100.00";
}

auto MultiplierForAssetType(const std::string& asset_type) -> double {
  return asset_type == "OPTION" ? 100.0 : 1.0;
}

}  // namespace

OrderProvider::OrderProvider(std::string path) : path_(std::move(path)) {}

auto OrderProvider::PreviewOrder(const application::OrderIntentRequest& request)
    -> application::OrderPreviewData {
  const auto limit_price = request.limit_price.empty()
                               ? DefaultPriceForSymbol(request.symbol)
                               : request.limit_price;
  const auto estimated_total = ParseDouble(request.quantity) *
                               ParseDouble(limit_price) *
                               MultiplierForAssetType(request.asset_type);

  application::OrderPreviewData data;
  data.preview_id = MakeRuntimeId("preview");
  data.account_id = request.account_id.empty() ? "local_acc" : request.account_id;
  data.symbol = request.symbol;
  data.asset_type = request.asset_type;
  data.instruction = request.instruction;
  data.quantity = request.quantity;
  data.order_type = request.order_type;
  data.limit_price = limit_price;
  data.estimated_total = FormatDouble(estimated_total);
  data.mode = "simulated";
  data.status = "preview";
  if (request.limit_price.empty()) {
    data.warnings.push_back("Using fallback local price estimate.");
  }
  return data;
}

auto OrderProvider::SubmitOrder(const application::OrderIntentRequest& request)
    -> application::OrderSubmissionData {
  const auto preview = PreviewOrder(request);

  pt::ptree root;
  {
    std::ifstream input(path_);
    if (input.good()) {
      try {
        pt::read_json(input, root);
      } catch (const std::exception&) {
        root = pt::ptree{};
      }
    }
  }

  pt::ptree orders;
  if (auto child = root.get_child_optional("orders")) {
    orders = child.get();
  }

  pt::ptree order;
  order.put("submissionId", MakeRuntimeId("submission"));
  order.put("accountId", preview.account_id);
  order.put("symbol", preview.symbol);
  order.put("assetType", preview.asset_type);
  order.put("instruction", preview.instruction);
  order.put("quantity", preview.quantity);
  order.put("orderType", preview.order_type);
  order.put("limitPrice", preview.limit_price);
  order.put("mode", request.confirm_live ? "simulated_confirmed" : "simulated");
  order.put("status", "accepted");
  order.put("submittedAt", CurrentUtcTimestamp());
  order.put("message", request.confirm_live
                           ? "Simulated local submission accepted."
                           : "Preview only submission stored locally.");
  orders.push_back({"", order});
  root.put_child("orders", orders);

  std::ofstream output(path_);
  pt::write_json(output, root);

  application::OrderSubmissionData data;
  data.submission_id = order.get<std::string>("submissionId");
  data.account_id = preview.account_id;
  data.symbol = preview.symbol;
  data.asset_type = preview.asset_type;
  data.instruction = preview.instruction;
  data.quantity = preview.quantity;
  data.order_type = preview.order_type;
  data.limit_price = preview.limit_price;
  data.mode = order.get<std::string>("mode");
  data.status = order.get<std::string>("status");
  data.submitted_at = order.get<std::string>("submittedAt");
  data.message = order.get<std::string>("message");
  return data;
}

auto OrderProvider::CancelOrder(const application::OrderCancelRequest& request)
    -> application::OrderCancellationData {
  pt::ptree root;
  {
    std::ifstream input(path_);
    if (input.good()) {
      try {
        pt::read_json(input, root);
      } catch (const std::exception&) {
        root = pt::ptree{};
      }
    }
  }

  pt::ptree orders;
  if (auto child = root.get_child_optional("orders")) {
    orders = child.get();
  }
  bool found = false;
  for (auto& order : orders) {
    if (order.second.get<std::string>("submissionId", "") == request.order_id) {
      order.second.put("status", "cancelled");
      order.second.put("cancelledAt", CurrentUtcTimestamp());
      order.second.put("message", request.confirm_live
                                     ? "Simulated live cancellation accepted."
                                     : "Simulated local cancellation accepted.");
      found = true;
      break;
    }
  }
  if (!found) {
    throw std::runtime_error("order not found");
  }

  root.put_child("orders", orders);
  std::ofstream output(path_);
  pt::write_json(output, root);

  application::OrderCancellationData data;
  data.order_id = request.order_id;
  data.account_id = request.account_id.empty() ? "local_acc" : request.account_id;
  data.mode = request.confirm_live ? "simulated_confirmed" : "simulated";
  data.status = "cancelled";
  data.cancelled_at = CurrentUtcTimestamp();
  data.message = request.confirm_live
                     ? "Simulated live cancellation accepted."
                     : "Simulated local cancellation accepted.";
  return data;
}

auto OrderProvider::ReplaceOrder(const application::OrderReplaceRequest& request)
    -> application::OrderReplacementData {
  const auto cancel = CancelOrder(
      {request.replacement.account_id, request.order_id,
       request.replacement.confirm_live});
  const auto submission = SubmitOrder(request.replacement);

  application::OrderReplacementData data;
  data.replacement_id = submission.submission_id;
  data.replaced_order_id = request.order_id;
  data.account_id = submission.account_id;
  data.symbol = submission.symbol;
  data.asset_type = submission.asset_type;
  data.instruction = submission.instruction;
  data.quantity = submission.quantity;
  data.order_type = submission.order_type;
  data.limit_price = submission.limit_price;
  data.mode = submission.mode;
  data.status = submission.status;
  data.submitted_at = submission.submitted_at;
  data.message = cancel.message + " Replacement accepted.";
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

auto OrderProvider::NextId() -> unsigned long long {
  return ++g_order_counter;
}

}  // namespace premia::providers::local
