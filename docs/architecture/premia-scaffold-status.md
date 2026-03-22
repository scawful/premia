# Premia Scaffold Status

This note captures the first implementation pass after the architecture review.

## What now exists

### Shared core scaffold

- `src/core/CMakeLists.txt` now builds a concrete `premia_core` static library.
- `src/core/include/premia/core/domain/value_types.hpp` defines shared provider,
  connection, and money/value types.
- `src/core/include/premia/core/application/screen_models.hpp` defines
  normalized screen DTOs for bootstrap, home, watchlists, quotes, and charts.
- `src/core/include/premia/core/application/service_contracts.hpp` defines
  application-facing service seams.
- `src/core/include/premia/core/application/workflow_models.hpp` defines
  connection workflow DTOs for Schwab and Plaid.
- `src/core/include/premia/core/application/scaffold_application_service.hpp`
  and `src/core/application/scaffold_application_service.cc` provide concrete
  scaffold service implementations used by both the API and the desktop app.
- the scaffold service now attempts to use real Schwab and Plaid service clients
  when valid non-placeholder config files are present, and otherwise falls back
  to deterministic demo data
- quote and chart DTOs now try to hydrate from real Schwab market-data responses
  when a valid authorized Schwab session exists, and otherwise keep the same
  deterministic fallback output
- `src/core/include/premia/core/ports/provider_ports.hpp` defines provider-side
  adapter ports.

### Provider extraction start

- `src/providers/` now exists as a dedicated adapter layer.
- `src/providers/local/watchlist_provider.cc` serves normalized watchlists from
  `assets/watchlists.json` with a deterministic fallback if that file is
  missing or malformed.
- `src/providers/schwab/market_data_provider.cc` now owns the Schwab-specific
  quote and chart parsing logic that had previously lived inside the shared core
  service.
- watchlist summaries and watchlist-screen rows now come from a provider adapter
  backed by `assets/watchlists.json` instead of hardcoded rows inside the core
  service.

### API scaffold

- `apps/api/` now contains a buildable `premia_api` target.
- The scaffold now serves application DTOs via `premia_core` services for:
  - `GET /health`
  - `GET /v1/bootstrap`
  - `GET /v1/screens/home`
  - `GET /v1/watchlists`
  - `GET /v1/screens/watchlists/{watchlistId}`
  - `GET /v1/screens/quotes/{symbol}`
  - `GET /v1/screens/charts/{symbol}`
  - `GET /v1/stream/events`
- Schwab and Plaid workflow routes are now implemented:
  - `POST /v1/connections/schwab/oauth/start`
  - `POST /v1/connections/schwab/oauth/complete`
  - `POST /v1/connections/plaid/link-token`
  - `POST /v1/connections/plaid/link-complete`
- these workflow routes now use real provider clients when valid config exists,
  and otherwise preserve a safe local scaffold fallback for development
- `GET /openapi` exposes the checked-in contract file.

### Swift client and UI scaffold

- `clients/swift/openapi-generator-config.yaml` captures generator defaults.
- `clients/swift/Generated/` now contains the first generated Swift client.
- `apps/mobile-ios/Package.swift` defines `PremiaModels`, `PremiaAPIClient`, and
  `PremiaUI` library targets.
- Placeholder Swift models, client configuration, and a SwiftUI
  `ConnectionStatusBadge` now exist.
- `apps/mobile-ios/Package.swift` now depends on the local generated package and
  `apps/mobile-ios/Sources/PremiaAPIClient/PremiaAPIClient.swift` wraps it with
  a first typed async `loadBootstrap()` method.

### Desktop migration start

- `src/app/view/chart/chart_view.cc` now renders a `premia_core` contract preview
  using `ScaffoldApplicationService`, so one active desktop screen is consuming
  normalized core DTOs instead of only broker-facing model code.
- `src/app/view/account/account_view.cc` now shows a `premia_core` fallback
  account/portfolio preview when the legacy TDA-driven account pane is not
  available, giving the desktop app a second active screen that can render
  normalized core contracts.
- `src/app/view/watchlist/watchlist_view.cc` now shows a `premia_core` fallback
  watchlist table when the legacy local or TDA-driven watchlist path is not
  available, so normalized contracts cover another active desktop surface.

## Validation status

- `cmake -S . -B build-arch-next` succeeds.
- `cmake --build build-arch-next --target premia_api` succeeds.
- `cmake --build build-arch-next --target premia` succeeds.
- `premia_api` screen endpoints and connection workflow routes return valid JSON
  with correct booleans and arrays.
- placeholder config files do not trigger live provider calls; bootstrap now
  reports providers as disconnected unless real credentials or workflow state
  make them active
- `swift package dump-package` in `apps/mobile-ios/` succeeds.
- `swift package dump-package` still succeeds in `apps/mobile-ios/` after adding
  the local generated-client dependency.
- `npx --yes @openapitools/openapi-generator-cli generate ...` succeeds and emits
  the first generated Swift client package.
- `swift build` in `apps/mobile-ios/` currently fails because the local machine
  has conflicting `/usr/local/include` module maps shadowing Apple SDK modules.
  That appears to be an environment issue, not a package-manifest issue.
- `swift package dump-package` in `clients/swift/Generated/` fails for the same
  environment reason after generation, not because generation itself failed.

## Immediate next moves

- replace scaffold service data with real provider-backed application services
- add request/response examples into `contracts/openapi/premia-v1.yaml`
- bridge `apps/mobile-ios/` onto the generated client package
- migrate another active desktop surface from singleton/provider calls to core
  service contracts
