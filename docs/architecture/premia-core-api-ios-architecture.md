# Premia Core + API + iOS Architecture

## Why this split

Premia currently mixes UI code, singleton app state, broker client logic,
provider-specific parsing, and token handling inside the desktop application.
That makes it hard to safely add Schwab, Plaid, Interactive Brokers, or a real
iOS app without duplicating logic and leaking secrets into clients.

The recommended target is a layered platform:
- `Premia Core` owns typed domain models, application services, and provider
  ports.
- `Premia API` owns auth flows, broker orchestration, token refresh, provider
  normalization, caching, and streaming.
- `Premia Desktop` becomes a client of shared application contracts instead of
  the source of truth.
- `Premia Mobile` uses SwiftUI over the same screen-oriented DTOs.

## Layer responsibilities

### Premia Core

Scope:
- Domain entities and value objects.
- Application services and use cases.
- Provider-agnostic ports and storage interfaces.
- DTO composition for screen and workflow responses.

Should not depend on:
- SDL, ImGui, SwiftUI, curl, broker JSON payloads, singleton UI state.

Core model families:
- `auth`: sessions, connection state, reauth requirements.
- `portfolio`: accounts, balances, holdings, buying power, performance.
- `market`: quotes, candles, market sessions, search results.
- `watchlist`: watchlists, watchlist items, quote hydration.
- `options`: expirations, chain slices, contract summaries.
- `banking`: institutions, linked accounts, transactions, sync cursors.
- `shared`: money, timestamps, paging, provider metadata, error envelopes.

### Premia API

Scope:
- OAuth start/complete flows for Schwab and future providers.
- Plaid link token creation and public token exchange.
- Provider normalization into app-facing DTOs.
- Caching, retries, rate limiting, background refresh, and stream fan-out.
- Security boundaries for secrets, tokens, and audit logging.

Transport recommendation:
- Start with REST + OpenAPI for external app clients.
- Add websocket or SSE for quotes, connection changes, and invalidation events.
- Keep gRPC internal-only if it becomes useful later.

### Premia Desktop

Scope:
- SDL/ImGui rendering and workstation workflows.
- Local command/debug tools when needed.
- No direct broker singletons in view code after migration.

Desktop should move from direct provider usage toward:
- presenter/view-model classes
- application service interfaces
- DTO-driven screen state

### Premia Mobile

Scope:
- SwiftUI screens and reusable components.
- A generated or hand-written client built from the Premia API contract.
- Local secure storage only for app session state and safe identifiers.

The mobile app should not:
- own broker secrets
- exchange OAuth codes directly with providers
- parse broker-native payloads

## Architectural rules

- UI layers only depend on application-facing contracts.
- Provider adapters never talk directly to UI.
- Raw provider JSON stops at adapter boundaries.
- Every public payload has stable IDs, timestamps, optionality, and freshness.
- Money and decimal values cross the API as strings.
- Provider differences are normalized into enums and capability flags.

## Authentication strategy

### Schwab and OAuth providers

Recommended flow:
1. Client asks Premia API to start auth.
2. API returns auth URL plus state.
3. User completes provider consent in browser or native web auth session.
4. Client sends callback URL or auth code to Premia API.
5. Premia API exchanges and stores tokens securely.
6. Premia API returns normalized connection status.

### Plaid

Recommended flow:
1. Client asks Premia API for a link token.
2. Client presents Plaid Link.
3. Client sends the resulting public token to Premia API.
4. Premia API exchanges and stores the access token.
5. Premia API syncs linked institutions/accounts and returns normalized data.

### IBKR

Recommended first step:
- Keep Interactive Brokers read-only until the sample-code integration is
  stripped down and explicit account workflows are defined.

## Screen-oriented service boundaries

These services should define the shared application surface:
- `AuthService`
- `BrokerConnectionService`
- `PortfolioService`
- `MarketDataService`
- `WatchlistService`
- `OptionsService`
- `BankLinkService`

Examples of application-facing DTOs:
- `ConnectionSummary`
- `PortfolioSummary`
- `AccountDetail`
- `HoldingRow`
- `WatchlistSummary`
- `WatchlistDetail`
- `QuoteDetail`
- `ChartSeries`
- `OptionChainSlice`
- `LinkedInstitution`
- `TransactionPage`

## Streaming model

Initial live event families:
- `quote.updated`
- `portfolio.invalidated`
- `watchlist.updated`
- `connection.changed`
- `session.reauth_required`

Clients should treat streams as invalidation and incremental update channels,
not as a replacement for screen bootstrapping endpoints.

## Caching and sync

Premia API should own:
- quote cache with freshness metadata
- chart window cache
- option chain cache
- linked institution/account cache
- Plaid transaction cursors and pagination

Clients should own:
- short-lived view caches for recent screens
- optimistic watchlist state when safe
- durable app session state in platform-secure storage

## Directory layout target

```text
premia/
  apps/
    api/
    desktop/
    mobile-ios/
  contracts/
    openapi/
  src/
    core/
    providers/
    infrastructure/
  clients/
    swift/
  docs/
    architecture/
  test/
    core/
    providers/
    api/
    contracts/
    integration/
```

## Immediate success criteria

The architecture is working when:
- the desktop app no longer authenticates directly against providers in views
- provider token files are no longer stored in `assets/`
- a single screen-oriented API contract powers both desktop and mobile clients
- Schwab and Plaid auth flows are mediated by the API
- IBKR is isolated behind an explicit, safe adapter boundary
