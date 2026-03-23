# Premia API Scaffold

`apps/api/` is a thin HTTP skeleton for the future Premia application-facing
service.

Current goals:
- exercise the normalized DTOs from `src/core/`
- expose the first MVP routes through concrete `premia_core` scaffold services
- provide a concrete place for auth middleware, handlers, and streaming work

Current routes:
- `GET /health`
- `GET /v1/bootstrap`
- `GET /v1/screens/home`
- `GET /v1/screens/account`
- `GET /v1/watchlists`
- `GET /v1/screens/watchlists/{watchlistId}`
- `POST /v1/watchlists`
- `PATCH /v1/watchlists/{watchlistId}`
- `POST /v1/watchlists/{watchlistId}/symbols`
- `DELETE /v1/watchlists/{watchlistId}/symbols/{symbol}`
- `GET /v1/screens/quotes/{symbol}`
- `GET /v1/screens/charts/{symbol}`
- `GET /v1/screens/options/{symbol}`
- `POST /v1/orders/preview`
- `GET /v1/orders/open`
- `GET /v1/orders/history`
- `POST /v1/orders/submit`
- `POST /v1/orders/{orderId}/cancel`
- `POST /v1/orders/{orderId}/replace`
- `GET /v1/stream/events`
- `POST /v1/connections/schwab/oauth/start`
- `POST /v1/connections/schwab/oauth/complete`
- `POST /v1/connections/plaid/link-token`
- `POST /v1/connections/plaid/link-complete`

Planned next routes:
- richer portfolio/account endpoints beyond current screen aggregates

Route behavior today:
- screen queries are served by `premia::core::application::ScaffoldApplicationService`
- Schwab and Plaid connection workflows mutate in-memory scaffold state so the
  next `GET /v1/bootstrap` reflects the updated connection summary
- when valid non-placeholder `assets/schwab.json` or `assets/plaid.json`
  credentials exist, the shared core service will use the real provider clients
  and persisted token files instead of the scaffold fallback
- quote and chart requests now attempt to use real Schwab market-data responses
  when a valid authorized Schwab client is available, otherwise they return the
  normalized scaffold payloads
- account and option-chain screen requests now flow through provider-backed core
  services as well, using TDA when valid credentials exist and local fixtures
  otherwise
- account reads now also support live Schwab first and a secondary read-only
  IBKR path when a local TWS or IB Gateway session is configured and running
- order preview and submit requests now flow through provider-backed order
  services, with TDA used only when valid credentials exist and a live
  submission is explicitly confirmed
- order cancel and replace requests follow the same safety model: simulated
  local behavior by default, live TDA behavior only when real credentials exist
  and live confirmation is explicit
- open-order and order-history reads now flow through the same provider-backed
  order service, using TDA when valid credentials exist and local fixtures
  otherwise
- watchlist mutation routes are backed by the local provider adapter and persist
  changes into `assets/watchlists.json`

Credential and token storage:
- live provider configs and tokens now default to runtime storage under
  `~/Library/Application Support/Premia/providers/` on macOS
- on macOS, provider config/token blobs are also mirrored into Keychain when
  keychain access is enabled
- set `PREMIA_RUNTIME_DIR` to override that location for tests, CI, or isolated
  local runs
- set `PREMIA_DISABLE_KEYCHAIN=1` to force pure runtime-file behavior in tests
  or CI
- checked-in `assets/*.json` files remain fallback fixtures and migration
  sources; they are no longer the preferred write target for live secrets

Build and run:

```bash
cmake -S . -B build
cmake --build build --target premia_api
./build/bin/premia_api --host 127.0.0.1 --port 8080
```

Smoke test:

```bash
curl http://127.0.0.1:8080/v1/bootstrap
curl http://127.0.0.1:8080/v1/screens/home
curl http://127.0.0.1:8080/v1/screens/quotes/AAPL
curl -X POST http://127.0.0.1:8080/v1/connections/plaid/link-token \
  -H "Content-Type: application/json" \
  -d '{"userId":"ios-user-1"}'
```
