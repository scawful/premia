#ifndef AccountModel_hpp
#define AccountModel_hpp

#include <vector>

#include "model/model.h"
#include "premia/core/application/screen_models.hpp"


namespace premia {
class AccountModel : public Model {
 private:
  Logger logger;
  bool loaded = false;
  core::application::PortfolioSummary portfolio_summary;
  core::application::AccountDetail account_detail;
  std::vector<core::application::ConnectionSummary> connections;

  public:
  void addLogger(const Logger& logger);
  void refresh();
  bool hasData() const;
  const core::application::PortfolioSummary& getPortfolioSummary() const;
  const core::application::AccountDetail& getAccountDetail() const;
  const std::vector<core::application::ConnectionSummary>& getConnections() const;
};
}  // namespace premia
#endif
