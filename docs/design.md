
# Software Design Plan

### Overview
- Stock trading client using TDAmeritrade API
- Primary function is analyzing risk of portfolio 
- Support for making buy and sell orders
- Real time quotes and historical charts

## Structure
Premia's main event loop is handled by a virtual state manager defined in the Manager class and implemented using the State virtual class. 

- Start Screen
	- TODO: Create login method
	- TODO: Display balances, movers, brief portfolio analysis 
- Quotes
	- Candle chart for TDA supported assets
		- Historical Data Timeframes:
			- Period: Day, Month, Year, YTD
			- Period Type: 1 2 3 4 5 6 10 15 20
			- Frequency: Minute Daily Weekly Monthly
			- Amount: 1 5 10 15 30
	- TODO: Add additional indicators
	- TODO: Crayon drawing on charts
- Stream
	- Real time quote streaming via async WebSocket session
	- TODO: Build candles from stream data 
- Options
	- Detailed option chain (Very buggy)
	- TODO: Add chart for options price history 
	- TODO: Multi leg strategy orders 
- Trade / Order [Unimplemented] 
	- Place Order
	- Replace Order
	- Cancel Order 
	- Get Order
- Portfolio
	- Positions
	- TODO: Balances
	- TODO: Orders (Active, Cancelled, Executed)
	
## Market Data 
**TDAmeritrade**
- Account
- Option Chain 
- Price History
	- Uses Candle ADT
- Price Structures [ADTs]
	- Candles
		- Volume
		- High and low price
		- Open and close price
		- Date and time
	- Options Contract
		- Put or Call
		- Bid, ask, bidSize, askSize
		- Open, close, high, low
		- Delta, gamma, theta, vega, rho
	- Strike Price
- Price Quote
- WebSocket Session

**Coinbase Pro**
- Account
- Client
- Product

## TDAmeritrade Data Interface
Object that handles communication with TDAmeritrade's Developer API. Present in any state of the program that requires data from the brokerage. 
-	RESTful requests like GET and POST 
-	WebSocket session threading and data sharing
-	Authenticating with access token and refresh key
-	Creating shared pointers of pricing data structures 

## Dependencies
	- SDL2.0
	- ImGui
	- ImGuiSDL
	- ImPlot

> Justin Scofield 2021
