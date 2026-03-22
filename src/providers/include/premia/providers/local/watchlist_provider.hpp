#ifndef PREMIA_PROVIDERS_LOCAL_WATCHLIST_PROVIDER_HPP
#define PREMIA_PROVIDERS_LOCAL_WATCHLIST_PROVIDER_HPP

#include <string>
#include <vector>

#include "premia/core/ports/provider_ports.hpp"

namespace premia::providers::local {

class WatchlistProvider : public core::ports::WatchlistProviderPort,
                          public core::ports::WatchlistMutationProviderPort {
 public:
  explicit WatchlistProvider(std::string path);

  auto ListWatchlists() const -> std::vector<core::application::WatchlistSummary> override;
  auto GetWatchlistScreen(const std::string& watchlist_id) const
      -> core::application::WatchlistScreenData override;
  auto CreateWatchlist(const std::string& name) -> core::application::WatchlistSummary override;
  auto RenameWatchlist(const std::string& watchlist_id,
                       const std::string& name) -> core::application::WatchlistSummary override;
  auto AddWatchlistSymbol(const std::string& watchlist_id,
                          const std::string& symbol) -> core::application::WatchlistSummary override;
  auto RemoveWatchlistSymbol(const std::string& watchlist_id,
                             const std::string& symbol) -> core::application::WatchlistSummary override;

 private:
  auto BuildFallbackData() const -> std::vector<core::application::WatchlistScreenData>;
  auto LoadData() const -> std::vector<core::application::WatchlistScreenData>;
  auto SaveData(const std::vector<core::application::WatchlistScreenData>& screens) const
      -> void;

  std::string path_;
};

}  // namespace premia::providers::local

#endif  // PREMIA_PROVIDERS_LOCAL_WATCHLIST_PROVIDER_HPP
