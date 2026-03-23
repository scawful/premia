#ifndef PREMIA_PROVIDERS_LOCAL_ORDER_PROVIDER_HPP
#define PREMIA_PROVIDERS_LOCAL_ORDER_PROVIDER_HPP

#include <string>

#include "premia/core/ports/provider_ports.hpp"

namespace premia::providers::local {

class OrderProvider : public core::ports::OrderProviderPort {
 public:
  explicit OrderProvider(std::string path);

  auto PreviewOrder(const core::application::OrderIntentRequest& request)
      -> core::application::OrderPreviewData override;
  auto SubmitOrder(const core::application::OrderIntentRequest& request)
      -> core::application::OrderSubmissionData override;
  auto CancelOrder(const core::application::OrderCancelRequest& request)
      -> core::application::OrderCancellationData override;
  auto ReplaceOrder(const core::application::OrderReplaceRequest& request)
      -> core::application::OrderReplacementData override;

 private:
  auto CurrentUtcTimestamp() const -> std::string;
  auto NextId() -> unsigned long long;

  std::string path_;
  unsigned long long counter_ = 0;
};

}  // namespace premia::providers::local

#endif  // PREMIA_PROVIDERS_LOCAL_ORDER_PROVIDER_HPP
