# premia

Details
--------
- Description: Risk parity anaylsis and trading program
- Platform: macOS, GNU/Linux
- Toolchain: clang
- Dependencies: SDL2, QuantLib
- API: TDAmeritrade

API KEY
--------

This application makes use of TDAmeritrade API which gives a custom api key. This key gives you access to trading on behalf of the account associated with it, which is not something I'd like to make publicly accessible. So to use this application you'll have to set that up. I won't include instructions on how to do that, at least for now, since the application isn't very useful in its state. But if you were to try to use it and could get it figured out yourself you can add the file 

apikey.hpp

and include the statement `static std::string TDA_API_KEY = "<INSERT-API-KEY>";` with your TDAmeritrade API key

