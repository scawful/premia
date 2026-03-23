# PremiaMobile App Host

This folder contains an Xcode app host for the Swift package in
`apps/mobile-ios/`.

Generate the project with:

```bash
xcodegen generate --spec PremiaMobile/project.yml
```

Build for Simulator with:

```bash
xcodebuild -project PremiaMobile/PremiaMobile.xcodeproj \
  -scheme PremiaMobile \
  -destination 'generic/platform=iOS Simulator' \
  CODE_SIGNING_ALLOWED=NO build
```

The app entry point renders `PremiaRootView` and defaults to
`http://127.0.0.1:8080` for the Premia API base URL.

Override at runtime with:
- env var `PREMIA_API_BASE_URL`
- `UserDefaults` key `PREMIA_API_BASE_URL`
