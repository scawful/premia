
# Software Design Plan

### Overview
- Stock trading client with support for multiple brokerages API 
    - TDAmeritrade
    - CoinbasePro
    - IEXCloud
- Primary function is analyzing risk of portfolio 
- Places an emphasis on risk premia hedging and the impact of the options market
- Support for making buy and sell orders
- Real time quotes and historical charts

## MVC Framework
Premia's main event loop is managed by the common Model-View-Controller framework, with a few custom touches to accomodate the ImGui library.

### Model
- Concrete Parent
    - API Interface Objects
    - Map of EventHandlers
- Chart
    - ChartModel : Model
        - Historical Data Timeframes:
        - Period: Day, Month, Year, YTD
        - Period Type: 1 2 3 4 5 6 10 15 20
        - Frequency: Minute Daily Weekly Monthly
        - Amount: 1 5 10 15 30

### View
- Abstract Parent
    - Public update function for ViewManager
- ViewManager
    - Handles the lifetime of a View object
- PrimaryView : View

- Account
    - TODO : SettingsView
- Analyze 
    - TODO : FundOwnershipView
- Chart
    - Chart Abstract Parent
    - CandleChart : Chart
    - ChartView : View  
    - TODO : LinePlotChart
    - TODO : BarPlotChart
- Core 
    - LoginView : View
    - MenuView : View
- Tools
    - TODO : ConsoleView
- Trade
    - TODO : OptionChainView
    - TODO : OrderView
    - TODO : PortfolioView
    - TODO : WatchlistView
- Dev 
    - DebugView : View

### Controller
- Abstract Parent
    - onEntry()
    - onInput()
    - onLoad()
    - doRender()
    - onExit()
- DebugController : Controller
- PrimaryController : Controller


## Todo List
- Create login method
- Display balances, movers, brief portfolio analysis 
- Set up parsing and GUI callbacks for WebSocket session data
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
	- Balances
	- Orders (Active, Cancelled, Executed)
	
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

## TDAmeritrade Interface
Object that handles communication with TDAmeritrade's Developer API. Present in any state of the program that requires data from the brokerage. 
-	RESTful requests like GET and POST 
-	WebSocket session threading and data sharing
-	Authenticating with access token and refresh key
-	Creating shared pointers of pricing data structures 

## IEXCloud Interface
Object that handles communication with the IEXCloud API. 

## Dependencies
	- SDL2.0
	- ImGui
	- ImPlot
    - libCURL
    - Boost

> Justin Scofield 2021 - 2022
