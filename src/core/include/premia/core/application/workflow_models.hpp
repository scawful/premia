#ifndef PREMIA_CORE_APPLICATION_WORKFLOW_MODELS_HPP
#define PREMIA_CORE_APPLICATION_WORKFLOW_MODELS_HPP

#include <string>

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

}  // namespace premia::core::application

#endif  // PREMIA_CORE_APPLICATION_WORKFLOW_MODELS_HPP
