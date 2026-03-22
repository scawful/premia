# Premia Providers

`src/providers/` contains adapter implementations that translate provider data
into the normalized core contracts.

Current adapters:
- `local/portfolio_provider.cc` - file-backed portfolio and holdings provider
  used for desktop and API fallback flows.
- `local/account_detail_provider.cc` - file-backed account detail and position
  provider used for desktop fallback flows.
- `local/watchlist_provider.cc` - file-backed local watchlist provider used for
  desktop and API fallback flows.
- `local/options_provider.cc` - file-backed option chain snapshot provider used
  for desktop fallback flows.
- `tda/portfolio_provider.cc` - authenticated TDA portfolio provider used when
  a valid `assets/tda.json` config is available.
- `tda/account_detail_provider.cc` - authenticated TDA account detail provider
  used when a valid `assets/tda.json` config is available.
- `tda/watchlist_provider.cc` - authenticated TDA watchlist provider used when
  a valid `assets/tda.json` config is available.
- `tda/options_provider.cc` - authenticated TDA option chain provider used when
  a valid `assets/tda.json` config is available.
- `schwab/market_data_provider.cc` - Schwab quote and chart adapter used when a
  valid Schwab config and token set are available.
- `schwab/workflow_provider.cc` - Schwab OAuth workflow adapter for start and
  completion flows.
- `plaid/workflow_provider.cc` - Plaid Link workflow adapter for link-token and
  completion flows.

These adapters are intentionally narrow. They let `premia_core` stop owning the
provider-specific parsing and fallback glue directly.
