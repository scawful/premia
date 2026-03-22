#ifndef PREMIA_PROVIDERS_LOCAL_ACCOUNT_DETAIL_PROVIDER_HPP
#define PREMIA_PROVIDERS_LOCAL_ACCOUNT_DETAIL_PROVIDER_HPP

#include <string>

#include "premia/core/ports/provider_ports.hpp"

namespace premia::providers::local {

class AccountDetailProvider : public core::ports::AccountDetailProviderPort {
 public:
  explicit AccountDetailProvider(std::string path);

  auto GetAccountDetail() const -> core::application::AccountDetail override;

 private:
  std::string path_;
};

}  // namespace premia::providers::local

#endif  // PREMIA_PROVIDERS_LOCAL_ACCOUNT_DETAIL_PROVIDER_HPP
