#ifndef PREMIA_PROVIDERS_SCHWAB_ORDER_PROVIDER_HPP
#define PREMIA_PROVIDERS_SCHWAB_ORDER_PROVIDER_HPP

#include <string>

#include "premia/core/ports/provider_ports.hpp"

namespace premia::providers::schwab {

class OrderProvider : public core::ports::OrderProviderPort {
 public:
  OrderProvider(std::string config_path, std::string token_path);

  auto PreviewOrder(const core::application::OrderIntentRequest& request)
      -> core::application::OrderPreviewData override;
  auto SubmitOrder(const core::application::OrderIntentRequest& request)
      -> core::application::OrderSubmissionData override;
  auto CancelOrder(const core::application::OrderCancelRequest& request)
      -> core::application::OrderCancellationData override;
  auto ReplaceOrder(const core::application::OrderReplaceRequest& request)
      -> core::application::OrderReplacementData override;
  auto GetOpenOrders(const std::string& account_id) const
      -> std::vector<core::application::OrderRecordData> override;
  auto GetOrderHistory(const std::string& account_id) const
      -> std::vector<core::application::OrderRecordData> override;

 private:
  auto CurrentUtcTimestamp() const -> std::string;

  std::string config_path_;
  std::string token_path_;
};

}  // namespace premia::providers::schwab

#endif  // PREMIA_PROVIDERS_SCHWAB_ORDER_PROVIDER_HPP
