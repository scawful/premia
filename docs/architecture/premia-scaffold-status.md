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
  and `src/core/application/scaffold_application_service.cc` now define the
  `ProviderBackedApplicationService` used by both the API and the desktop app.
- `src/core/include/premia/core/application/composition_root.hpp` and
  `src/core/application/composition_root.cc` now provide a simple composition
  root that assembles and exposes the provider-backed application service.
- `src/core/application/provider_service_components.hpp` and
  `src/core/application/provider_service_components.cc` now split connection,
  portfolio/account, market/options, watchlist, and workflow logic into smaller
  composed services behind the provider-backed application service.
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
- `src/providers/local/portfolio_provider.cc` serves normalized portfolio and
  holdings data from `assets/portfolio.json` with a deterministic fallback.
- `src/providers/local/account_detail_provider.cc` serves normalized account
  detail and positions data from `assets/account.json` with a deterministic
  fallback.
- `src/providers/local/options_provider.cc` serves normalized option-chain
  snapshot data from `assets/options.json` with a deterministic fallback.
- `src/providers/local/watchlist_provider.cc` serves normalized watchlists from
  `assets/watchlists.json` with a deterministic fallback if that file is
  missing or malformed.
- `src/providers/tda/portfolio_provider.cc` can now supply authenticated TDA
  portfolio summary and holdings data when a valid `assets/tda.json` config is
  present.
- `src/providers/tda/account_detail_provider.cc` can now supply authenticated
  TDA account detail and positions data.
- `src/providers/tda/watchlist_provider.cc` can now supply authenticated TDA
  watchlists when a valid `assets/tda.json` config is present.
- `src/providers/tda/options_provider.cc` can now supply authenticated TDA
  option-chain snapshots.
- `src/providers/local/order_provider.cc` and `src/providers/tda/order_provider.cc`
  now back order preview and submission workflows through the shared contract
  surface.
- those order providers now also support cancel and replace flows through the
  same provider-backed contract surface.
- those order providers now also support open-order and order-history queries
  through the same provider-backed contract surface.
- `src/providers/schwab/market_data_provider.cc` now owns the Schwab-specific
  quote and chart parsing logic that had previously lived inside the shared core
  service.
- `src/providers/schwab/workflow_provider.cc` and
  `src/providers/plaid/workflow_provider.cc` now own the broker-specific
  workflow logic for Schwab OAuth and Plaid Link.
- watchlist summaries and watchlist-screen rows now come from a provider adapter
  backed by `assets/watchlists.json`, with TDA taking precedence when an
  authenticated watchlist provider is available.
- portfolio summary and top holdings now come from provider adapters as well,
  with TDA taking precedence over the local file-backed portfolio fallback.
- account detail and option-chain fallback views are now backed by provider
  adapters too, letting more of the desktop UI leave singleton-driven model
  flows behind.
- account/options flows are no longer read-only at the API layer; order preview
  and submission routes now exist with safe local fallbacks and explicit live
  confirmation requirements for TDA.
- watchlist create/rename/add-symbol/remove-symbol flows are now supported
  through the local watchlist provider and the HTTP API mutation routes.

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
- the mobile wrapper now also exposes typed async `loadHome()`, `loadQuote()`,
  and `loadChart()` methods, plus hand-written Swift models for portfolio,
  holdings, quotes, and chart snapshots.
- the mobile wrapper now also exposes typed async watchlist and connection
  workflow methods, with hand-written models for watchlist rows/screens and
  Schwab/Plaid launch states.
- the mobile wrapper now also exposes watchlist mutation helpers and maps
  generated request failures into a normalized `PremiaAPIClientError` model for
  SwiftUI error-state handling.
- the mobile wrapper now also exposes typed account-detail and option-chain
  loaders on top of the generated client.
- option-chain filter/search state now has a dedicated hand-written Swift query
  model so SwiftUI controls can bind to one app-level type.
- the mobile wrapper now also exposes typed order preview/submission models and
  methods, so trading flows can start from app-level Swift types too.
