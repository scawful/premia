#ifndef PREMIA_PROVIDERS_SCHWAB_WORKFLOW_PROVIDER_HPP
#define PREMIA_PROVIDERS_SCHWAB_WORKFLOW_PROVIDER_HPP

#include <string>

#include "premia/core/application/screen_models.hpp"
#include "premia/core/application/workflow_models.hpp"

namespace premia::providers::schwab {

class WorkflowProvider {
 public:
  WorkflowProvider(std::string config_path, std::string token_path);

  auto StartOAuth(const core::application::SchwabOAuthStartRequest& request,
                  const std::string& state,
                  const std::string& expires_at) const
      -> core::application::SchwabOAuthStartData;
  auto CompleteOAuth(
      const core::application::SchwabOAuthCompleteRequest& request,
      core::application::ConnectionSummary summary,
      const std::string& completed_at) const
      -> core::application::ConnectionSummary;

 private:
  auto HasUsableConfig() const -> bool;

  std::string config_path_;
  std::string token_path_;
};

}  // namespace premia::providers::schwab

#endif  // PREMIA_PROVIDERS_SCHWAB_WORKFLOW_PROVIDER_HPP
