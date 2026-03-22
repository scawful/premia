#ifndef PREMIA_PROVIDERS_LOCAL_OPTIONS_PROVIDER_HPP
#define PREMIA_PROVIDERS_LOCAL_OPTIONS_PROVIDER_HPP

#include <string>

#include "premia/core/ports/provider_ports.hpp"

namespace premia::providers::local {

class OptionsProvider : public core::ports::OptionsProviderPort {
 public:
  explicit OptionsProvider(std::string path);

  auto GetOptionChainSnapshot(const std::string& symbol,
                              const std::string& strike_count,
                              const std::string& strategy,
                              const std::string& range,
                              const std::string& exp_month,
                              const std::string& option_type) const
      -> core::application::OptionChainSnapshot override;

 private:
  std::string path_;
};

}  // namespace premia::providers::local

#endif  // PREMIA_PROVIDERS_LOCAL_OPTIONS_PROVIDER_HPP
