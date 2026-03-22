# Premia Apps

This directory holds application entry points built on top of the shared core.

Current scaffold:
- `apps/api/` - HTTP API skeleton with demo responses derived from core DTOs.
- `apps/mobile-ios/` - Swift package layout for models, client code, and UI.

Longer term this directory should also host a refactored desktop client once the
shared contracts are stable enough to pull logic out of `src/app/`.
