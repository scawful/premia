
# Software Design Plan

### Overview
- Stock trading client using TDAmeritrade API
- Primary function is analyzing risk of portfolio 
- Support for making buy and sell orders
- Real time quotes and historical charts

## Structure
List represents the object relationships present in Premia's event loop. 

### Virtual State Manager
State.hpp, Manager.cpp, Manager.hpp

	- Start Screen
		- 
	- Quotes
		- Candle chart for TDA supported assets
	- Stream
		- Real time quote streaming via async WebSocket session
		- TODO: Build candles from stream data 
	- Options
		- Detailed option chain
		- Very buggy.
	- Portfolio
		- Positions
		- 
### Market Data 
	- TDAmeritrade 
		- Account
		- Option Chain 
		- Price History
		- Price Structures 
		- Price Quote
		- Websocket Session

## Dependencies
	- SDL2.0
	- ImGui
	- ImGuiSDL
	- ImPlot

> Justin Scofield 2021
