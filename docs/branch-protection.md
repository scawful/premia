# Branch Protection

Recommended protection for `master`:

- require pull requests before merge
- require at least 1 approval
- dismiss stale approvals when new commits are pushed
- require branches to be up to date before merge
- require status checks to pass before merge
- do not allow force pushes
- do not allow deletions

Recommended required checks:

- `build-test`

What `build-test` covers today:

- C++ configure/build for `premia`, `premia_api`, and test binaries
- provider-backed C++ tests
- API integration tests
- OpenAPI validation and Swift client regeneration
- Swift package build and tests
- iOS Simulator app-host build

If more jobs are added later, update this file to keep the required-check list
in sync with the GitHub repository settings.
