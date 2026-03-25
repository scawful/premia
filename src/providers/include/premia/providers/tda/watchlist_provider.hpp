#ifndef PREMIA_PROVIDERS_TDA_WATCHLIST_PROVIDER_HPP
#define PREMIA_PROVIDERS_TDA_WATCHLIST_PROVIDER_HPP

#include <string>
#include <vector>

#include "premia/core/ports/provider_ports.hpp"

namespace premia::providers::tda {

class WatchlistProvider : public core::ports::WatchlistProviderPort,
                          public core::ports::WatchlistMutationProviderPort {
 public:
  WatchlistProvider(std::string config_path, std::string overlay_path);

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
  auto PinWatchlistSymbol(const std::string& watchlist_id,
                          const std::string& symbol,
                          bool pinned) -> core::application::WatchlistSummary override;
  auto MoveWatchlistSymbol(const std::string& watchlist_id,
                           const std::string& symbol,
                           const std::string& before_symbol)
      -> core::application::WatchlistSummary override;
  auto ArchiveWatchlist(const std::string& watchlist_id, bool archived)
      -> core::application::WatchlistSummary override;
  auto DeleteWatchlist(const std::string& watchlist_id)
      -> core::application::WatchlistSummary override;
  auto MoveSymbolToWatchlist(const std::string& source_watchlist_id,
                             const std::string& destination_watchlist_id,
                             const std::string& symbol)
      -> core::application::WatchlistSummary override;

 private:
  auto LoadScreens() const -> std::vector<core::application::WatchlistScreenData>;
  auto LoadRemoteScreens() const -> std::vector<core::application::WatchlistScreenData>;
  auto HasUsableConfig() const -> bool;
  auto LoadOverlayScreens() const -> std::vector<core::application::WatchlistScreenData>;
  auto SaveOverlayScreens(
      const std::vector<core::application::WatchlistScreenData>& screens) const -> void;
  auto BuildPayload(const core::application::WatchlistScreenData& screen) const -> std::string;
  auto SyncOverlayWithRemote(
      const std::vector<core::application::WatchlistScreenData>& remote_screens) const
      -> std::vector<core::application::WatchlistScreenData>;
  auto ResolveWatchlist(const std::string& watchlist_id) const
      -> core::application::WatchlistScreenData;
  auto FindAccountId() const -> std::string;

  std::string config_path_;
  std::string overlay_path_;
};

}  // namespace premia::providers::tda

#endif  // PREMIA_PROVIDERS_TDA_WATCHLIST_PROVIDER_HPP
