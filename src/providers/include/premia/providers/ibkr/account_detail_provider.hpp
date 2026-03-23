#ifndef PREMIA_PROVIDERS_IBKR_ACCOUNT_DETAIL_PROVIDER_HPP
#define PREMIA_PROVIDERS_IBKR_ACCOUNT_DETAIL_PROVIDER_HPP

#include <string>

#include "premia/core/ports/provider_ports.hpp"

namespace premia::providers::ibkr {

class AccountDetailProvider : public core::ports::AccountDetailProviderPort {
 public:
  explicit AccountDetailProvider(std::string config_path);

  auto GetAccountDetail() const -> core::application::AccountDetail override;

 private:
  std::string config_path_;
};

}  // namespace premia::providers::ibkr

#endif  // PREMIA_PROVIDERS_IBKR_ACCOUNT_DETAIL_PROVIDER_HPP
