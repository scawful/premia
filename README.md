# premia

Details
--------
- Description: Risk Parity Anaylsis and Trading Client
- Platform: Windows, macOS, GNU/Linux
- Toolchain: mingw-w64, clang64
- Dependencies: Boost, SDL2, OpenSSL, QuantLib, ImGui, ImPlot, TWS
- Supported API: TDAmeritrade, InteractiveBrokers, CoinbasePro

Screenshots
--------
![](https://i.ibb.co/X8cSy1T/C15-E0-A46-8357-4-D8-A-BD9-F-15-CDA776-AAF1.png)
![](https://i.ibb.co/NjdT3GS/49-C38803-E56-D-4648-9-F9-F-704-E661-C0-D7-C.png)
![](https://i.ibb.co/JHD6MFK/Screen-Shot-2022-01-09-at-9-52-33-AM.png)

API KEYS
--------

This program requires API keys for both TDAmeritrade and Coinbase Pro to gain access to all features. As of right now, there is no gui based input for these values, so I have them stored in an untracked header file. If you'd like to use this program, you must create: apikey.hpp

`static std::string TDA_API_KEY = "<INSERT-API-KEY>";` with your TDAmeritrade API key

`static std::string REFRESH_TOKEN = "<INSERT-REFRESH-TOKEN";` optional refresh token

`static std::string CBP_KEY = "<INSERT-CBP-KEY>";`

`static std::string CBP_SECRET = "<INSERT-CBP-SECRET-KEY>";`

`static std::string CBP_PASSPHRASE = "<INSERT-CBP-PASSPHRASE>";`

