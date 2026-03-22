#include "premia/providers/plaid/workflow_provider.hpp"

#include <fstream>
#include <sstream>
#include <string>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Plaid/client.h"

namespace premia::providers::plaid {

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

auto ParseLinkTokenResponse(const std::string& response)
    -> application::PlaidLinkTokenData {
  application::PlaidLinkTokenData data;
  if (response.empty()) {
    return data;
  }

  try {
    std::istringstream ss(response);
    pt::ptree tree;
    pt::read_json(ss, tree);
    data.link_token = tree.get<std::string>("link_token", "");
    data.expiration = tree.get<std::string>("expiration", "");
  } catch (const std::exception&) {
  }
  return data;
}

}  // namespace

WorkflowProvider::WorkflowProvider(std::string config_path, std::string token_path)
    : config_path_(std::move(config_path)), token_path_(std::move(token_path)) {}

auto WorkflowProvider::HasUsableConfig() const -> bool {
  pt::ptree tree;
  if (!ReadConfigTree(config_path_, tree)) {
    return false;
  }

  const auto client_id = tree.get<std::string>("client_id", "");
  const auto secret = tree.get<std::string>("secret", "");
  return !IsPlaceholderValue(client_id) && !IsPlaceholderValue(secret);
}

auto WorkflowProvider::CreateLinkToken(
    const application::PlaidLinkTokenRequest& request,
    const std::string& fallback_expiration,
    unsigned long long fallback_id) const -> application::PlaidLinkTokenData {
  ::premia::plaid::Client client;
  if (HasUsableConfig() && client.LoadConfig(config_path_)) {
    client.LoadTokens(token_path_);
    const auto user_id = request.user_id.empty() ? std::string("premia-user")
                                                 : request.user_id;
    auto data = ParseLinkTokenResponse(client.CreateLinkToken(user_id));
    if (!data.link_token.empty()) {
      return data;
    }
  }

  application::PlaidLinkTokenData data;
  const auto user_id = request.user_id.empty() ? std::string("premia-user")
                                               : request.user_id;
  data.link_token = "link-sandbox-" + user_id + "-" + std::to_string(fallback_id);
  data.expiration = fallback_expiration;
  return data;
}

auto WorkflowProvider::CompleteLink(
    const application::PlaidLinkCompleteRequest& request,
    application::ConnectionSummary summary,
    const std::string& completed_at) const -> application::ConnectionSummary {
  ::premia::plaid::Client client;
  if (HasUsableConfig() && client.LoadConfig(config_path_)) {
    client.LoadTokens(token_path_);
    if (!request.public_token.empty() && client.ExchangePublicToken(request.public_token)) {
      client.SaveTokens(token_path_);
      summary.status = domain::ConnectionStatus::kConnected;
      summary.last_sync_at = completed_at;
      summary.reauth_required = false;
    } else if (!request.public_token.empty()) {
      summary.status = domain::ConnectionStatus::kNotConnected;
      summary.last_sync_at.clear();
      summary.reauth_required = false;
    }
    return summary;
  }

  if (!request.public_token.empty()) {
    summary.status = domain::ConnectionStatus::kConnected;
    summary.last_sync_at = completed_at;
    summary.reauth_required = false;
  }
  return summary;
}

}  // namespace premia::providers::plaid
