# Premia Architecture Notes

This folder captures a concrete evolution plan for Premia as a shared platform
instead of a single desktop app with embedded broker clients.

Files:
- `docs/architecture/premia-core-api-ios-architecture.md` - target platform
  architecture for Premia Core, Premia API, desktop, and SwiftUI.
- `docs/architecture/premia-module-map.md` - recommended repo layout, module
  boundaries, and migration map from the current codebase.
- `docs/architecture/premia-implementation-roadmap.md` - phased implementation
  plan with milestones, repo paths, and exit criteria.
- `docs/architecture/premia-mvp-api-contracts.md` - screen-oriented API
  contract summary for desktop and iOS clients.
- `docs/architecture/premia-scaffold-status.md` - implementation snapshot of
  the first `src/core`, `apps/api`, and Swift package scaffolds.
- `contracts/openapi/premia-v1.yaml` - initial OpenAPI outline for the first
  application-facing API surface.

Planning principles:
- Treat broker adapters as implementation details behind stable app contracts.
- Keep secrets and OAuth exchanges out of UI clients.
- Use typed DTOs for screens and components instead of raw provider payloads.
- Let both desktop and iOS consume the same application-facing API surface.
