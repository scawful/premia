#include "premia/providers/schwab/workflow_provider.hpp"

#include <fstream>
#include <string>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Schwab/client.h"

namespace premia::providers::schwab {

namespace application = premia::core::application;
namespace domain = premia::core::domain;
namespace pt = boost::property_tree;

namespace {

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

WorkflowProvider::WorkflowProvider(std::string config_path, std::string token_path)
    : config_path_(std::move(config_path)), token_path_(std::move(token_path)) {}

auto WorkflowProvider::HasUsableConfig() const -> bool {
  pt::ptree tree;
  if (!ReadConfigTree(config_path_, tree)) {
    return false;
  }

  const auto app_key = tree.get<std::string>("app_key", "");
  const auto app_secret = tree.get<std::string>("app_secret", "");
  return !IsPlaceholderValue(app_key) && !IsPlaceholderValue(app_secret);
}

auto WorkflowProvider::StartOAuth(const application::SchwabOAuthStartRequest& request,
                                  const std::string& state,
                                  const std::string& expires_at) const
    -> application::SchwabOAuthStartData {
  application::SchwabOAuthStartData data;
  data.state = state;
  data.expires_at = expires_at;

  ::premia::schwab::Client client;
  if (HasUsableConfig() && client.LoadConfig(config_path_)) {
    data.auth_url = client.BuildAuthUrl();
    return data;
  }

  const auto redirect_uri = request.redirect_uri.empty()
                                ? std::string("premia://schwab/callback")
                                : request.redirect_uri;
  data.auth_url =
      "https://api.schwabapi.com/v1/oauth/authorize?response_type=code&client_id="
      "premia-demo-app&redirect_uri=" +
      redirect_uri + "&state=" + state;
  return data;
}

auto WorkflowProvider::CompleteOAuth(
    const application::SchwabOAuthCompleteRequest& request,
    application::ConnectionSummary summary,
    const std::string& completed_at) const -> application::ConnectionSummary {
  ::premia::schwab::Client client;
  if (HasUsableConfig() && client.LoadConfig(config_path_)) {
    client.LoadTokens(token_path_);
    if (!request.callback.empty() && client.ExchangeAuthCode(request.callback)) {
      client.SaveTokens(token_path_);
      client.GetAccountNumbers();
      summary.status = domain::ConnectionStatus::kConnected;
      summary.last_sync_at = completed_at;
      summary.reauth_required = false;
    } else if (!request.callback.empty()) {
      summary.status = domain::ConnectionStatus::kReauthRequired;
      summary.reauth_required = true;
    }
    return summary;
  }

  if (!request.callback.empty()) {
    summary.status = domain::ConnectionStatus::kConnected;
    summary.last_sync_at = completed_at;
    summary.reauth_required = false;
  }
  return summary;
}

}  // namespace premia::providers::schwab
