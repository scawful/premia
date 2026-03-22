#ifndef PREMIA_PROVIDERS_PLAID_WORKFLOW_PROVIDER_HPP
#define PREMIA_PROVIDERS_PLAID_WORKFLOW_PROVIDER_HPP

#include <string>

#include "premia/core/application/screen_models.hpp"
#include "premia/core/application/workflow_models.hpp"

namespace premia::providers::plaid {

class WorkflowProvider {
 public:
  WorkflowProvider(std::string config_path, std::string token_path);

  auto CreateLinkToken(const core::application::PlaidLinkTokenRequest& request,
                       const std::string& fallback_expiration,
                       unsigned long long fallback_id) const
      -> core::application::PlaidLinkTokenData;
  auto CompleteLink(const core::application::PlaidLinkCompleteRequest& request,
                    core::application::ConnectionSummary summary,
                    const std::string& completed_at) const
      -> core::application::ConnectionSummary;

 private:
  auto HasUsableConfig() const -> bool;

  std::string config_path_;
  std::string token_path_;
};

}  // namespace premia::providers::plaid

#endif  // PREMIA_PROVIDERS_PLAID_WORKFLOW_PROVIDER_HPP
