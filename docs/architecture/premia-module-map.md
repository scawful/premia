# Premia Module Map

## Target repo layout

```text
premia/
  apps/
    api/
      src/
        http/
        websocket/
        middleware/
        handlers/
        presenters/
        jobs/
      config/
    desktop/
      src/
        ui/
        presenters/
        bootstrap/
    mobile-ios/
      PremiaMobile/
      PremiaUI/
      PremiaAPIClient/
      PremiaModels/
  contracts/
    openapi/
      premia-v1.yaml
    events/
      stream-events.md
  src/
    core/
      domain/
      application/
      ports/
    providers/
      tda/
      schwab/
      ibkr/
      plaid/
    infrastructure/
      cache/
      persistence/
      logging/
      secrets/
      time/
  clients/
    swift/
      Generated/
  test/
    core/
    providers/
    api/
    contracts/
    integration/
```

## Module boundaries

### Domain and application

- `premia-domain`
  - pure entities, value objects, enums, and domain rules
  - no transport, broker SDKs, UI, or persistence code
- `premia-application`
  - use cases, screen aggregators, orchestration, policies
  - depends on domain and ports only
- `premia-ports`
  - interfaces for providers, persistence, cache, secrets, clock, streams

### Provider adapters

- `premia-provider-tda`
- `premia-provider-schwab`
- `premia-provider-ibkr`
- `premia-provider-plaid`

Each provider module:
- consumes raw provider APIs or SDKs
- maps provider payloads into core types
- exposes provider capabilities behind a port interface
- never leaks provider JSON to the application or UI layers

### Infrastructure

- `premia-infra-cache`
- `premia-infra-persistence`
- `premia-infra-secrets`
- `premia-infra-logging`
- `premia-infra-time`

### App clients

- `premia-api-http`
- `premia-api-stream`
- `premia-desktop-ui`
- `premia-ios-app`
- `premia-swift-client`

## Dependency graph

```text
premia-ios-app -> premia-swift-client -> premia-contracts
premia-desktop-ui -> premia-application -> premia-domain

premia-api-http -> premia-application -> premia-domain
premia-api-stream -> premia-application -> premia-domain

premia-provider-* -> premia-ports -> premia-application -> premia-domain
premia-infra-* -> premia-ports -> premia-application -> premia-domain

premia-domain -> nothing
```

## Current to target migration map

### Existing code to preserve and relocate

- `src/service/TDAmeritrade/` -> `src/providers/tda/`
- `src/service/Schwab/` -> `src/providers/schwab/`
- `src/service/InteractiveBrokers/` -> `src/providers/ibkr/`
- `src/service/Plaid/` -> `src/providers/plaid/`

### Existing code to refactor before reuse

- `src/app/core/` singleton facades -> replace with application service wiring
- `src/app/model/` business logic -> move reusable parts into
  `src/core/application/`
- `src/app/view/` direct provider access -> replace with DTO-driven presenters

### Existing code to treat as transitional

- `premiamobile.xcworkspace/` -> preserve only as an artifact of intent; build
  a new iOS app from `apps/mobile-ios/`
- `src/service/TDAmeritrade/proto/` -> keep only if needed for internal-only
  service boundaries; do not force mobile contracts to mirror current gRPC work

### Desktop path note

- active desktop path: `src/app/view/`, `src/app/model/`, and `premia_core`
  provider-backed services
- archived desktop path: `src/app/view/legacy/` is preserved for historical
  reference only and is not compiled into the active `premia` target

## Capability map by provider

### Schwab

Near-term scope:
- OAuth connection status
- accounts and balances
- holdings and portfolio summary
- quotes and charts
- option chain snapshots

### Plaid

Near-term scope:
- link flow
- linked institutions and depository accounts
- transaction sync and pagination

### IBKR

Near-term scope:
- connection status
- read-only account and positions once sample code is stripped down
- no FA mutation flows in initial shared API

### TDAmeritrade

Near-term scope:
- transitional support while Schwab parity is established
- direct UI dependencies should be reduced even if TDA remains in the tree

## Contracts-first rule

The public contract lives in `contracts/openapi/`.

Everything else is downstream of that contract:
- API handlers implement it
- Swift client is generated from it
- desktop presenters consume the same DTOs or equivalent local models
- provider adapters only exist to satisfy application use cases behind it
