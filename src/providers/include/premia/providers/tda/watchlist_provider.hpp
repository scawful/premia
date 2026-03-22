#ifndef PREMIA_PROVIDERS_TDA_WATCHLIST_PROVIDER_HPP
#define PREMIA_PROVIDERS_TDA_WATCHLIST_PROVIDER_HPP

#include <string>
#include <vector>

#include "premia/core/ports/provider_ports.hpp"

namespace premia::providers::tda {

class WatchlistProvider : public core::ports::WatchlistProviderPort {
 public:
  explicit WatchlistProvider(std::string config_path);

  auto ListWatchlists() const -> std::vector<core::application::WatchlistSummary> override;
  auto GetWatchlistScreen(const std::string& watchlist_id) const
      -> core::application::WatchlistScreenData override;

 private:
  auto LoadScreens() const -> std::vector<core::application::WatchlistScreenData>;
  auto HasUsableConfig() const -> bool;

  std::string config_path_;
};

}  // namespace premia::providers::tda

#endif  // PREMIA_PROVIDERS_TDA_WATCHLIST_PROVIDER_HPP
