# Premia Providers

`src/providers/` contains adapter implementations that translate provider data
into the normalized core contracts.

Current adapters:
- `local/watchlist_provider.cc` - file-backed local watchlist provider used for
  desktop and API fallback flows.
- `tda/watchlist_provider.cc` - authenticated TDA watchlist provider used when
  a valid `assets/tda.json` config is available.
- `schwab/market_data_provider.cc` - Schwab quote and chart adapter used when a
  valid Schwab config and token set are available.

These adapters are intentionally narrow. They let `premia_core` stop owning the
provider-specific parsing and fallback glue directly.
