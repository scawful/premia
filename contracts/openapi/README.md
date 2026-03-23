# Premia OpenAPI Contract

`contracts/openapi/premia-v1.yaml` is the canonical application-facing contract
for Premia clients.

Current scope:
- bootstrap
- home screen
- watchlists
- quote detail
- chart detail
- Schwab/Plaid connection workflows
- stream entrypoint

Downstream consumers:
- `apps/api/` implements this surface
- `clients/swift/` uses it for generation
- `apps/mobile-ios/` wraps generated client code in hand-written app adapters

Suggested validation tools:

```bash
npx --yes @openapitools/openapi-generator-cli validate -i contracts/openapi/premia-v1.yaml
```

Suggested generation command:

```bash
npx --yes @openapitools/openapi-generator-cli generate \
  -i contracts/openapi/premia-v1.yaml \
  -g swift6 \
  -o clients/swift/Generated \
  -c clients/swift/openapi-generator-config.yaml
```

Current status:
- the first generated client package already exists in `clients/swift/Generated/`
- the generated package is blocked from local Swift validation by the same
  `/usr/local/include` module-map conflict affecting other Swift builds on this
  machine
