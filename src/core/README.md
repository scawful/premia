# Premia Core

`src/core/` is the shared platform layer that sits between provider adapters and
client applications.

Current scaffold:
- `include/premia/core/domain/` - value types and shared enums.
- `include/premia/core/application/` - screen-oriented DTOs and service
  contracts.
- `include/premia/core/ports/` - provider-side interfaces that adapters will
  implement.

Design rules:
- no UI dependencies
- no direct broker JSON in public core headers
- no token or secret storage concerns in domain types
- DTOs should match the normalized API contract, not provider payload shapes
