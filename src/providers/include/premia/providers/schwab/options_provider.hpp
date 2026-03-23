#ifndef PREMIA_PROVIDERS_SCHWAB_OPTIONS_PROVIDER_HPP
#define PREMIA_PROVIDERS_SCHWAB_OPTIONS_PROVIDER_HPP

#include <string>

#include "premia/core/ports/provider_ports.hpp"

namespace premia::providers::schwab {

class OptionsProvider : public core::ports::OptionsProviderPort {
 public:
  OptionsProvider(std::string config_path, std::string token_path);

  auto GetOptionChainSnapshot(const std::string& symbol,
                              const std::string& strike_count,
                              const std::string& strategy,
                              const std::string& range,
                              const std::string& exp_month,
                              const std::string& option_type) const
      -> core::application::OptionChainSnapshot override;

 private:
  std::string config_path_;
  std::string token_path_;
};

}  // namespace premia::providers::schwab

#endif  // PREMIA_PROVIDERS_SCHWAB_OPTIONS_PROVIDER_HPP
