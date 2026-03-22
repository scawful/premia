#ifndef PREMIA_PROVIDERS_TDA_ORDER_PROVIDER_HPP
#define PREMIA_PROVIDERS_TDA_ORDER_PROVIDER_HPP

#include <string>

#include "premia/core/ports/provider_ports.hpp"

namespace premia::tda {
class Client;
}

namespace premia::providers::tda {

class OrderProvider : public core::ports::OrderProviderPort {
 public:
  explicit OrderProvider(std::string config_path);

  auto PreviewOrder(const core::application::OrderIntentRequest& request)
      -> core::application::OrderPreviewData override;
  auto SubmitOrder(const core::application::OrderIntentRequest& request)
      -> core::application::OrderSubmissionData override;

 private:
  auto CurrentUtcTimestamp() const -> std::string;
  auto ResolveAccountId(::premia::tda::Client& client,
                        const std::string& requested_account_id) const
      -> std::string;
  auto BuildPayload(const core::application::OrderIntentRequest& request,
                    const std::string& account_id) const -> std::string;
  auto HasUsableConfig() const -> bool;

  std::string config_path_;
};

}  // namespace premia::providers::tda

#endif  // PREMIA_PROVIDERS_TDA_ORDER_PROVIDER_HPP
