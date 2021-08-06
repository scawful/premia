# premia

Details
--------
- Description: Risk parity anaylsis and trading program
- Platform: Windows, macOS, GNU/Linux
- Toolchain: mingw-w64, clang
- Dependencies: Boost, SDL2, OpenSSL, QuantLib, ImGui, ImGuiSDL, ImPlot
- API: TDAmeritrade, Coinbase Pro

API KEYS
--------

This program requires API keys for both TDAmeritrade and Coinbase Pro to gain access to all features. As of right now, there is no gui based input for these values, so I have them stored in an untracked header file. If you'd like to use this program, you must create: apikey.hpp

`static std::string TDA_API_KEY = "<INSERT-API-KEY>";` with your TDAmeritrade API key

`static std::string REFRESH_TOKEN = "<INSERT-REFRESH-TOKEN";` optional refresh token

`static std::string CBP_KEY = "<INSERT-CBP-KEY>";`

`static std::string CBP_SECRET = "<INSERT-CBP-SECRET-KEY>";`

`static std::string CBP_PASSPHRASE = "<INSERT-CBP-PASSPHRASE>";`

