# Premia Swift Client

This directory is reserved for generated and hand-maintained Swift client code
derived from `contracts/openapi/premia-v1.yaml`.

Recommended flow:
1. Keep `contracts/openapi/premia-v1.yaml` as the canonical client contract.
2. Use OpenAPI Generator to emit a Swift package into `clients/swift/Generated/`.
3. Wrap the generated client in a thin hand-written adapter before exposing it
   to SwiftUI screens.

Current status:
- `clients/swift/Generated/` has been generated from the current OpenAPI file.
- The generated package manifest exists and the API/model files are present.
- `apps/mobile-ios/Package.swift` now depends on the generated package via a
  local path dependency, and the hand-written client wraps it instead of only
  constructing URLs manually.
- the hand-written client now also normalizes generated errors into a shared
  Swift error model and exposes watchlist mutation helpers on top of the
  generated package.
- the generated client now includes account and options screen APIs, and the
  hand-written wrapper maps them into app-level Swift models.
- Local Swift package compilation is currently blocked by conflicting
  `/usr/local/include` module maps on this machine, the same environment issue
  affecting the hand-written Swift scaffold.

Suggested generation command:

```bash
npx --yes @openapitools/openapi-generator-cli generate \
  -i contracts/openapi/premia-v1.yaml \
  -g swift5 \
  -o clients/swift/Generated \
  -c clients/swift/openapi-generator-config.yaml
```

The hand-written Swift package scaffold currently lives in `apps/mobile-ios/`.
