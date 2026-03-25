#ifndef PREMIA_CORE_APPLICATION_WORKFLOW_MODELS_HPP
#define PREMIA_CORE_APPLICATION_WORKFLOW_MODELS_HPP

#include <string>
#include <vector>

namespace premia::core::application {

struct SchwabOAuthStartRequest {
  std::string redirect_uri;
  std::string client_platform;
};

struct SchwabOAuthStartData {
  std::string auth_url;
  std::string state;
  std::string expires_at;
};

struct SchwabOAuthCompleteRequest {
  std::string callback;
  std::string state;
};

struct PlaidLinkTokenRequest {
  std::string user_id;
  std::string redirect_uri;
};

struct PlaidLinkTokenData {
  std::string link_token;
  std::string expiration;
};

struct PlaidLinkCompleteRequest {
  std::string public_token;
  std::string institution_id;
};

struct OrderIntentRequest {
  std::string account_id;
  std::string symbol;
  std::string asset_type;
  std::string instruction;
  std::string quantity;
  std::string order_type;
  std::string limit_price;
  std::string duration;
  std::string session;
  bool confirm_live = false;
};

struct OrderPreviewData {
  std::string preview_id;
  std::string account_id;
  std::string symbol;
  std::string asset_type;
  std::string instruction;
  std::string quantity;
  std::string order_type;
  std::string limit_price;
  std::string estimated_total;
  std::string mode;
  std::string status;
  std::vector<std::string> warnings;
};

struct OrderSubmissionData {
  std::string submission_id;
  std::string account_id;
  std::string symbol;
  std::string asset_type;
  std::string instruction;
  std::string quantity;
  std::string order_type;
  std::string limit_price;
  std::string mode;
  std::string status;
  std::string submitted_at;
  std::string message;
};

struct OrderCancelRequest {
  std::string account_id;
  std::string order_id;
  bool confirm_live = false;
};

struct OrderCancellationData {
  std::string order_id;
  std::string account_id;
  std::string mode;
  std::string status;
  std::string cancelled_at;
  std::string message;
};

struct OrderReplaceRequest {
  std::string order_id;
  OrderIntentRequest replacement;
};

struct OrderReplacementData {
  std::string replacement_id;
  std::string replaced_order_id;
  std::string account_id;
  std::string symbol;
  std::string asset_type;
  std::string instruction;
  std::string quantity;
  std::string order_type;
  std::string limit_price;
  std::string mode;
  std::string status;
  std::string submitted_at;
  std::string message;
};

struct OrderRecordData {
  std::string order_id;
  std::string account_id;
  std::string symbol;
  std::string asset_type;
  std::string instruction;
  std::string quantity;
  std::string order_type;
  std::string limit_price;
  std::string mode;
  std::string status;
  std::string submitted_at;
  std::string updated_at;
  std::string message;
};

// An order template stores reusable parameters for fast order entry from a
// watchlist symbol. symbol is optional — if empty the template applies to any
// symbol. quantity may represent shares ("10") or a dollar amount ("500.00")
// depending on is_dollar_amount.
struct OrderTemplate {
  std::string id;
  std::string name;
  std::string symbol;           // optional; empty means symbol-agnostic
  std::string order_type;       // "MARKET", "LIMIT"
  std::string action;           // "BUY", "SELL"
  std::string quantity;         // shares or dollar amount as decimal string
  bool is_dollar_amount = false;
  std::string time_in_force;    // "DAY", "GTC"
  std::string session;          // "NORMAL", "EXTENDED"
  std::string asset_type;       // "EQUITY", "OPTION"
  std::string provider_preference; // optional, e.g. "schwab"
  std::string created_at;
  std::string updated_at;
};

struct QuickTradePreviewRequest {
  std::string symbol;
  std::string template_id;
  std::string account_id;  // optional
  bool confirm_live = false;
};

}  // namespace premia::core::application

#endif  // PREMIA_CORE_APPLICATION_WORKFLOW_MODELS_HPP
