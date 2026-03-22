# Premia Mobile iOS Scaffold

This is the first Swift package scaffold for the future Premia iOS client.

Targets:
- `PremiaModels` - app-facing models mirrored from the normalized API contract
- `PremiaAPIClient` - thin async wrapper around the generated client package
- `PremiaUI` - reusable SwiftUI components that render normalized models

This package is intentionally small and hand-written for now. The generated
OpenAPI client should eventually live under `clients/swift/Generated/`, with
hand-written adapters bridging it into the `PremiaAPIClient` target.

Current integration:
- `apps/mobile-ios/Package.swift` depends on the local generated package at
  `clients/swift/Generated/`
- `PremiaAPIClient` now configures `PremiaAPIClientGeneratedAPI.basePath` and
  exposes typed async wrapper methods for bootstrap, home, quote, and chart
  loading
- `PremiaAPIClient` also wraps watchlist loading plus Schwab and Plaid
  connection workflow methods so SwiftUI can consume normalized app models
  instead of generated types directly
- watchlist create/rename/add/remove mutation wrappers are now exposed, and API
  errors are normalized into a hand-written `PremiaAPIClientError` model for
  SwiftUI state handling
- account and option-chain screen wrappers are now exposed too, so SwiftUI can
  consume the new account/options API surface without touching generated models
