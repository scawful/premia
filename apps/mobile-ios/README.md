# Premia Mobile iOS Scaffold

This is the first Swift package scaffold for the future Premia iOS client.

Targets:
- `PremiaModels` - app-facing models mirrored from the normalized API contract
- `PremiaAPIClient` - thin async client wrapper and configuration surface
- `PremiaUI` - reusable SwiftUI components that render normalized models

This package is intentionally small and hand-written for now. The generated
OpenAPI client should eventually live under `clients/swift/Generated/`, with
hand-written adapters bridging it into the `PremiaAPIClient` target.
