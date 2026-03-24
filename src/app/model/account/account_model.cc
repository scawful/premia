#include "account_model.h"

#include "premia/core/application/composition_root.hpp"

namespace premia {

void AccountModel::addLogger(const Logger& new_logger) { logger = new_logger; }

void AccountModel::setActiveAccountId(const std::string& account_id) {
  if (active_account_id_ == account_id) {
    return;
  }
  active_account_id_ = account_id;
  loaded = false;
}

void AccountModel::refresh() {
  auto& composition_root = core::application::CompositionRoot::Instance();
  portfolio_summary =
      composition_root.AppService().GetPortfolioSummaryForAccount(active_account_id_);
  account_detail =
      composition_root.AppService().GetAccountDetailForAccount(active_account_id_);
  connections = composition_root.AppService().GetConnections();
  loaded = true;
}

bool AccountModel::hasData() const { return loaded; }

const core::application::PortfolioSummary& AccountModel::getPortfolioSummary() const {
  return portfolio_summary;
}

const core::application::AccountDetail& AccountModel::getAccountDetail() const {
  return account_detail;
}

const std::vector<core::application::ConnectionSummary>& AccountModel::getConnections() const {
  return connections;
}

}  // namespace premia
