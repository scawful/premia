# Premia Implementation Roadmap

This roadmap assumes the goal is not just to add an iOS app, but to turn Premia
into a reusable platform with a shared application-facing API surface.

## Milestone 0 - Stabilize the current desktop code

Goal:
- remove regressions from recent modernization work before extracting shared
  modules

Work:
- repair the active desktop login flow and make auth success/failure explicit
- remove hardcoded credential regressions from chart bootstrap code
- restore sane SDL + ImGui input handling
- stop views from calling unbound `events.at(...)`
- make provider token/config storage safer than `assets/*.json`
- strip Interactive Brokers sample mutation flows from the default path

Repo focus:
- `src/app/core/`
- `src/app/model/`
- `src/app/view/`
- `src/service/InteractiveBrokers/`

Exit criteria:
- desktop app can boot and log in without hidden provider-side effects
- keyboard and menu interactions are stable again
- no hardcoded secrets or sample FA operations remain on the active path

## Milestone 1 - Define contracts and extract core models

Goal:
- create the shared language of the system before moving more code

Work:
- define app-facing DTOs for bootstrap, portfolio, watchlists, quotes, charts,
  connections, and Plaid banking
- create `src/core/domain/`, `src/core/application/`, and `src/core/ports/`
- move shared value objects like money, timestamps, paging, and provider status
- define provider ports for market data, portfolio, auth, and banking

Repo focus:
- `src/core/`
- `contracts/openapi/`
- `test/core/`
- `test/contracts/`

Exit criteria:
- no new app feature work bypasses core contracts
- application DTOs exist independently of broker payloads

## Milestone 2 - Stand up Premia API

Goal:
- establish a single backend for auth, normalization, and client-safe access

Work:
- create `apps/api/` skeleton with REST handlers and auth middleware
- implement `GET /v1/bootstrap`, `GET /v1/screens/home`, `GET /v1/watchlists`,
  `GET /v1/screens/quotes/{symbol}`, and `GET /v1/screens/charts/{symbol}`
- add structured error envelopes and request IDs
- add secure token storage abstraction

Repo focus:
- `apps/api/`
- `src/core/application/`
- `src/infrastructure/`

Exit criteria:
- one minimal client can load bootstrap, portfolio summary, watchlists, quote,
  and chart data through the Premia API alone

## Milestone 3 - Harden provider adapters

Goal:
- convert existing provider code from direct app dependencies into safe adapters

Work:
- move provider implementations to `src/providers/`
- Schwab: typed parsing, explicit HTTP error handling, token refresh service,
  account bootstrap integration
- Plaid: link token service, public token exchange, null-safe parsing,
  transaction pagination support
- IBKR: remove default sample operations, isolate read-only connection/account
  access, make case-sensitive build fixes
- TDA: keep only the minimum needed for migration and parity

Repo focus:
- `src/providers/schwab/`
- `src/providers/plaid/`
- `src/providers/ibkr/`
- `src/providers/tda/`
- `test/providers/`

Exit criteria:
- providers are only reachable through ports
- provider failures surface as structured application errors

## Milestone 4 - Connection workflows and mobile-safe auth

Goal:
- make all connection flows API-mediated and safe for SwiftUI clients

Work:
- implement Schwab OAuth start/complete endpoints
- implement Plaid link token and completion endpoints
- expose connection summaries and reauth-required state
- add secure secret storage and token rotation policies

Repo focus:
- `apps/api/src/http/handlers/`
- `src/core/application/services/`
- `src/infrastructure/secrets/`

Exit criteria:
- iOS and desktop can connect providers without storing provider secrets locally

## Milestone 5 - SwiftUI mobile MVP

Goal:
- ship a read-heavy iOS experience using the shared API

Work:
- create `apps/mobile-ios/`
- generate or hand-write a Swift client from `contracts/openapi/premia-v1.yaml`
- implement screens for bootstrap, home/portfolio, watchlists, quote detail,
  chart detail, and connection status
- build reusable SwiftUI components for metric cards, holding rows, quote badges,
  change pills, and chart containers

Repo focus:
- `apps/mobile-ios/`
- `clients/swift/`
- `contracts/openapi/`

Exit criteria:
- the iOS app can render a meaningful portfolio and watchlist experience without
  broker-specific UI code

## Milestone 6 - Banking and options expansion

Goal:
- expand the normalized surface beyond the initial market-data MVP

Work:
- add Plaid banking overview and transaction paging endpoints
- add normalized option chain endpoints and option summary DTOs
- add provider capability flags so unsupported features degrade cleanly

Repo focus:
- `src/core/application/`
- `apps/api/`
- `contracts/openapi/`
- `apps/mobile-ios/`

Exit criteria:
- banking and options screens are driven by normalized contracts, not provider
  payload branching in clients

## Milestone 7 - Desktop convergence

Goal:
- make the desktop app another client of the shared contracts instead of a
  separate architecture

Work:
- move desktop UI code into `apps/desktop/`
- replace singleton provider calls with presenters/view-models over application
  services
- keep console/debug workflows, but make them explicit dev tools

Repo focus:
- `apps/desktop/`
- `src/core/application/`
- `src/providers/`

Exit criteria:
- desktop and iOS share the same screen models and connection semantics

## Recommended sequencing notes

- Do not start a real SwiftUI build before Milestones 1 and 2 are underway.
- Do not expose IBKR trading or FA operations before a read-only surface is
  stable and explicit.
- Do not expand provider-specific feature depth until the shared contracts are
  settled.
