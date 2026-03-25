#ifndef PREMIA_PROVIDERS_LOCAL_ORDER_TEMPLATE_PROVIDER_HPP
#define PREMIA_PROVIDERS_LOCAL_ORDER_TEMPLATE_PROVIDER_HPP

#include <string>
#include <vector>

#include "premia/core/ports/provider_ports.hpp"

namespace premia::providers::local {

// Persists order templates as JSON in a local file (assets/order_templates.json).
// Each template is stored with a stable id generated at creation time.
class OrderTemplateProvider : public core::ports::OrderTemplateProviderPort {
 public:
  explicit OrderTemplateProvider(std::string path);

  auto ListTemplates() const
      -> std::vector<core::application::OrderTemplate> override;
  auto CreateTemplate(const core::application::OrderTemplate& tmpl)
      -> core::application::OrderTemplate override;
  auto UpdateTemplate(const std::string& id,
                      const core::application::OrderTemplate& tmpl)
      -> core::application::OrderTemplate override;
  auto DeleteTemplate(const std::string& id)
      -> core::application::OrderTemplate override;

 private:
  auto CurrentUtcTimestamp() const -> std::string;

  std::string path_;
};

}  // namespace premia::providers::local

#endif  // PREMIA_PROVIDERS_LOCAL_ORDER_TEMPLATE_PROVIDER_HPP