- the mobile wrapper now also exposes typed cancel/replace order flows on top of
  the generated trading API.
- the mobile wrapper now also exposes order-list/history loaders and a simple
  SwiftUI `OrdersListView` for rendering provider-backed order records.
- `apps/mobile-ios/Sources/PremiaUI/` now contains a real tab-based SwiftUI
  navigation shell with home, account, watchlists, quote/chart, and orders
  screens built on top of the wrapper layer.
- `PremiaMobile/` now contains an XcodeGen-backed iOS app host that launches
  `PremiaRootView` as a real Simulator-buildable app target.

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
- `src/app/model/account/account_model.cc` and
  `src/app/model/options/options_model.cc` now use composition-root services
  instead of calling broker singletons directly, so more of the desktop model
  layer now depends on normalized provider-backed application contracts.
- `src/app/view/account/account_view.cc` now prefers the provider-backed core
  account pane and no longer keeps the legacy TDA account pane in the active
  desktop UI path.
- `src/app/view/options/option_chain.cc` now renders option rows and underlying
  details from normalized core snapshots instead of directly reading the old
  TDA-style raw option maps in the view layer.
- `src/app/model/chart/chart_model.cc` and `src/app/view/watchlist/watchlist_view.cc`
  now avoid direct TDA singleton usage on their active paths, further shifting
  desktop behavior onto provider-backed core contracts.
- the active `src/app/` desktop paths no longer depend on direct
  `tda::TDA::getInstance()` calls; the old singleton usage has been removed from
  login, console, and legacy watchlist model paths as well.
- the chart and watchlist desktop paths now stay on provider-backed core data
  rather than rebuilding active rendering around direct TDA singleton lookups.
- unused singleton-era app facades and dead desktop models have been deleted,
  including the old app-local `TDA.hpp`, `Schwab.hpp`, `Plaid.hpp`,
  `ConsoleModel`, and `WatchlistModel` files.

## Validation status

- `cmake -S . -B build-arch-next` succeeds.
- `cmake --build build-arch-next --target premia_api` succeeds.
- `cmake --build build-arch-next --target premia` succeeds.
- `premia_api` screen endpoints and connection workflow routes return valid JSON
  with correct booleans and arrays.
- `premia_api` now also serves account and option-chain screen payloads through
  the same provider-backed core service layer.
- placeholder config files do not trigger live provider calls; bootstrap now
  reports providers as disconnected unless real credentials or workflow state
  make them active
- `swift package dump-package` in `apps/mobile-ios/` succeeds.
- `clients/swift/Generated/` has been migrated from the deprecated `swift5`
  generator to the `swift6` generator.
- `swift package dump-package` still succeeds in `apps/mobile-ios/` after adding
  the local generated-client dependency.
- `npx --yes @openapitools/openapi-generator-cli generate ...` succeeds and emits
  the first generated Swift client package.
- `swift build` in `apps/mobile-ios/` now succeeds after moving the conflicting
  SDK-shadow headers and modulemaps from `/usr/local/include` into
  `~/.local/share/premia/swift-sdk-shadow-backup/`.
- `swift package dump-package` in `clients/swift/Generated/` succeeds with the
  Swift 6 generator output.
- `xcodebuild -project PremiaMobile/PremiaMobile.xcodeproj -scheme PremiaMobile -destination 'generic/platform=iOS Simulator' CODE_SIGNING_ALLOWED=NO build` succeeds.
- `./build-next-providers/bin/premia_provider_core_test` passes provider-backed
  order fallback and guardrail tests.
- `./build-next-providers/bin/premia_api_integration_test` now exercises the
  provider-backed account, options, and trading/history HTTP routes through the
  real `premia_api` binary against isolated fixture workspaces.
- `swift test --package-path apps/mobile-ios` passes Swift model and SwiftUI
  smoke tests.

## Immediate next moves

- replace scaffold service data with real provider-backed application services
- add request/response examples into `contracts/openapi/premia-v1.yaml`
- bridge `apps/mobile-ios/` onto the generated client package
- migrate another active desktop surface from singleton/provider calls to core
  service contracts
