# Premia MVP API Contracts

This document summarizes the first screen-oriented API surface that both the
desktop and iOS clients should target. The canonical machine-readable contract
belongs in `contracts/openapi/premia-v1.yaml`.

## Contract principles

- expose screen and workflow data, not broker-native payloads
- include `meta.requestId` and `meta.asOf` on every response
- return structured errors with retry and action hints
- encode money and decimal values as strings
- expose provider freshness and connection state explicitly

## Common envelopes

Success envelope:

```json
{
  "data": {},
  "meta": {
    "requestId": "req_123",
    "asOf": "2026-03-22T18:45:00Z"
  }
}
```

Error envelope:

```json
{
  "error": {
    "code": "AUTH_RENEWAL_REQUIRED",
    "message": "Schwab session expired",
    "provider": "schwab",
    "retryable": false,
    "action": "reauth"
  },
  "meta": {
    "requestId": "req_123"
  }
}
```

## MVP endpoints

### App bootstrap

- `GET /v1/bootstrap`
- used to initialize app state, feature flags, and connection summaries

### Home screen

- `GET /v1/screens/home`
- returns connection summaries, portfolio summary, top holdings, watchlists, and
  market session info

### Watchlists

- `GET /v1/watchlists`
- `GET /v1/screens/watchlists/{watchlistId}`
- `POST /v1/watchlists`
- `PATCH /v1/watchlists/{watchlistId}`
- `POST /v1/watchlists/{watchlistId}/symbols`
- `DELETE /v1/watchlists/{watchlistId}/symbols/{symbol}`

### Quotes and charts

- `GET /v1/screens/quotes/{symbol}`
- `GET /v1/screens/charts/{symbol}`

Recommended chart query parameters:
- `range`
- `interval`
- `extendedHours`

### Connection workflows

- `POST /v1/connections/schwab/oauth/start`
- `POST /v1/connections/schwab/oauth/complete`
- `POST /v1/connections/plaid/link-token`
- `POST /v1/connections/plaid/link-complete`

### Streaming

- `GET /v1/stream/events`

First event families:
- `quote.updated`
- `portfolio.invalidated`
- `watchlist.updated`
- `connection.changed`
- `session.reauth_required`

## Core DTOs

### ConnectionSummary

Fields:
- `provider`
- `status`
- `displayName`
- `lastSyncAt`
- `reauthRequired`
- `capabilities`

### PortfolioSummary

Fields:
- `totalValue`
- `dayChange`
- `cash`
- `buyingPower`
- `holdingsCount`

### HoldingRow

Fields:
- `id`
- `symbol`
- `name`
- `quantity`
- `marketValue`
- `dayChange`

### WatchlistSummary and WatchlistRow

Fields:
- `id`
- `name`
- `instrumentCount`

For each row:
- `symbol`
- `name`
- `lastPrice`
- `dayChange`
- `bid`
- `ask`
- `updatedAt`

### QuoteDetail

Fields:
- `instrument`
- `quote`
- `position`
- `watchlistMembership`

### ChartSeries

Fields:
- `instrument`
- `range`
- `interval`
- `timezone`
- `series`
- `stats`

### Plaid workflow DTOs

Fields:
- `linkToken`
- `expiration`
- `institutionId`
- `linkedAccounts`
- `cursor`

## Design notes for SwiftUI

- DTOs should support immediate mapping to screen-specific state objects.
- Stable IDs and explicit optionality are required for list diffing.
- Provider status must be available at the screen level so the UI can degrade
  gracefully when one adapter is stale or disconnected.
