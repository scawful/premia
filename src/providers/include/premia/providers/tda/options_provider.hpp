#ifndef PREMIA_PROVIDERS_TDA_OPTIONS_PROVIDER_HPP
#define PREMIA_PROVIDERS_TDA_OPTIONS_PROVIDER_HPP

#include <string>

#include "premia/core/ports/provider_ports.hpp"

namespace premia::providers::tda {

class OptionsProvider : public core::ports::OptionsProviderPort {
 public:
  explicit OptionsProvider(std::string config_path);

  auto GetOptionChainSnapshot(const std::string& symbol,
                              const std::string& strike_count,
                              const std::string& strategy,
                              const std::string& range,
                              const std::string& exp_month,
                              const std::string& option_type) const
      -> core::application::OptionChainSnapshot override;

 private:
  std::string config_path_;
};

}  // namespace premia::providers::tda

#endif  // PREMIA_PROVIDERS_TDA_OPTIONS_PROVIDER_HPP
