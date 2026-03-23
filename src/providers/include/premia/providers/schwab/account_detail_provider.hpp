#ifndef PREMIA_PROVIDERS_SCHWAB_ACCOUNT_DETAIL_PROVIDER_HPP
#define PREMIA_PROVIDERS_SCHWAB_ACCOUNT_DETAIL_PROVIDER_HPP

#include <string>

#include "premia/core/ports/provider_ports.hpp"

namespace premia::providers::schwab {

class AccountDetailProvider : public core::ports::AccountDetailProviderPort {
 public:
  AccountDetailProvider(std::string config_path, std::string token_path);

  auto GetAccountDetail() const -> core::application::AccountDetail override;

 private:
  std::string config_path_;
  std::string token_path_;
};

}  // namespace premia::providers::schwab

#endif  // PREMIA_PROVIDERS_SCHWAB_ACCOUNT_DETAIL_PROVIDER_HPP
