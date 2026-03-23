# Legacy Desktop Views

This directory is preserved as archival desktop UI code from the pre-
`premia_core` / provider-backed architecture.

Status:
- not compiled by the active `premia` target
- not part of the supported desktop runtime path
- kept for historical reference and design recovery only

Current desktop builds are driven by the provider-backed ImGui views in
`src/app/view/` and the shared services behind `premia_core`.

Important notes:
- several files here depend on missing or renamed base classes/includes
- several files assume old direct provider access patterns that are no longer
  used by the active app
- do not re-enable these files in CMake without reconciling them against the
  current provider-backed architecture first

Historically interesting files:
- `risk_premia_frame.cc` - old home/education hub concept
- `Frames/Analyze/FundOwnershipFrame.cpp` - old analysis UI prototype
- `Frames/Trade/TradingFrame.cpp` - old direct-trading prototype
