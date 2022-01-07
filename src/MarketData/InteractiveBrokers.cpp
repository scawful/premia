#include "StdAfx.h"

#include "InteractiveBrokers.hpp"

#include "EClientSocket.h"
#include "EPosixClientSocketPlatform.h"

#include "Contract.h"
#include "Order.h"
#include "OrderState.h"
#include "Execution.h"
#include "CommissionReport.h"
#include "InteractiveBrokers/ContractSamples.h"
#include "InteractiveBrokers/OrderSamples.h"
#include "ScannerSubscription.h"
#include "InteractiveBrokers/ScannerSubscriptionSamples.h"
#include "executioncondition.h"
#include "PriceCondition.h"
#include "MarginCondition.h"
#include "PercentChangeCondition.h"
#include "TimeCondition.h"
#include "VolumeCondition.h"
#include "InteractiveBrokers/AvailableAlgoParams.h"
#include "InteractiveBrokers/FAMethodSamples.h"
#include "CommonDefs.h"
#include "InteractiveBrokers/AccountSummaryTags.h"
#include "InteractiveBrokers/Utils.h"

const int PING_DEADLINE = 2; // seconds
const int SLEEP_BETWEEN_PINGS = 30; // seconds

/**
 * @brief Get current time from server
 * @author InteractiveBrokers LLC
 * 
 */
void InteractiveBrokers::reqCurrentTime()
{
    printf( "Requesting Current Time\n");

    // set ping deadline to "now + n seconds"
    sleep_deadline = time( NULL) + PING_DEADLINE;

    client_state = ST_PING_ACK;

    client_socket->reqCurrentTime();
}

/**
 * @brief Construct a new Interactive Brokers:: Interactive Brokers object
 * @author InteractiveBrokers LLC
 * 
 */
InteractiveBrokers::InteractiveBrokers() :
    os_signal(2000) //2-seconds timeout
, client_socket(new EClientSocket(this, &os_signal))
, client_state(ST_CONNECT)
, sleep_deadline(0)
, order_id(0)
, p_reader(0)
, extra_auth(false)
{
    
}

InteractiveBrokers::~InteractiveBrokers()
{
    if (p_reader)
        delete p_reader;

    delete client_socket;
}

bool InteractiveBrokers::connect(const char *host, int port, int clientId)
{
    // trying to connect
    printf( "Connecting to %s:%d clientId:%d\n", !( host && *host) ? "127.0.0.1" : host, port, clientId);
    
    //! [connect]
    bool result = client_socket->eConnect( host, port, clientId, extra_auth);
    //! [connect]
    if (result) {
        printf( "Connected to %s:%d clientId:%d\n", client_socket->host().c_str(), client_socket->port(), clientId);
        //! [ereader]
        p_reader = new EReader(client_socket, &os_signal);
        p_reader->start();
        //! [ereader]
    }
    else
        printf( "Cannot connect to %s:%d clientId:%d\n", client_socket->host().c_str(), client_socket->port(), clientId);

    return result;
}

void InteractiveBrokers::disconnect() const
{
    client_socket->eDisconnect();

    printf ( "Disconnected\n");
}

bool InteractiveBrokers::isConnected() const
{
    return client_socket->isConnected();
}

void InteractiveBrokers::setConnectOptions(const std::string& connectOptions)
{
    client_socket->setConnectOptions(connectOptions);
}

void InteractiveBrokers::processMessages()
{
    time_t now = time(NULL);

    switch (client_state) {
        case ST_REQHEADTIMESTAMP_ACK:
        case ST_REQHISTOGRAMDATA_ACK:
        case ST_REROUTECFD_ACK:
        case ST_CONNECT:
        case ST_CANCELORDER:
        case ST_CANCELORDER_ACK:
        case ST_MARKETRULE_ACK:
        case ST_CONTFUT_ACK:
            break;
        case ST_PNLSINGLE:
            pnlSingleOperation();
            break;
        case ST_PNLSINGLE_ACK:
            break;
        case ST_PNL:
            pnlOperation();
            break;
        case ST_PNL_ACK:
            break;
        case ST_TICKDATAOPERATION:
            tickDataOperation();
            break;
        case ST_TICKDATAOPERATION_ACK:
            break;
        case ST_TICKOPTIONCOMPUTATIONOPERATION:
            tickOptionComputationOperation();
            break;
        case ST_TICKOPTIONCOMPUTATIONOPERATION_ACK:
            break;
        case ST_DELAYEDTICKDATAOPERATION:
            delayedTickDataOperation();
            break;
        case ST_DELAYEDTICKDATAOPERATION_ACK:
            break;
        case ST_MARKETDEPTHOPERATION:
            marketDepthOperations();
            break;
        case ST_MARKETDEPTHOPERATION_ACK:
            break;
        case ST_REALTIMEBARS:
            realTimeBars();
            break;
        case ST_REALTIMEBARS_ACK:
            break;
        case ST_MARKETDATATYPE:
            marketDataType();
            break;
        case ST_MARKETDATATYPE_ACK:
            break;
        case ST_HISTORICALDATAREQUESTS:
            historicalDataRequests();
            break;
        case ST_HISTORICALDATAREQUESTS_ACK:
            break;
        case ST_OPTIONSOPERATIONS:
            optionsOperations();
            break;
        case ST_OPTIONSOPERATIONS_ACK:
            break;
        case ST_CONTRACTOPERATION:
            contractOperations();
            break;
        case ST_CONTRACTOPERATION_ACK:
            break;
        case ST_MARKETSCANNERS:
            marketScanners();
            break;
        case ST_MARKETSCANNERS_ACK:
            break;
        case ST_FUNDAMENTALS:
            fundamentals();
            break;
        case ST_FUNDAMENTALS_ACK:
            break;
        case ST_BULLETINS:
            bulletins();
            break;
        case ST_BULLETINS_ACK:
            break;
        case ST_ACCOUNTOPERATIONS:
            accountOperations();
            break;
        case ST_ACCOUNTOPERATIONS_ACK:
            break;
        case ST_ORDEROPERATIONS:
            orderOperations();
            break;
        case ST_ORDEROPERATIONS_ACK:
            break;
        case ST_OCASAMPLES:
            ocaSamples();
            break;
        case ST_OCASAMPLES_ACK:
            break;
        case ST_CONDITIONSAMPLES:
            conditionSamples();
            break;
        case ST_CONDITIONSAMPLES_ACK:
            break;
        case ST_BRACKETSAMPLES:
            bracketSample();
            break;
        case ST_BRACKETSAMPLES_ACK:
            break;
        case ST_HEDGESAMPLES:
            hedgeSample();
            break;
        case ST_HEDGESAMPLES_ACK:
            break;
        case ST_TESTALGOSAMPLES:
            testAlgoSamples();
            break;
        case ST_TESTALGOSAMPLES_ACK:
            break;
        case ST_FAORDERSAMPLES:
            financialAdvisorOrderSamples();
            break;
        case ST_FAORDERSAMPLES_ACK:
            break;
        case ST_FAOPERATIONS:
            financialAdvisorOperations();
            break;
        case ST_FAOPERATIONS_ACK:
            break;
        case ST_DISPLAYGROUPS:
            testDisplayGroups();
            break;
        case ST_DISPLAYGROUPS_ACK:
            break;
        case ST_MISCELANEOUS:
            miscelaneous();
            break;
        case ST_MISCELANEOUS_ACK:
            break;
        case ST_FAMILYCODES:
            reqFamilyCodes();
            break;
        case ST_FAMILYCODES_ACK:
            break;
        case ST_SYMBOLSAMPLES:
            reqMatchingSymbols();
            break;
        case ST_SYMBOLSAMPLES_ACK:
            break;
        case ST_REQMKTDEPTHEXCHANGES:
            reqMktDepthExchanges();
            break;
        case ST_REQMKTDEPTHEXCHANGES_ACK:
            break;
        case ST_REQNEWSTICKS:
            reqNewsTicks();
            break;
        case ST_REQNEWSTICKS_ACK:
            break;
        case ST_REQSMARTCOMPONENTS:
            reqSmartComponents();
            break;
        case ST_REQSMARTCOMPONENTS_ACK:
            break;
        case ST_NEWSPROVIDERS:
            reqNewsProviders();
            break;
        case ST_NEWSPROVIDERS_ACK:
            break;
        case ST_REQNEWSARTICLE:
            reqNewsArticle();
            break;
        case ST_REQNEWSARTICLE_ACK:
            break;
        case ST_REQHISTORICALNEWS:
            reqHistoricalNews();
            break;
        case ST_REQHISTORICALNEWS_ACK:
            break;
        case ST_REQHEADTIMESTAMP:
            reqHeadTimestamp();
            break;
        case ST_REQHISTOGRAMDATA:
            reqHistogramData();
            break;
        case ST_REROUTECFD:
            rerouteCFDOperations();
            break;
        case ST_MARKETRULE:
            marketRuleOperations();
            break;
        case ST_CONTFUT:
            continuousFuturesOperations();
            break;
        case ST_REQHISTORICALTICKS:
            reqHistoricalTicks();
            break;
        case ST_REQHISTORICALTICKS_ACK:
            break;
        case ST_REQTICKBYTICKDATA:
            reqTickByTickData();
            break;
        case ST_REQTICKBYTICKDATA_ACK:
            break;
        case ST_WHATIFSAMPLES:
            whatIfSamples();
            break;
        case ST_WHATIFSAMPLES_ACK:
            break;
        case ST_PING:
            reqCurrentTime();
            break;
        case ST_PING_ACK:
            if( sleep_deadline < now) {
                disconnect();
                return;
            }
            break;
        case ST_IDLE:
            if( sleep_deadline < now) {
                client_state = ST_PING;
                return;
            }
            break;
    }

    os_signal.waitForSignal();
    errno = 0;
    p_reader->processMsgs();
}

//! [nextvalidid]
void InteractiveBrokers::nextValidId( OrderId orderId)
{
    printf("Next Valid Id: %ld\n", orderId);
    order_id = orderId;
    //! [nextvalidid]

    //client_state = ST_TICKOPTIONCOMPUTATIONOPERATION; 
    //client_state = ST_TICKDATAOPERATION; 
    //client_state = ST_REQTICKBYTICKDATA; 
    //client_state = ST_REQHISTORICALTICKS; 
    //client_state = ST_CONTFUT; 
    //client_state = ST_PNLSINGLE; 
    //client_state = ST_PNL; 
    //client_state = ST_DELAYEDTICKDATAOPERATION; 
    //client_state = ST_MARKETDEPTHOPERATION;
    //client_state = ST_REALTIMEBARS;
    //client_state = ST_MARKETDATATYPE;
    //client_state = ST_HISTORICALDATAREQUESTS;
    //client_state = ST_CONTRACTOPERATION;
    //client_state = ST_MARKETSCANNERS;
    //client_state = ST_FUNDAMENTALS;
    //client_state = ST_BULLETINS;
    //client_state = ST_ACCOUNTOPERATIONS;
    client_state = ST_ORDEROPERATIONS;
    //client_state = ST_OCASAMPLES;
    //client_state = ST_CONDITIONSAMPLES;
    //client_state = ST_BRACKETSAMPLES;
    //client_state = ST_HEDGESAMPLES;
    //client_state = ST_TESTALGOSAMPLES;
    //client_state = ST_FAORDERSAMPLES;
    //client_state = ST_FAOPERATIONS;
    //client_state = ST_DISPLAYGROUPS;
    //client_state = ST_MISCELANEOUS;
    //client_state = ST_FAMILYCODES;
    //client_state = ST_SYMBOLSAMPLES;
    //client_state = ST_REQMKTDEPTHEXCHANGES;
    //client_state = ST_REQNEWSTICKS;
    //client_state = ST_REQSMARTCOMPONENTS;
    //client_state = ST_NEWSPROVIDERS;
    //client_state = ST_REQNEWSARTICLE;
    //client_state = ST_REQHISTORICALNEWS;
    //client_state = ST_REQHEADTIMESTAMP;
    //client_state = ST_REQHISTOGRAMDATA;
    //client_state = ST_REROUTECFD;
    //client_state = ST_MARKETRULE;
    //client_state = ST_PING;
    //client_state = ST_WHATIFSAMPLES;
}


//////////////////////////////////////////////////////////////////
// methods
//! [connectack]
void InteractiveBrokers::connectAck() {
	if (!extra_auth && client_socket->asyncEConnect())
        client_socket->startApi();
}
//! [connectack]

void InteractiveBrokers::pnlOperation()
{
	//! [reqpnl]
    client_socket->reqPnL(7001, "DUD00029", "");
	//! [reqpnl]
	
    std::this_thread::sleep_for(std::chrono::seconds(2));

	//! [cancelpnl]
    client_socket->cancelPnL(7001);
	//! [cancelpnl] 
	
    client_state = ST_PNL_ACK;
}

void InteractiveBrokers::pnlSingleOperation()
{
	//! [reqpnlsingle]
    client_socket->reqPnLSingle(7002, "DUD00029", "", 268084);
	//! [reqpnlsingle]
	
    std::this_thread::sleep_for(std::chrono::seconds(2));

	//! [cancelpnlsingle]
    client_socket->cancelPnLSingle(7002);
	//! [cancelpnlsingle]
	
    client_state = ST_PNLSINGLE_ACK;
}

void InteractiveBrokers::tickDataOperation()
{
	/*** Requesting real time market data ***/
    std::this_thread::sleep_for(std::chrono::seconds(1));
    //! [reqmktdata]
	client_socket->reqMktData(1001, ContractSamples::StockComboContract(), "", false, false, TagValueListSPtr());
	client_socket->reqMktData(1002, ContractSamples::OptionWithLocalSymbol(), "", false, false, TagValueListSPtr());
	//! [reqmktdata]
	//! [reqmktdata_snapshot]
	client_socket->reqMktData(1003, ContractSamples::FutureComboContract(), "", true, false, TagValueListSPtr());
	//! [reqmktdata_snapshot]

	/*
	//! [regulatorysnapshot]
	// Each regulatory snapshot incurs a fee of 0.01 USD
	client_socket->reqMktData(1013, ContractSamples::USStock(), "", false, true, TagValueListSPtr());
	//! [regulatorysnapshot]
	*/
	
	//! [reqmktdata_genticks]
	//Requesting RTVolume (Time & Sales), shortable and Fundamental Ratios generic ticks
	client_socket->reqMktData(1004, ContractSamples::USStockAtSmart(), "233,236,258", false, false, TagValueListSPtr());
	//! [reqmktdata_genticks]

	//! [reqmktdata_contractnews]
	// Without the API news subscription this will generate an "invalid tick type" error
	client_socket->reqMktData(1005, ContractSamples::USStock(), "mdoff,292:BZ", false, false, TagValueListSPtr());
	client_socket->reqMktData(1006, ContractSamples::USStock(), "mdoff,292:BT", false, false, TagValueListSPtr());
	client_socket->reqMktData(1007, ContractSamples::USStock(), "mdoff,292:FLY", false, false, TagValueListSPtr());
	client_socket->reqMktData(1008, ContractSamples::USStock(), "mdoff,292:MT", false, false, TagValueListSPtr());
	//! [reqmktdata_contractnews]
	//! [reqmktdata_broadtapenews]
	client_socket->reqMktData(1009, ContractSamples::BTbroadtapeNewsFeed(), "mdoff,292", false, false, TagValueListSPtr());
	client_socket->reqMktData(1010, ContractSamples::BZbroadtapeNewsFeed(), "mdoff,292", false, false, TagValueListSPtr());
	client_socket->reqMktData(1011, ContractSamples::FLYbroadtapeNewsFeed(), "mdoff,292", false, false, TagValueListSPtr());
	client_socket->reqMktData(1012, ContractSamples::MTbroadtapeNewsFeed(), "mdoff,292", false, false, TagValueListSPtr());
	//! [reqmktdata_broadtapenews]

	//! [reqoptiondatagenticks]
	//Requesting data for an option contract will return the greek values
	client_socket->reqMktData(1013, ContractSamples::USOptionContract(), "", false, false, TagValueListSPtr());
	//! [reqoptiondatagenticks]
	
	//! [reqfuturesopeninterest]
	//Requesting data for a futures contract will return the futures open interest
	client_socket->reqMktData(1014, ContractSamples::SimpleFuture(), "mdoff,588", false, false, TagValueListSPtr());
	//! [reqfuturesopeninterest]

	//! [reqpreopenbidask]
	//Requesting data for a futures contract will return the pre-open bid/ask flag
	client_socket->reqMktData(1015, ContractSamples::SimpleFuture(), "", false, false, TagValueListSPtr());
	//! [reqpreopenbidask]

	//! [reqavgoptvolume]
	//Requesting data for a stock will return the average option volume
	client_socket->reqMktData(1016, ContractSamples::USStockAtSmart(), "mdoff,105", false, false, TagValueListSPtr());
	//! [reqavgoptvolume]

	std::this_thread::sleep_for(std::chrono::seconds(1));
	/*** Canceling the market data subscription ***/
	//! [cancelmktdata]
	client_socket->cancelMktData(1001);
	client_socket->cancelMktData(1002);
	client_socket->cancelMktData(1003);
	client_socket->cancelMktData(1014);
	client_socket->cancelMktData(1015);
	client_socket->cancelMktData(1016);
	//! [cancelmktdata]

	client_state = ST_TICKDATAOPERATION_ACK;
}

void InteractiveBrokers::tickOptionComputationOperation()
{
	/*** Requesting real time market data ***/
	std::this_thread::sleep_for(std::chrono::seconds(1));
	//! [reqmktdata]
	client_socket->reqMktData(2001, ContractSamples::FuturesOnOptions(), "", false, false, TagValueListSPtr());
	//! [reqmktdata]

	std::this_thread::sleep_for(std::chrono::seconds(10));
	/*** Canceling the market data subscription ***/
	//! [cancelmktdata]
	client_socket->cancelMktData(2001);
	//! [cancelmktdata]

	client_state = ST_TICKOPTIONCOMPUTATIONOPERATION_ACK;
}

void InteractiveBrokers::delayedTickDataOperation()
{
	/*** Requesting delayed market data ***/

	//! [reqmktdata_delayedmd]
	client_socket->reqMarketDataType(4); // send delayed-frozen (4) market data type
	client_socket->reqMktData(1013, ContractSamples::HKStk(), "", false, false, TagValueListSPtr());
	client_socket->reqMktData(1014, ContractSamples::USOptionContract(), "", false, false, TagValueListSPtr());
	//! [reqmktdata_delayedmd]

	std::this_thread::sleep_for(std::chrono::seconds(10));

	/*** Canceling the delayed market data subscription ***/
	//! [cancelmktdata_delayedmd]
	client_socket->cancelMktData(1013);
	client_socket->cancelMktData(1014);
	//! [cancelmktdata_delayedmd]

	client_state = ST_DELAYEDTICKDATAOPERATION_ACK;
}

void InteractiveBrokers::marketDepthOperations()
{
	/*** Requesting the Deep Book ***/
	//! [reqmarketdepth]
	client_socket->reqMktDepth(2001, ContractSamples::EurGbpFx(), 5, false, TagValueListSPtr());
	//! [reqmarketdepth]
	std::this_thread::sleep_for(std::chrono::seconds(2));
	/*** Canceling the Deep Book request ***/
	//! [cancelmktdepth]
	client_socket->cancelMktDepth(2001, false);
	//! [cancelmktdepth]

	/*** Requesting the Deep Book ***/
	//! [reqmarketdepth]
	client_socket->reqMktDepth(2002, ContractSamples::EuropeanStock(), 5, true, TagValueListSPtr());
	//! [reqmarketdepth]
	std::this_thread::sleep_for(std::chrono::seconds(5));
	/*** Canceling the Deep Book request ***/
	//! [cancelmktdepth]
	client_socket->cancelMktDepth(2002, true);
	//! [cancelmktdepth]

	client_state = ST_MARKETDEPTHOPERATION_ACK;
}

void InteractiveBrokers::realTimeBars()
{
	/*** Requesting real time bars ***/
	//! [reqrealtimebars]
	client_socket->reqRealTimeBars(3001, ContractSamples::EurGbpFx(), 5, "MIDPOINT", true, TagValueListSPtr());
	//! [reqrealtimebars]
	std::this_thread::sleep_for(std::chrono::seconds(2));
	/*** Canceling real time bars ***/
    //! [cancelrealtimebars]
	client_socket->cancelRealTimeBars(3001);
    //! [cancelrealtimebars]

	client_state = ST_REALTIMEBARS_ACK;
}

void InteractiveBrokers::marketDataType()
{
	//! [reqmarketdatatype]
	/*** By default only real-time (1) market data is enabled
		 Sending frozen (2) enables frozen market data
		 Sending delayed (3) enables delayed market data and disables delayed-frozen market data
		 Sending delayed-frozen (4) enables delayed and delayed-frozen market data
		 Sending real-time (1) disables frozen, delayed and delayed-frozen market data ***/
	client_socket->reqMarketDataType(2);
	//! [reqmarketdatatype]

	client_state = ST_MARKETDATATYPE_ACK;
}

void InteractiveBrokers::historicalDataRequests()
{
	/*** Requesting historical data ***/
	//! [reqhistoricaldata]
	std::time_t rawtime;
    std::tm* timeinfo;
    char queryTime [80];

	std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);
	std::strftime(queryTime, 80, "%Y%m%d %H:%M:%S", timeinfo);

	client_socket->reqHistoricalData(4001, ContractSamples::EurGbpFx(), queryTime, "1 M", "1 day", "MIDPOINT", 1, 1, false, TagValueListSPtr());
	client_socket->reqHistoricalData(4002, ContractSamples::EuropeanStock(), queryTime, "10 D", "1 min", "TRADES", 1, 1, false, TagValueListSPtr());
	//! [reqhistoricaldata]
	std::this_thread::sleep_for(std::chrono::seconds(2));
	/*** Canceling historical data requests ***/
	client_socket->cancelHistoricalData(4001);
	client_socket->cancelHistoricalData(4002);

	client_state = ST_HISTORICALDATAREQUESTS_ACK;
}

void InteractiveBrokers::optionsOperations()
{
	//! [reqsecdefoptparams]
	client_socket->reqSecDefOptParams(0, "IBM", "", "STK", 8314);
	//! [reqsecdefoptparams]

	//! [calculateimpliedvolatility]
	client_socket->calculateImpliedVolatility(5001, ContractSamples::NormalOption(), 5, 85, TagValueListSPtr());
	//! [calculateimpliedvolatility]

	//** Canceling implied volatility ***
	client_socket->cancelCalculateImpliedVolatility(5001);

	//! [calculateoptionprice]
	client_socket->calculateOptionPrice(5002, ContractSamples::NormalOption(), 0.22, 85, TagValueListSPtr());
	//! [calculateoptionprice]

	//** Canceling option's price calculation ***
	client_socket->cancelCalculateOptionPrice(5002);

	//! [exercise_options]
	//** Exercising options ***
	client_socket->exerciseOptions(5003, ContractSamples::OptionWithTradingClass(), 1, 1, "", 1);
	//! [exercise_options]

	client_state = ST_OPTIONSOPERATIONS_ACK;
}

void InteractiveBrokers::contractOperations()
{
	client_socket->reqContractDetails(209, ContractSamples::EurGbpFx());
	std::this_thread::sleep_for(std::chrono::seconds(2));
	//! [reqcontractdetails]
	client_socket->reqContractDetails(210, ContractSamples::OptionForQuery());
	client_socket->reqContractDetails(212, ContractSamples::IBMBond());
	client_socket->reqContractDetails(213, ContractSamples::IBKRStk());
	client_socket->reqContractDetails(214, ContractSamples::Bond());
	client_socket->reqContractDetails(215, ContractSamples::FuturesOnOptions());
	client_socket->reqContractDetails(216, ContractSamples::SimpleFuture());
	//! [reqcontractdetails]

	//! [reqcontractdetailsnews]
	client_socket->reqContractDetails(211, ContractSamples::NewsFeedForQuery());
	//! [reqcontractdetailsnews]

	client_state = ST_CONTRACTOPERATION_ACK;
}

void InteractiveBrokers::marketScanners()
{
	/*** Requesting all available parameters which can be used to build a scanner request ***/
	//! [reqscannerparameters]
	client_socket->reqScannerParameters();
	//! [reqscannerparameters]
	std::this_thread::sleep_for(std::chrono::seconds(2));

	/*** Triggering a scanner subscription ***/
	//! [reqscannersubscription]
	client_socket->reqScannerSubscription(7001, ScannerSubscriptionSamples::HotUSStkByVolume(), TagValueListSPtr(), TagValueListSPtr());
	
	TagValueSPtr t1(new TagValue("usdMarketCapAbove", "10000"));
	TagValueSPtr t2(new TagValue("optVolumeAbove", "1000"));
	TagValueSPtr t3(new TagValue("usdMarketCapAbove", "100000000"));

	TagValueListSPtr TagValues(new TagValueList());
	TagValues->push_back(t1);
	TagValues->push_back(t2);
	TagValues->push_back(t3);

	client_socket->reqScannerSubscription(7002, ScannerSubscriptionSamples::HotUSStkByVolume(), TagValueListSPtr(), TagValues); // requires TWS v973+
	
	//! [reqscannersubscription]

	//! [reqcomplexscanner]

	TagValueSPtr t(new TagValue("underConID", "265598"));
	TagValueListSPtr AAPLConIDTag(new TagValueList());
	AAPLConIDTag->push_back(t);
	client_socket->reqScannerSubscription(7003, ScannerSubscriptionSamples::ComplexOrdersAndTrades(), TagValueListSPtr(), AAPLConIDTag); // requires TWS v975+

	//! [reqcomplexscanner]

	std::this_thread::sleep_for(std::chrono::seconds(2));
	/*** Canceling the scanner subscription ***/
	//! [cancelscannersubscription]
	client_socket->cancelScannerSubscription(7001);
	client_socket->cancelScannerSubscription(7002);
	//! [cancelscannersubscription]

	client_state = ST_MARKETSCANNERS_ACK;
}

void InteractiveBrokers::fundamentals()
{
	/*** Requesting Fundamentals ***/
	//! [reqfundamentaldata]
	client_socket->reqFundamentalData(8001, ContractSamples::USStock(), "ReportsFinSummary", TagValueListSPtr());
	//! [reqfundamentaldata]
	std::this_thread::sleep_for(std::chrono::seconds(2));

	/*** Canceling fundamentals request ***/
	//! [cancelfundamentaldata]
	client_socket->cancelFundamentalData(8001);
	//! [cancelfundamentaldata]

	client_state = ST_FUNDAMENTALS_ACK;
}

void InteractiveBrokers::bulletins()
{
	/*** Requesting Interactive Broker's news bulletins */
	//! [reqnewsbulletins]
	client_socket->reqNewsBulletins(true);
	//! [reqnewsbulletins]
	std::this_thread::sleep_for(std::chrono::seconds(2));
	/*** Canceling IB's news bulletins ***/
	//! [cancelnewsbulletins]
	client_socket->cancelNewsBulletins();
	//! [cancelnewsbulletins]

	client_state = ST_BULLETINS_ACK;
}

void InteractiveBrokers::accountOperations()
{
	/*** Requesting managed accounts***/
	//! [reqmanagedaccts]
	client_socket->reqManagedAccts();
	//! [reqmanagedaccts]
	std::this_thread::sleep_for(std::chrono::seconds(2));
	/*** Requesting accounts' summary ***/
	//! [reqaaccountsummary]
	client_socket->reqAccountSummary(9001, "All", AccountSummaryTags::getAllTags());
	//! [reqaaccountsummary]
	std::this_thread::sleep_for(std::chrono::seconds(2));
	//! [reqaaccountsummaryledger]
	client_socket->reqAccountSummary(9002, "All", "$LEDGER");
	//! [reqaaccountsummaryledger]
	std::this_thread::sleep_for(std::chrono::seconds(2));
	//! [reqaaccountsummaryledgercurrency]
	client_socket->reqAccountSummary(9003, "All", "$LEDGER:EUR");
	//! [reqaaccountsummaryledgercurrency]
	std::this_thread::sleep_for(std::chrono::seconds(2));
	//! [reqaaccountsummaryledgerall]
	client_socket->reqAccountSummary(9004, "All", "$LEDGER:ALL");
	//! [reqaaccountsummaryledgerall]
	std::this_thread::sleep_for(std::chrono::seconds(2));
	//! [cancelaaccountsummary]
	client_socket->cancelAccountSummary(9001);
	client_socket->cancelAccountSummary(9002);
	client_socket->cancelAccountSummary(9003);
	client_socket->cancelAccountSummary(9004);
	//! [cancelaaccountsummary]
	std::this_thread::sleep_for(std::chrono::seconds(2));
	/*** Subscribing to an account's information. Only one at a time! ***/
	//! [reqaaccountupdates]
	client_socket->reqAccountUpdates(true, "U150462");
	//! [reqaaccountupdates]
	std::this_thread::sleep_for(std::chrono::seconds(2));
	//! [cancelaaccountupdates]
	client_socket->reqAccountUpdates(false, "U150462");
	//! [cancelaaccountupdates]
	std::this_thread::sleep_for(std::chrono::seconds(2));

	//! [reqaaccountupdatesmulti]
	client_socket->reqAccountUpdatesMulti(9002, "U150462", "EUstocks", true);
	//! [reqaaccountupdatesmulti]
	std::this_thread::sleep_for(std::chrono::seconds(2));

	/*** Requesting all accounts' positions. ***/
	//! [reqpositions]
	client_socket->reqPositions();
	//! [reqpositions]
	std::this_thread::sleep_for(std::chrono::seconds(2));
	//! [cancelpositions]
	client_socket->cancelPositions();
	//! [cancelpositions]

	//! [reqpositionsmulti]
	client_socket->reqPositionsMulti(9003, "U150462", "EUstocks");
	//! [reqpositionsmulti]

	client_state = ST_ACCOUNTOPERATIONS_ACK;
}

void InteractiveBrokers::orderOperations()
{
	/*** Requesting the next valid id ***/
	//! [reqids]
	//The parameter is always ignored.
	client_socket->reqIds(-1);
	//! [reqids]
	//! [reqallopenorders]
	client_socket->reqAllOpenOrders();
	//! [reqallopenorders]
	//! [reqautoopenorders]
	client_socket->reqAutoOpenOrders(true);
	//! [reqautoopenorders]
	//! [reqopenorders]
	client_socket->reqOpenOrders();
	//! [reqopenorders]

	/*** Placing/modifying an order - remember to ALWAYS increment the nextValidId after placing an order so it can be used for the next one! ***/
    //! [order_submission]
	client_socket->placeOrder(order_id++, ContractSamples::USStock(), OrderSamples::LimitOrder("SELL", 1, 50));
    //! [order_submission]

	//client_socket->placeOrder(order_id++, ContractSamples::OptionAtBox(), OrderSamples::Block("BUY", 50, 20));
	//client_socket->placeOrder(order_id++, ContractSamples::OptionAtBox(), OrderSamples::BoxTop("SELL", 10));
	//client_socket->placeOrder(order_id++, ContractSamples::FutureComboContract(), OrderSamples::ComboLimitOrder("SELL", 1, 1, false));
	//client_socket->placeOrder(order_id++, ContractSamples::StockComboContract(), OrderSamples::ComboMarketOrder("BUY", 1, false));
	//client_socket->placeOrder(order_id++, ContractSamples::OptionComboContract(), OrderSamples::ComboMarketOrder("BUY", 1, true));
	//client_socket->placeOrder(order_id++, ContractSamples::StockComboContract(), OrderSamples::LimitOrderForComboWithLegPrices("BUY", 1, std::vector<double>(10, 5), true));
	//client_socket->placeOrder(order_id++, ContractSamples::USStock(), OrderSamples::Discretionary("SELL", 1, 45, 0.5));
	//client_socket->placeOrder(order_id++, ContractSamples::OptionAtBox(), OrderSamples::LimitIfTouched("BUY", 1, 30, 34));
	//client_socket->placeOrder(order_id++, ContractSamples::USStock(), OrderSamples::LimitOnClose("SELL", 1, 34));
	//client_socket->placeOrder(order_id++, ContractSamples::USStock(), OrderSamples::LimitOnOpen("BUY", 1, 35));
	//client_socket->placeOrder(order_id++, ContractSamples::USStock(), OrderSamples::MarketIfTouched("BUY", 1, 35));
	//client_socket->placeOrder(order_id++, ContractSamples::USStock(), OrderSamples::MarketOnClose("SELL", 1));
	//client_socket->placeOrder(order_id++, ContractSamples::USStock(), OrderSamples::MarketOnOpen("BUY", 1));
	//client_socket->placeOrder(order_id++, ContractSamples::USStock(), OrderSamples::MarketOrder("SELL", 1));
	//client_socket->placeOrder(order_id++, ContractSamples::USStock(), OrderSamples::MarketToLimit("BUY", 1));
	//client_socket->placeOrder(order_id++, ContractSamples::OptionAtIse(), OrderSamples::MidpointMatch("BUY", 1));
	//client_socket->placeOrder(order_id++, ContractSamples::USStock(), OrderSamples::Stop("SELL", 1, 34.4));
	//client_socket->placeOrder(order_id++, ContractSamples::USStock(), OrderSamples::StopLimit("BUY", 1, 35, 33));
	//client_socket->placeOrder(order_id++, ContractSamples::USStock(), OrderSamples::StopWithProtection("SELL", 1, 45));
	//client_socket->placeOrder(order_id++, ContractSamples::USStock(), OrderSamples::SweepToFill("BUY", 1, 35));
	//client_socket->placeOrder(order_id++, ContractSamples::USStock(), OrderSamples::TrailingStop("SELL", 1, 0.5, 30));
	//client_socket->placeOrder(order_id++, ContractSamples::USStock(), OrderSamples::TrailingStopLimit("BUY", 1, 2, 5, 50));
	
	//! [place_midprice]
	client_socket->placeOrder(order_id++, ContractSamples::USStockAtSmart(), OrderSamples::Midprice("BUY", 1, 150));
	//! [place_midprice]
	
	//! [place order with cashQty]
	client_socket->placeOrder(order_id++, ContractSamples::USStockAtSmart(), OrderSamples::LimitOrderWithCashQty("BUY", 1, 30, 5000));
	//! [place order with cashQty]

	std::this_thread::sleep_for(std::chrono::seconds(1));

	/*** Cancel one order ***/
	//! [cancelorder]
	client_socket->cancelOrder(order_id-1);
	//! [cancelorder]
	
	/*** Cancel all orders for all accounts ***/
	//! [reqglobalcancel]
	client_socket->reqGlobalCancel();
	//! [reqglobalcancel]

	/*** Request the day's executions ***/
	//! [reqexecutions]
	client_socket->reqExecutions(10001, ExecutionFilter());
	//! [reqexecutions]

	//! [reqcompletedorders]
	client_socket->reqCompletedOrders(false);
	//! [reqcompletedorders]

	client_state = ST_ORDEROPERATIONS_ACK;
}

void InteractiveBrokers::ocaSamples()
{
	//OCA ORDER
	//! [ocasubmit]
	std::vector<Order> ocaOrders;
	ocaOrders.push_back(OrderSamples::LimitOrder("BUY", 1, 10));
	ocaOrders.push_back(OrderSamples::LimitOrder("BUY", 1, 11));
	ocaOrders.push_back(OrderSamples::LimitOrder("BUY", 1, 12));
	for(unsigned int i = 0; i < ocaOrders.size(); i++){
		OrderSamples::OneCancelsAll("TestOca", ocaOrders[i], 2);
		client_socket->placeOrder(order_id++, ContractSamples::USStock(), ocaOrders[i]);
	}
	//! [ocasubmit]

	client_state = ST_OCASAMPLES_ACK;
}

void InteractiveBrokers::conditionSamples()
{
	//! [order_conditioning_activate]
	Order lmt = OrderSamples::LimitOrder("BUY", 100, 10);
	//Order will become active if conditioning criteria is met
	PriceCondition* priceCondition = dynamic_cast<PriceCondition *>(OrderSamples::Price_Condition(208813720, "SMART", 600, false, false));
	ExecutionCondition* execCondition = dynamic_cast<ExecutionCondition *>(OrderSamples::Execution_Condition("EUR.USD", "CASH", "IDEALPRO", true));
	MarginCondition* marginCondition = dynamic_cast<MarginCondition *>(OrderSamples::Margin_Condition(30, true, false));
	PercentChangeCondition* pctChangeCondition = dynamic_cast<PercentChangeCondition *>(OrderSamples::Percent_Change_Condition(15.0, 208813720, "SMART", true, true));
	TimeCondition* timeCondition = dynamic_cast<TimeCondition *>(OrderSamples::Time_Condition("20160118 23:59:59", true, false));
	VolumeCondition* volumeCondition = dynamic_cast<VolumeCondition *>(OrderSamples::Volume_Condition(208813720, "SMART", false, 100, true));

	lmt.conditions.push_back(std::shared_ptr<PriceCondition>(priceCondition));
	lmt.conditions.push_back(std::shared_ptr<ExecutionCondition>(execCondition));
	lmt.conditions.push_back(std::shared_ptr<MarginCondition>(marginCondition));
	lmt.conditions.push_back(std::shared_ptr<PercentChangeCondition>(pctChangeCondition));
	lmt.conditions.push_back(std::shared_ptr<TimeCondition>(timeCondition));
	lmt.conditions.push_back(std::shared_ptr<VolumeCondition>(volumeCondition));
	client_socket->placeOrder(order_id++, ContractSamples::USStock(),lmt);
	//! [order_conditioning_activate]

	//Conditions can make the order active or cancel it. Only LMT orders can be conditionally canceled.
	//! [order_conditioning_cancel]
	Order lmt2 = OrderSamples::LimitOrder("BUY", 100, 20);
	//The active order will be cancelled if conditioning criteria is met
	lmt2.conditionsCancelOrder = true;
	PriceCondition* priceCondition2 = dynamic_cast<PriceCondition *>(OrderSamples::Price_Condition(208813720, "SMART", 600, false, false));
	lmt2.conditions.push_back(std::shared_ptr<PriceCondition>(priceCondition2));
	client_socket->placeOrder(order_id++, ContractSamples::EuropeanStock(), lmt2);
	//! [order_conditioning_cancel]

	client_state = ST_CONDITIONSAMPLES_ACK;
}

void InteractiveBrokers::bracketSample(){
	Order parent;
	Order takeProfit;
	Order stopLoss;
	//! [bracketsubmit]
	OrderSamples::BracketOrder(order_id++, parent, takeProfit, stopLoss, "BUY", 100, 30, 40, 20);
	client_socket->placeOrder(parent.orderId, ContractSamples::EuropeanStock(), parent);
	client_socket->placeOrder(takeProfit.orderId, ContractSamples::EuropeanStock(), takeProfit);
	client_socket->placeOrder(stopLoss.orderId, ContractSamples::EuropeanStock(), stopLoss);
	//! [bracketsubmit]

	client_state = ST_BRACKETSAMPLES_ACK;
}

void InteractiveBrokers::hedgeSample(){
	//F Hedge order
	//! [hedgesubmit]
	//Parent order on a contract which currency differs from your base currency
	Order parent = OrderSamples::LimitOrder("BUY", 100, 10);
	parent.orderId = order_id++;
	parent.transmit = false;
	//Hedge on the currency conversion
	Order hedge = OrderSamples::MarketFHedge(parent.orderId, "BUY");
	//Place the parent first...
	client_socket->placeOrder(parent.orderId, ContractSamples::EuropeanStock(), parent);
	//Then the hedge order
	client_socket->placeOrder(order_id++, ContractSamples::EurGbpFx(), hedge);
	//! [hedgesubmit]

	client_state = ST_HEDGESAMPLES_ACK;
}

void InteractiveBrokers::testAlgoSamples(){
	//! [algo_base_order]
	Order baseOrder = OrderSamples::LimitOrder("BUY", 1000, 1);
	//! [algo_base_order]

	//! [arrivalpx]
	AvailableAlgoParams::FillArrivalPriceParams(baseOrder, 0.1, "Aggressive", "09:00:00 CET", "16:00:00 CET", true, true, 100000);
	client_socket->placeOrder(order_id++, ContractSamples::USStockAtSmart(), baseOrder);
	//! [arrivalpx]

	//! [darkice]
	AvailableAlgoParams::FillDarkIceParams(baseOrder, 10, "09:00:00 CET", "16:00:00 CET", true, 100000);
	client_socket->placeOrder(order_id++, ContractSamples::USStockAtSmart(), baseOrder);
	//! [darkice]

	//! [ad]
	// The Time Zone in "startTime" and "endTime" attributes is ignored and always defaulted to GMT
	AvailableAlgoParams::FillAccumulateDistributeParams(baseOrder, 10, 60, true, true, 1, true, true, "20161010-12:00:00 GMT", "20161010-16:00:00 GMT");
	client_socket->placeOrder(order_id++, ContractSamples::USStockAtSmart(), baseOrder);
	//! [ad]

	//! [twap]
	AvailableAlgoParams::FillTwapParams(baseOrder, "Marketable", "09:00:00 CET", "16:00:00 CET", true, 100000);
	client_socket->placeOrder(order_id++, ContractSamples::USStockAtSmart(), baseOrder);
	//! [twap]

	//! [vwap]
	AvailableAlgoParams::FillBalanceImpactRiskParams(baseOrder, 0.1, "Aggressive", true);
	client_socket->placeOrder(order_id++, ContractSamples::USStockAtSmart(), baseOrder);
	 //! [vwap]

	//! [balanceimpactrisk]
	AvailableAlgoParams::FillBalanceImpactRiskParams(baseOrder, 0.1, "Aggressive", true);
	client_socket->placeOrder(order_id++, ContractSamples::USStockAtSmart(), baseOrder);
	//! [balanceimpactrisk]

	//! [minimpact]
	AvailableAlgoParams::FillMinImpactParams(baseOrder, 0.3);
	client_socket->placeOrder(order_id++, ContractSamples::USStockAtSmart(), baseOrder);
	//! [minimpact]

	//! [adaptive]
	AvailableAlgoParams::FillAdaptiveParams(baseOrder, "Normal");
	client_socket->placeOrder(order_id++, ContractSamples::USStockAtSmart(), baseOrder);
	//! [adaptive]

	//! [closepx]
    AvailableAlgoParams::FillClosePriceParams(baseOrder, 0.5, "Neutral", "12:00:00 EST", true, 100000);
    client_socket->placeOrder(order_id++, ContractSamples::USStockAtSmart(), baseOrder);
    //! [closepx]
    
    //! [pctvol]
    AvailableAlgoParams::FillPctVolParams(baseOrder, 0.5, "12:00:00 EST", "14:00:00 EST", true, 100000);
    client_socket->placeOrder(order_id++, ContractSamples::USStockAtSmart(), baseOrder);
    //! [pctvol]               
    
    //! [pctvolpx]
    AvailableAlgoParams::FillPriceVariantPctVolParams(baseOrder, 0.1, 0.05, 0.01, 0.2, "12:00:00 EST", "14:00:00 EST", true, 100000);
    client_socket->placeOrder(order_id++, ContractSamples::USStockAtSmart(), baseOrder);
    //! [pctvolpx]
    
    //! [pctvolsz]
    AvailableAlgoParams::FillSizeVariantPctVolParams(baseOrder, 0.2, 0.4, "12:00:00 EST", "14:00:00 EST", true, 100000);
    client_socket->placeOrder(order_id++, ContractSamples::USStockAtSmart(), baseOrder);
    //! [pctvolsz]
    
    //! [pctvoltm]
    AvailableAlgoParams::FillTimeVariantPctVolParams(baseOrder, 0.2, 0.4, "12:00:00 EST", "14:00:00 EST", true, 100000);
    client_socket->placeOrder(order_id++, ContractSamples::USStockAtSmart(), baseOrder);
    //! [pctvoltm]

	//! [jeff_vwap_algo]
	AvailableAlgoParams::FillJefferiesVWAPParams(baseOrder, "10:00:00 EST", "16:00:00 EST", 10, 10, "Exclude_Both", 130, 135, 1, 10, "Patience", false, "Midpoint");
	client_socket->placeOrder(order_id++, ContractSamples::JefferiesContract(), baseOrder);
	//! [jeff_vwap_algo]

	//! [csfb_inline_algo]
	AvailableAlgoParams::FillCSFBInlineParams(baseOrder, "10:00:00 EST", "16:00:00 EST", "Patient", 10, 20, 100, "Default", false, 40, 100, 100, 35);
	client_socket->placeOrder(order_id++, ContractSamples::CSFBContract(), baseOrder);
	//! [csfb_inline_algo]
	
	client_state = ST_TESTALGOSAMPLES_ACK;
}

void InteractiveBrokers::financialAdvisorOrderSamples()
{
	//! [faorderoneaccount]
	Order faOrderOneAccount = OrderSamples::MarketOrder("BUY", 100);
	// Specify the Account Number directly
	faOrderOneAccount.account = "DU119915";
	client_socket->placeOrder(order_id++, ContractSamples::USStock(), faOrderOneAccount);
	//! [faorderoneaccount]
	std::this_thread::sleep_for(std::chrono::seconds(1));

	//! [faordergroupequalquantity]
	Order faOrderGroupEQ = OrderSamples::LimitOrder("SELL", 200, 2000);
	faOrderGroupEQ.faGroup = "Group_Equal_Quantity";
	faOrderGroupEQ.faMethod = "EqualQuantity";
	client_socket->placeOrder(order_id++, ContractSamples::SimpleFuture(), faOrderGroupEQ);
	//! [faordergroupequalquantity]
	std::this_thread::sleep_for(std::chrono::seconds(1));

	//! [faordergrouppctchange]
	Order faOrderGroupPC;
	faOrderGroupPC.action = "BUY";
	faOrderGroupPC.orderType = "MKT";
	// You should not specify any order quantity for PctChange allocation method
	faOrderGroupPC.faGroup = "Pct_Change";
	faOrderGroupPC.faMethod = "PctChange";
	faOrderGroupPC.faPercentage = "100";
	client_socket->placeOrder(order_id++, ContractSamples::EurGbpFx(), faOrderGroupPC);
	//! [faordergrouppctchange]
	std::this_thread::sleep_for(std::chrono::seconds(1));

	//! [faorderprofile]
	Order faOrderProfile = OrderSamples::LimitOrder("BUY", 200, 100);
	faOrderProfile.faProfile = "Percent_60_40";
	client_socket->placeOrder(order_id++, ContractSamples::EuropeanStock(), faOrderProfile);
	//! [faorderprofile]

	//! [modelorder]
	Order modelOrder = OrderSamples::LimitOrder("BUY", 200, 100);
	modelOrder.account = "DF12345";
	modelOrder.modelCode = "Technology";
	client_socket->placeOrder(order_id++, ContractSamples::USStock(), modelOrder);
	//! [modelorder]

	client_state = ST_FAORDERSAMPLES_ACK;
}

void InteractiveBrokers::financialAdvisorOperations()
{
	/*** Requesting FA information ***/
	//! [requestfaaliases]
	client_socket->requestFA(faDataType::ALIASES);
	//! [requestfaaliases]

	//! [requestfagroups]
	client_socket->requestFA(faDataType::GROUPS);
	//! [requestfagroups]

	//! [requestfaprofiles]
	client_socket->requestFA(faDataType::PROFILES);
	//! [requestfaprofiles]

	/*** Replacing FA information - Fill in with the appropriate XML string. ***/
	//! [replacefaonegroup]
	client_socket->replaceFA(faDataType::GROUPS, FAMethodSamples::FAOneGroup());
	//! [replacefaonegroup]

	//! [replacefatwogroups]
	client_socket->replaceFA(faDataType::GROUPS, FAMethodSamples::FATwoGroups());
	//! [replacefatwogroups]

	//! [replacefaoneprofile]
	client_socket->replaceFA(faDataType::PROFILES, FAMethodSamples::FAOneProfile());
	//! [replacefaoneprofile]

	//! [replacefatwoprofiles]
	client_socket->replaceFA(faDataType::PROFILES, FAMethodSamples::FATwoProfiles());
	//! [replacefatwoprofiles]

	//! [reqSoftDollarTiers]
	client_socket->reqSoftDollarTiers(4001);
	//! [reqSoftDollarTiers]

	client_state = ST_FAOPERATIONS_ACK;
}

void InteractiveBrokers::testDisplayGroups(){
	//! [querydisplaygroups]
	client_socket->queryDisplayGroups(9001);
	//! [querydisplaygroups]

	std::this_thread::sleep_for(std::chrono::seconds(1));

	//! [subscribetogroupevents]
	client_socket->subscribeToGroupEvents(9002, 1);
	//! [subscribetogroupevents]

	std::this_thread::sleep_for(std::chrono::seconds(1));

	//! [updatedisplaygroup]
	client_socket->updateDisplayGroup(9002, "8314@SMART");
	//! [updatedisplaygroup]

	std::this_thread::sleep_for(std::chrono::seconds(1));

	//! [subscribefromgroupevents]
	client_socket->unsubscribeFromGroupEvents(9002);
	//! [subscribefromgroupevents]

	client_state = ST_TICKDATAOPERATION_ACK;
}

void InteractiveBrokers::miscelaneous()
{
	/*** Request TWS' current time ***/
	client_socket->reqCurrentTime();
	/*** Setting TWS logging level  ***/
	client_socket->setServerLogLevel(5);

	client_state = ST_MISCELANEOUS_ACK;
}

void InteractiveBrokers::reqFamilyCodes()
{
	/*** Request TWS' family codes ***/
	//! [reqfamilycodes]
	client_socket->reqFamilyCodes();
	//! [reqfamilycodes]

	client_state = ST_FAMILYCODES_ACK;
}

void InteractiveBrokers::reqMatchingSymbols()
{
	/*** Request TWS' mathing symbols ***/
	//! [reqmatchingsymbols]
	client_socket->reqMatchingSymbols(11001, "IBM");
	//! [reqmatchingsymbols]
	client_state = ST_SYMBOLSAMPLES_ACK;
}

void InteractiveBrokers::reqMktDepthExchanges()
{
	/*** Request TWS' market depth exchanges ***/
	//! [reqMktDepthExchanges]
	client_socket->reqMktDepthExchanges();
	//! [reqMktDepthExchanges]

	client_state = ST_REQMKTDEPTHEXCHANGES_ACK;
}

void InteractiveBrokers::reqNewsTicks()
{
	//! [reqmktdata_ticknews]
	client_socket->reqMktData(12001, ContractSamples::USStockAtSmart(), "mdoff,292", false, false, TagValueListSPtr());
	//! [reqmktdata_ticknews]

	std::this_thread::sleep_for(std::chrono::seconds(5));

	//! [cancelmktdata2]
	client_socket->cancelMktData(12001);
	//! [cancelmktdata2]

	client_state = ST_REQNEWSTICKS_ACK;
}

void InteractiveBrokers::reqSmartComponents()
{
	static bool bFirstRun = true;

	if (bFirstRun) {
		client_socket->reqMktData(13001, ContractSamples::USStockAtSmart(), "", false, false, TagValueListSPtr());

		bFirstRun = false;
	}

	std::this_thread::sleep_for(std::chrono::seconds(5));

	if (m_bboExchange.size() > 0) {
		client_socket->cancelMktData(13001);

		//! [reqsmartcomponents]
		client_socket->reqSmartComponents(13002, m_bboExchange);
		//! [reqsmartcomponents]
		client_state = ST_REQSMARTCOMPONENTS_ACK;
	}
}

void InteractiveBrokers::reqNewsProviders()
{
	/*** Request TWS' news providers ***/
	//! [reqNewsProviders]
	client_socket->reqNewsProviders();
	//! [reqNewsProviders]

	client_state = ST_NEWSPROVIDERS_ACK;
}

void InteractiveBrokers::reqNewsArticle()
{
	/*** Request TWS' news article ***/
	//! [reqNewsArticle]
	TagValueList* list = new TagValueList();
	// list->push_back((TagValueSPtr)new TagValue("manual", "1"));
	client_socket->reqNewsArticle(12001, "MST", "MST$06f53098", TagValueListSPtr(list));
	//! [reqNewsArticle]

	client_state = ST_REQNEWSARTICLE_ACK;
}

void InteractiveBrokers::reqHistoricalNews(){

	//! [reqHistoricalNews]
	TagValueList* list = new TagValueList();
	list->push_back((TagValueSPtr)new TagValue("manual", "1"));
	client_socket->reqHistoricalNews(12001, 8314, "BZ+FLY", "", "", 5, TagValueListSPtr(list));
	//! [reqHistoricalNews]

	std::this_thread::sleep_for(std::chrono::seconds(1));

	client_state = ST_REQHISTORICALNEWS_ACK;
}


void InteractiveBrokers::reqHeadTimestamp() {
	//! [reqHeadTimeStamp]
	client_socket->reqHeadTimestamp(14001, ContractSamples::EurGbpFx(), "MIDPOINT", 1, 1);
	//! [reqHeadTimeStamp]	
	std::this_thread::sleep_for(std::chrono::seconds(1));
	//! [cancelHeadTimestamp]
	client_socket->cancelHeadTimestamp(14001);
	//! [cancelHeadTimestamp]
	
	client_state = ST_REQHEADTIMESTAMP_ACK;
}

void InteractiveBrokers::reqHistogramData() {
	//! [reqHistogramData]
	client_socket->reqHistogramData(15001, ContractSamples::IBMUSStockAtSmart(), false, "1 weeks");
	//! [reqHistogramData]
	std::this_thread::sleep_for(std::chrono::seconds(2));
	//! [cancelHistogramData]
	client_socket->cancelHistogramData(15001);
	//! [cancelHistogramData]
	client_state = ST_REQHISTOGRAMDATA_ACK;
}

void InteractiveBrokers::rerouteCFDOperations()
{
    //! [reqmktdatacfd]
	client_socket->reqMktData(16001, ContractSamples::USStockCFD(), "", false, false, TagValueListSPtr());
    std::this_thread::sleep_for(std::chrono::seconds(1));
	client_socket->reqMktData(16002, ContractSamples::EuropeanStockCFD(), "", false, false, TagValueListSPtr());
    std::this_thread::sleep_for(std::chrono::seconds(1));
	client_socket->reqMktData(16003, ContractSamples::CashCFD(), "", false, false, TagValueListSPtr());
	std::this_thread::sleep_for(std::chrono::seconds(1));
	//! [reqmktdatacfd]

    //! [reqmktdepthcfd]
	client_socket->reqMktDepth(16004, ContractSamples::USStockCFD(), 10, false, TagValueListSPtr());
    std::this_thread::sleep_for(std::chrono::seconds(1));
	client_socket->reqMktDepth(16005, ContractSamples::EuropeanStockCFD(), 10, false, TagValueListSPtr());
    std::this_thread::sleep_for(std::chrono::seconds(1));
	client_socket->reqMktDepth(16006, ContractSamples::CashCFD(), 10, false, TagValueListSPtr());
	std::this_thread::sleep_for(std::chrono::seconds(1));
	//! [reqmktdepthcfd]

	client_state = ST_REROUTECFD_ACK;
}

void InteractiveBrokers::marketRuleOperations()
{
	client_socket->reqContractDetails(17001, ContractSamples::IBMBond());
	client_socket->reqContractDetails(17002, ContractSamples::IBKRStk());

    std::this_thread::sleep_for(std::chrono::seconds(2));

	//! [reqmarketrule]
	client_socket->reqMarketRule(26);
	client_socket->reqMarketRule(635);
	client_socket->reqMarketRule(1388);
	//! [reqmarketrule]

	client_state = ST_MARKETRULE_ACK;
}

void InteractiveBrokers::continuousFuturesOperations()
{
	client_socket->reqContractDetails(18001, ContractSamples::ContFut());

	//! [reqhistoricaldatacontfut]
	std::time_t rawtime;
    std::tm* timeinfo;
    char queryTime [80];

	std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);
	std::strftime(queryTime, 80, "%Y%m%d %H:%M:%S", timeinfo);

	client_socket->reqHistoricalData(18002, ContractSamples::ContFut(), queryTime, "1 Y", "1 month", "TRADES", 0, 1, false, TagValueListSPtr());

    std::this_thread::sleep_for(std::chrono::seconds(10));

	client_socket->cancelHistoricalData(18002);
	//! [reqhistoricaldatacontfut]

	client_state = ST_CONTFUT_ACK;
}

void InteractiveBrokers::reqHistoricalTicks() 
{
	//! [reqhistoricalticks]
    client_socket->reqHistoricalTicks(19001, ContractSamples::IBMUSStockAtSmart(), "20170621 09:38:33", "", 10, "BID_ASK", 1, true, TagValueListSPtr());
    client_socket->reqHistoricalTicks(19002, ContractSamples::IBMUSStockAtSmart(), "20170621 09:38:33", "", 10, "MIDPOINT", 1, true, TagValueListSPtr());
    client_socket->reqHistoricalTicks(19003, ContractSamples::IBMUSStockAtSmart(), "20170621 09:38:33", "", 10, "TRADES", 1, true, TagValueListSPtr());
    //! [reqhistoricalticks]
    client_state = ST_REQHISTORICALTICKS_ACK;
}

void InteractiveBrokers::reqTickByTickData() 
{
    /*** Requesting tick-by-tick data (only refresh) ***/
    
    client_socket->reqTickByTickData(20001, ContractSamples::EuropeanStock(), "Last", 0, false);
    client_socket->reqTickByTickData(20002, ContractSamples::EuropeanStock(), "AllLast", 0, false);
    client_socket->reqTickByTickData(20003, ContractSamples::EuropeanStock(), "BidAsk", 0, true);
    client_socket->reqTickByTickData(20004, ContractSamples::EurGbpFx(), "MidPoint", 0, false);

    std::this_thread::sleep_for(std::chrono::seconds(10));

	//! [canceltickbytick]
    client_socket->cancelTickByTickData(20001);
    client_socket->cancelTickByTickData(20002);
    client_socket->cancelTickByTickData(20003);
    client_socket->cancelTickByTickData(20004);
    //! [canceltickbytick]
	
    /*** Requesting tick-by-tick data (historical + refresh) ***/
    //! [reqtickbytick]
    client_socket->reqTickByTickData(20005, ContractSamples::EuropeanStock(), "Last", 10, false);
    client_socket->reqTickByTickData(20006, ContractSamples::EuropeanStock(), "AllLast", 10, false);
    client_socket->reqTickByTickData(20007, ContractSamples::EuropeanStock(), "BidAsk", 10, false);
    client_socket->reqTickByTickData(20008, ContractSamples::EurGbpFx(), "MidPoint", 10, true);
	//! [reqtickbytick]
	
    std::this_thread::sleep_for(std::chrono::seconds(10));

    client_socket->cancelTickByTickData(20005);
    client_socket->cancelTickByTickData(20006);
    client_socket->cancelTickByTickData(20007);
    client_socket->cancelTickByTickData(20008);

    client_state = ST_REQTICKBYTICKDATA_ACK;
}

void InteractiveBrokers::whatIfSamples()
{
    /*** Placing waht-if order ***/
    //! [whatiforder]
    client_socket->placeOrder(order_id++, ContractSamples::USStockAtSmart(), OrderSamples::WhatIfLimitOrder("BUY", 200, 120));
    //! [whatiforder]

    client_state = ST_WHATIFSAMPLES_ACK;
}

void InteractiveBrokers::currentTime( long time)
{
    if ( client_state == ST_PING_ACK) {
        time_t t = ( time_t)time;
        struct tm * timeinfo = localtime ( &t);
        printf( "The current date/time is: %s", asctime( timeinfo));

        time_t now = ::time(NULL);
        sleep_deadline = now + SLEEP_BETWEEN_PINGS;

        client_state = ST_PING_ACK;
    }
}

//! [error]
void InteractiveBrokers::error(int id, int errorCode, const std::string& errorString)
{
    printf( "Error. Id: %d, Code: %d, Msg: %s\n", id, errorCode, errorString.c_str());
}
//! [error]

//! [tickprice]
void InteractiveBrokers::tickPrice( TickerId tickerId, TickType field, double price, const TickAttrib& attribs) {
    printf( "Tick Price. Ticker Id: %ld, Field: %d, Price: %g, CanAutoExecute: %d, PastLimit: %d, PreOpen: %d\n", tickerId, (int)field, price, attribs.canAutoExecute, attribs.pastLimit, attribs.preOpen);
}
//! [tickprice]

//! [ticksize]
void InteractiveBrokers::tickSize( TickerId tickerId, TickType field, int size) {
    printf( "Tick Size. Ticker Id: %ld, Field: %d, Size: %d\n", tickerId, (int)field, size);
}
//! [ticksize]

//! [tickoptioncomputation]
void InteractiveBrokers::tickOptionComputation( TickerId tickerId, TickType tickType, double impliedVol, double delta,
                                        double optPrice, double pvDividend,
                                        double gamma, double vega, double theta, double undPrice) {
    printf( "TickOptionComputation. Ticker Id: %ld, Type: %d, ImpliedVolatility: %g, Delta: %g, OptionPrice: %g, pvDividend: %g, Gamma: %g, Vega: %g, Theta: %g, Underlying Price: %g\n", tickerId, (int)tickType, impliedVol, delta, optPrice, pvDividend, gamma, vega, theta, undPrice);
}
//! [tickoptioncomputation]

//! [tickgeneric]
void InteractiveBrokers::tickGeneric(TickerId tickerId, TickType tickType, double value) {
    printf( "Tick Generic. Ticker Id: %ld, Type: %d, Value: %g\n", tickerId, (int)tickType, value);
}
//! [tickgeneric]

//! [tickstring]
void InteractiveBrokers::tickString(TickerId tickerId, TickType tickType, const std::string& value) {
    printf( "Tick String. Ticker Id: %ld, Type: %d, Value: %s\n", tickerId, (int)tickType, value.c_str());
}
//! [tickstring]

void InteractiveBrokers::tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const std::string& formattedBasisPoints,
                            double totalDividends, int holdDays, const std::string& futureLastTradeDate, double dividendImpact, double dividendsToLastTradeDate) {
    printf( "TickEFP. %ld, Type: %d, BasisPoints: %g, FormattedBasisPoints: %s, Total Dividends: %g, HoldDays: %d, Future Last Trade Date: %s, Dividend Impact: %g, Dividends To Last Trade Date: %g\n", tickerId, (int)tickType, basisPoints, formattedBasisPoints.c_str(), totalDividends, holdDays, futureLastTradeDate.c_str(), dividendImpact, dividendsToLastTradeDate);
}

//! [orderstatus]
void InteractiveBrokers::orderStatus(OrderId orderId, const std::string& status, double filled,
        double remaining, double avgFillPrice, int permId, int parentId,
        double lastFillPrice, int clientId, const std::string& whyHeld, double mktCapPrice){
    printf("OrderStatus. Id: %ld, Status: %s, Filled: %g, Remaining: %g, AvgFillPrice: %g, PermId: %d, LastFillPrice: %g, ClientId: %d, WhyHeld: %s, MktCapPrice: %g\n", orderId, status.c_str(), filled, remaining, avgFillPrice, permId, lastFillPrice, clientId, whyHeld.c_str(), mktCapPrice);
}
//! [orderstatus]

//! [openorder]
void InteractiveBrokers::openOrder( OrderId orderId, const Contract& contract, const Order& order, const OrderState& orderState) {
    printf( "OpenOrder. PermId: %d, ClientId: %ld, OrderId: %ld, Account: %s, Symbol: %s, SecType: %s, Exchange: %s:, Action: %s, OrderType:%s, TotalQty: %g, CashQty: %g, "
    "LmtPrice: %g, AuxPrice: %g, Status: %s\n", 
        order.permId, order.clientId, orderId, order.account.c_str(), contract.symbol.c_str(), contract.secType.c_str(), contract.exchange.c_str(), 
        order.action.c_str(), order.orderType.c_str(), order.totalQuantity, order.cashQty == UNSET_DOUBLE ? 0 : order.cashQty, order.lmtPrice, order.auxPrice, orderState.status.c_str());
}
//! [openorder]

//! [openorderend]
void InteractiveBrokers::openOrderEnd() {
    printf( "OpenOrderEnd\n");
}
//! [openorderend]

void InteractiveBrokers::winError( const std::string& str, int lastError) {}
void InteractiveBrokers::connectionClosed() {
    printf( "Connection Closed\n");
}

//! [updateaccountvalue]
void InteractiveBrokers::updateAccountValue(const std::string& key, const std::string& val,
                                    const std::string& currency, const std::string& accountName) {
    printf("UpdateAccountValue. Key: %s, Value: %s, Currency: %s, Account Name: %s\n", key.c_str(), val.c_str(), currency.c_str(), accountName.c_str());
}
//! [updateaccountvalue]

//! [updateportfolio]
void InteractiveBrokers::updatePortfolio(const Contract& contract, double position,
                                    double marketPrice, double marketValue, double averageCost,
                                    double unrealizedPNL, double realizedPNL, const std::string& accountName){
    printf("UpdatePortfolio. %s, %s @ %s: Position: %g, MarketPrice: %g, MarketValue: %g, AverageCost: %g, UnrealizedPNL: %g, RealizedPNL: %g, AccountName: %s\n", (contract.symbol).c_str(), (contract.secType).c_str(), (contract.primaryExchange).c_str(), position, marketPrice, marketValue, averageCost, unrealizedPNL, realizedPNL, accountName.c_str());
}
//! [updateportfolio]

//! [updateaccounttime]
void InteractiveBrokers::updateAccountTime(const std::string& timeStamp) {
    printf( "UpdateAccountTime. Time: %s\n", timeStamp.c_str());
}
//! [updateaccounttime]

//! [accountdownloadend]
void InteractiveBrokers::accountDownloadEnd(const std::string& accountName) {
    printf( "Account download finished: %s\n", accountName.c_str());
}
//! [accountdownloadend]

//! [contractdetails]
void InteractiveBrokers::contractDetails( int reqId, const ContractDetails& contractDetails) {
    printf( "ContractDetails begin. ReqId: %d\n", reqId);
    printContractMsg(contractDetails.contract);
    printContractDetailsMsg(contractDetails);
    printf( "ContractDetails end. ReqId: %d\n", reqId);
}
//! [contractdetails]

//! [bondcontractdetails]
void InteractiveBrokers::bondContractDetails( int reqId, const ContractDetails& contractDetails) {
    printf( "BondContractDetails begin. ReqId: %d\n", reqId);
    printBondContractDetailsMsg(contractDetails);
    printf( "BondContractDetails end. ReqId: %d\n", reqId);
}
//! [bondcontractdetails]

void InteractiveBrokers::printContractMsg(const Contract& contract) {
    printf("\tConId: %ld\n", contract.conId);
    printf("\tSymbol: %s\n", contract.symbol.c_str());
    printf("\tSecType: %s\n", contract.secType.c_str());
    printf("\tLastTradeDateOrContractMonth: %s\n", contract.lastTradeDateOrContractMonth.c_str());
    printf("\tStrike: %g\n", contract.strike);
    printf("\tRight: %s\n", contract.right.c_str());
    printf("\tMultiplier: %s\n", contract.multiplier.c_str());
    printf("\tExchange: %s\n", contract.exchange.c_str());
    printf("\tPrimaryExchange: %s\n", contract.primaryExchange.c_str());
    printf("\tCurrency: %s\n", contract.currency.c_str());
    printf("\tLocalSymbol: %s\n", contract.localSymbol.c_str());
    printf("\tTradingClass: %s\n", contract.tradingClass.c_str());
}

void InteractiveBrokers::printContractDetailsMsg(const ContractDetails& contractDetails) {
    printf("\tMarketName: %s\n", contractDetails.marketName.c_str());
    printf("\tMinTick: %g\n", contractDetails.minTick);
    printf("\tPriceMagnifier: %ld\n", contractDetails.priceMagnifier);
    printf("\tOrderTypes: %s\n", contractDetails.orderTypes.c_str());
    printf("\tValidExchanges: %s\n", contractDetails.validExchanges.c_str());
    printf("\tUnderConId: %d\n", contractDetails.underConId);
    printf("\tLongName: %s\n", contractDetails.longName.c_str());
    printf("\tContractMonth: %s\n", contractDetails.contractMonth.c_str());
    printf("\tIndystry: %s\n", contractDetails.industry.c_str());
    printf("\tCategory: %s\n", contractDetails.category.c_str());
    printf("\tSubCategory: %s\n", contractDetails.subcategory.c_str());
    printf("\tTimeZoneId: %s\n", contractDetails.timeZoneId.c_str());
    printf("\tTradingHours: %s\n", contractDetails.tradingHours.c_str());
    printf("\tLiquidHours: %s\n", contractDetails.liquidHours.c_str());
    printf("\tEvRule: %s\n", contractDetails.evRule.c_str());
    printf("\tEvMultiplier: %g\n", contractDetails.evMultiplier);
    printf("\tMdSizeMultiplier: %d\n", contractDetails.mdSizeMultiplier);
    printf("\tAggGroup: %d\n", contractDetails.aggGroup);
    printf("\tUnderSymbol: %s\n", contractDetails.underSymbol.c_str());
    printf("\tUnderSecType: %s\n", contractDetails.underSecType.c_str());
    printf("\tMarketRuleIds: %s\n", contractDetails.marketRuleIds.c_str());
    printf("\tRealExpirationDate: %s\n", contractDetails.realExpirationDate.c_str());
    printf("\tLastTradeTime: %s\n", contractDetails.lastTradeTime.c_str());
    printContractDetailsSecIdList(contractDetails.secIdList);
}

void InteractiveBrokers::printContractDetailsSecIdList(const TagValueListSPtr &secIdList) {
    const int secIdListCount = secIdList.get() ? secIdList->size() : 0;
    if (secIdListCount > 0) {
        printf("\tSecIdList: {");
        for (int i = 0; i < secIdListCount; ++i) {
            const TagValue* tagValue = ((*secIdList)[i]).get();
            printf("%s=%s;",tagValue->tag.c_str(), tagValue->value.c_str());
        }
        printf("}\n");
    }
}

void InteractiveBrokers::printBondContractDetailsMsg(const ContractDetails& contractDetails) {
    printf("\tSymbol: %s\n", contractDetails.contract.symbol.c_str());
    printf("\tSecType: %s\n", contractDetails.contract.secType.c_str());
    printf("\tCusip: %s\n", contractDetails.cusip.c_str());
    printf("\tCoupon: %g\n", contractDetails.coupon);
    printf("\tMaturity: %s\n", contractDetails.maturity.c_str());
    printf("\tIssueDate: %s\n", contractDetails.issueDate.c_str());
    printf("\tRatings: %s\n", contractDetails.ratings.c_str());
    printf("\tBondType: %s\n", contractDetails.bondType.c_str());
    printf("\tCouponType: %s\n", contractDetails.couponType.c_str());
    printf("\tConvertible: %s\n", contractDetails.convertible ? "yes" : "no");
    printf("\tCallable: %s\n", contractDetails.callable ? "yes" : "no");
    printf("\tPutable: %s\n", contractDetails.putable ? "yes" : "no");
    printf("\tDescAppend: %s\n", contractDetails.descAppend.c_str());
    printf("\tExchange: %s\n", contractDetails.contract.exchange.c_str());
    printf("\tCurrency: %s\n", contractDetails.contract.currency.c_str());
    printf("\tMarketName: %s\n", contractDetails.marketName.c_str());
    printf("\tTradingClass: %s\n", contractDetails.contract.tradingClass.c_str());
    printf("\tConId: %ld\n", contractDetails.contract.conId);
    printf("\tMinTick: %g\n", contractDetails.minTick);
    printf("\tMdSizeMultiplier: %d\n", contractDetails.mdSizeMultiplier);
    printf("\tOrderTypes: %s\n", contractDetails.orderTypes.c_str());
    printf("\tValidExchanges: %s\n", contractDetails.validExchanges.c_str());
    printf("\tNextOptionDate: %s\n", contractDetails.nextOptionDate.c_str());
    printf("\tNextOptionType: %s\n", contractDetails.nextOptionType.c_str());
    printf("\tNextOptionPartial: %s\n", contractDetails.nextOptionPartial ? "yes" : "no");
    printf("\tNotes: %s\n", contractDetails.notes.c_str());
    printf("\tLong Name: %s\n", contractDetails.longName.c_str());
    printf("\tEvRule: %s\n", contractDetails.evRule.c_str());
    printf("\tEvMultiplier: %g\n", contractDetails.evMultiplier);
    printf("\tAggGroup: %d\n", contractDetails.aggGroup);
    printf("\tMarketRuleIds: %s\n", contractDetails.marketRuleIds.c_str());
    printf("\tTimeZoneId: %s\n", contractDetails.timeZoneId.c_str());
    printf("\tLastTradeTime: %s\n", contractDetails.lastTradeTime.c_str());
    printContractDetailsSecIdList(contractDetails.secIdList);
}

//! [contractdetailsend]
void InteractiveBrokers::contractDetailsEnd( int reqId) {
    printf( "ContractDetailsEnd. %d\n", reqId);
}
//! [contractdetailsend]

//! [execdetails]
void InteractiveBrokers::execDetails( int reqId, const Contract& contract, const Execution& execution) {
    printf( "ExecDetails. ReqId: %d - %s, %s, %s - %s, %ld, %g, %d\n", reqId, contract.symbol.c_str(), contract.secType.c_str(), contract.currency.c_str(), execution.execId.c_str(), execution.orderId, execution.shares, execution.lastLiquidity);
}
//! [execdetails]

//! [execdetailsend]
void InteractiveBrokers::execDetailsEnd( int reqId) {
    printf( "ExecDetailsEnd. %d\n", reqId);
}
//! [execdetailsend]

//! [updatemktdepth]
void InteractiveBrokers::updateMktDepth(TickerId id, int position, int operation, int side,
                                double price, int size) {
    printf( "UpdateMarketDepth. %ld - Position: %d, Operation: %d, Side: %d, Price: %g, Size: %d\n", id, position, operation, side, price, size);
}
//! [updatemktdepth]

//! [updatemktdepthl2]
void InteractiveBrokers::updateMktDepthL2(TickerId id, int position, const std::string& marketMaker, int operation,
                                    int side, double price, int size, bool isSmartDepth) {
    printf( "UpdateMarketDepthL2. %ld - Position: %d, Operation: %d, Side: %d, Price: %g, Size: %d, isSmartDepth: %d\n", id, position, operation, side, price, size, isSmartDepth);
}
//! [updatemktdepthl2]

//! [updatenewsbulletin]
void InteractiveBrokers::updateNewsBulletin(int msgId, int msgType, const std::string& newsMessage, const std::string& originExch) {
    printf( "News Bulletins. %d - Type: %d, Message: %s, Exchange of Origin: %s\n", msgId, msgType, newsMessage.c_str(), originExch.c_str());
}
//! [updatenewsbulletin]

//! [managedaccounts]
void InteractiveBrokers::managedAccounts( const std::string& accountsList) {
    printf( "Account List: %s\n", accountsList.c_str());
}
//! [managedaccounts]

//! [receivefa]
void InteractiveBrokers::receiveFA(faDataType pFaDataType, const std::string& cxml) {
    std::cout << "Receiving FA: " << (int)pFaDataType << std::endl << cxml << std::endl;
}
//! [receivefa]

//! [historicaldata]
void InteractiveBrokers::historicalData(TickerId reqId, const Bar& bar) {
    printf( "HistoricalData. ReqId: %ld - Date: %s, Open: %g, High: %g, Low: %g, Close: %g, Volume: %lld, Count: %d, WAP: %g\n", reqId, bar.time.c_str(), bar.open, bar.high, bar.low, bar.close, bar.volume, bar.count, bar.wap);
}
//! [historicaldata]

//! [historicaldataend]
void InteractiveBrokers::historicalDataEnd(int reqId, const std::string& startDateStr, const std::string& endDateStr) {
    std::cout << "HistoricalDataEnd. ReqId: " << reqId << " - Start Date: " << startDateStr << ", End Date: " << endDateStr << std::endl;	
}
//! [historicaldataend]

//! [scannerparameters]
void InteractiveBrokers::scannerParameters(const std::string& xml) {
    printf( "ScannerParameters. %s\n", xml.c_str());
}
//! [scannerparameters]

//! [scannerdata]
void InteractiveBrokers::scannerData(int reqId, int rank, const ContractDetails& contractDetails,
                                const std::string& distance, const std::string& benchmark, const std::string& projection,
                                const std::string& legsStr) {
    printf( "ScannerData. %d - Rank: %d, Symbol: %s, SecType: %s, Currency: %s, Distance: %s, Benchmark: %s, Projection: %s, Legs String: %s\n", reqId, rank, contractDetails.contract.symbol.c_str(), contractDetails.contract.secType.c_str(), contractDetails.contract.currency.c_str(), distance.c_str(), benchmark.c_str(), projection.c_str(), legsStr.c_str());
}
//! [scannerdata]

//! [scannerdataend]
void InteractiveBrokers::scannerDataEnd(int reqId) {
    printf( "ScannerDataEnd. %d\n", reqId);
}
//! [scannerdataend]

//! [realtimebar]
void InteractiveBrokers::realtimeBar(TickerId reqId, long time, double open, double high, double low, double close,
                                long volume, double wap, int count) {
    printf( "RealTimeBars. %ld - Time: %ld, Open: %g, High: %g, Low: %g, Close: %g, Volume: %ld, Count: %d, WAP: %g\n", reqId, time, open, high, low, close, volume, count, wap);
}
//! [realtimebar]

//! [fundamentaldata]
void InteractiveBrokers::fundamentalData(TickerId reqId, const std::string& data) {
    printf( "FundamentalData. ReqId: %ld, %s\n", reqId, data.c_str());
}
//! [fundamentaldata]

void InteractiveBrokers::deltaNeutralValidation(int reqId, const DeltaNeutralContract& deltaNeutralContract) {
    printf( "DeltaNeutralValidation. %d, ConId: %ld, Delta: %g, Price: %g\n", reqId, deltaNeutralContract.conId, deltaNeutralContract.delta, deltaNeutralContract.price);
}

//! [ticksnapshotend]
void InteractiveBrokers::tickSnapshotEnd(int reqId) {
    printf( "TickSnapshotEnd: %d\n", reqId);
}
//! [ticksnapshotend]

//! [marketdatatype]
void InteractiveBrokers::marketDataType(TickerId reqId, int marketDataType) {
    printf( "MarketDataType. ReqId: %ld, Type: %d\n", reqId, marketDataType);
}
//! [marketdatatype]

//! [commissionreport]
void InteractiveBrokers::commissionReport( const CommissionReport& commissionReport) {
    printf( "CommissionReport. %s - %g %s RPNL %g\n", commissionReport.execId.c_str(), commissionReport.commission, commissionReport.currency.c_str(), commissionReport.realizedPNL);
}
//! [commissionreport]

//! [position]
void InteractiveBrokers::position( const std::string& account, const Contract& contract, double position, double avgCost) {
    printf( "Position. %s - Symbol: %s, SecType: %s, Currency: %s, Position: %g, Avg Cost: %g\n", account.c_str(), contract.symbol.c_str(), contract.secType.c_str(), contract.currency.c_str(), position, avgCost);
}
//! [position]

//! [positionend]
void InteractiveBrokers::positionEnd() {
    printf( "PositionEnd\n");
}
//! [positionend]

//! [accountsummary]
void InteractiveBrokers::accountSummary( int reqId, const std::string& account, const std::string& tag, const std::string& value, const std::string& currency) {
    printf( "Acct Summary. ReqId: %d, Account: %s, Tag: %s, Value: %s, Currency: %s\n", reqId, account.c_str(), tag.c_str(), value.c_str(), currency.c_str());
}
//! [accountsummary]

//! [accountsummaryend]
void InteractiveBrokers::accountSummaryEnd( int reqId) {
    printf( "AccountSummaryEnd. Req Id: %d\n", reqId);
}
//! [accountsummaryend]

void InteractiveBrokers::verifyMessageAPI( const std::string& apiData) {
    printf("verifyMessageAPI: %s\b", apiData.c_str());
}

void InteractiveBrokers::verifyCompleted( bool isSuccessful, const std::string& errorText) {
    printf("verifyCompleted. IsSuccessfule: %d - Error: %s\n", isSuccessful, errorText.c_str());
}

void InteractiveBrokers::verifyAndAuthMessageAPI( const std::string& apiDatai, const std::string& xyzChallenge) {
    printf("verifyAndAuthMessageAPI: %s %s\n", apiDatai.c_str(), xyzChallenge.c_str());
}

void InteractiveBrokers::verifyAndAuthCompleted( bool isSuccessful, const std::string& errorText) {
    printf("verifyAndAuthCompleted. IsSuccessful: %d - Error: %s\n", isSuccessful, errorText.c_str());
    if (isSuccessful)
        client_socket->startApi();
}

//! [displaygrouplist]
void InteractiveBrokers::displayGroupList( int reqId, const std::string& groups) {
    printf("Display Group List. ReqId: %d, Groups: %s\n", reqId, groups.c_str());
}
//! [displaygrouplist]

//! [displaygroupupdated]
void InteractiveBrokers::displayGroupUpdated( int reqId, const std::string& contractInfo) {
    std::cout << "Display Group Updated. ReqId: " << reqId << ", Contract Info: " << contractInfo << std::endl;
}
//! [displaygroupupdated]

//! [positionmulti]
void InteractiveBrokers::positionMulti( int reqId, const std::string& account,const std::string& modelCode, const Contract& contract, double pos, double avgCost) {
    printf("Position Multi. Request: %d, Account: %s, ModelCode: %s, Symbol: %s, SecType: %s, Currency: %s, Position: %g, Avg Cost: %g\n", reqId, account.c_str(), modelCode.c_str(), contract.symbol.c_str(), contract.secType.c_str(), contract.currency.c_str(), pos, avgCost);
}
//! [positionmulti]

//! [positionmultiend]
void InteractiveBrokers::positionMultiEnd( int reqId) {
    printf("Position Multi End. Request: %d\n", reqId);
}
//! [positionmultiend]

//! [accountupdatemulti]
void InteractiveBrokers::accountUpdateMulti( int reqId, const std::string& account, const std::string& modelCode, const std::string& key, const std::string& value, const std::string& currency) {
    printf("AccountUpdate Multi. Request: %d, Account: %s, ModelCode: %s, Key, %s, Value: %s, Currency: %s\n", reqId, account.c_str(), modelCode.c_str(), key.c_str(), value.c_str(), currency.c_str());
}
//! [accountupdatemulti]

//! [accountupdatemultiend]
void InteractiveBrokers::accountUpdateMultiEnd( int reqId) {
    printf("Account Update Multi End. Request: %d\n", reqId);
}
//! [accountupdatemultiend]

//! [securityDefinitionOptionParameter]
void InteractiveBrokers::securityDefinitionOptionalParameter(int reqId, const std::string& exchange, int underlyingConId, const std::string& tradingClass,
                                                        const std::string& multiplier, const std::set<std::string>& expirations, const std::set<double>& strikes) {
    printf("Security Definition Optional Parameter. Request: %d, Trading Class: %s, Multiplier: %s\n", reqId, tradingClass.c_str(), multiplier.c_str());
}
//! [securityDefinitionOptionParameter]

//! [securityDefinitionOptionParameterEnd]
void InteractiveBrokers::securityDefinitionOptionalParameterEnd(int reqId) {
    printf("Security Definition Optional Parameter End. Request: %d\n", reqId);
}
//! [securityDefinitionOptionParameterEnd]

//! [softDollarTiers]
void InteractiveBrokers::softDollarTiers(int reqId, const std::vector<SoftDollarTier> &tiers) {
    printf("Soft dollar tiers (%lu):", tiers.size());

    for (unsigned int i = 0; i < tiers.size(); i++) {
        printf("%s\n", tiers[i].displayName().c_str());
    }
}
//! [softDollarTiers]

//! [familyCodes]
void InteractiveBrokers::familyCodes(const std::vector<FamilyCode> &familyCodes) {
    printf("Family codes (%lu):\n", familyCodes.size());

    for (unsigned int i = 0; i < familyCodes.size(); i++) {
        printf("Family code [%d] - accountID: %s familyCodeStr: %s\n", i, familyCodes[i].accountID.c_str(), familyCodes[i].familyCodeStr.c_str());
    }
}
//! [familyCodes]

//! [symbolSamples]
void InteractiveBrokers::symbolSamples(int reqId, const std::vector<ContractDescription> &contractDescriptions) {
    printf("Symbol Samples (total=%lu) reqId: %d\n", contractDescriptions.size(), reqId);

    for (unsigned int i = 0; i < contractDescriptions.size(); i++) {
        Contract contract = contractDescriptions[i].contract;
        std::vector<std::string> derivativeSecTypes = contractDescriptions[i].derivativeSecTypes;
        printf("Contract (%u): %ld %s %s %s %s, ", i, contract.conId, contract.symbol.c_str(), contract.secType.c_str(), contract.primaryExchange.c_str(), contract.currency.c_str());
        printf("Derivative Sec-types (%lu):", derivativeSecTypes.size());
        for (unsigned int j = 0; j < derivativeSecTypes.size(); j++) {
            printf(" %s", derivativeSecTypes[j].c_str());
        }
        printf("\n");
    }
}
//! [symbolSamples]

//! [mktDepthExchanges]
void InteractiveBrokers::mktDepthExchanges(const std::vector<DepthMktDataDescription> &depthMktDataDescriptions) {
    printf("Mkt Depth Exchanges (%lu):\n", depthMktDataDescriptions.size());

    for (unsigned int i = 0; i < depthMktDataDescriptions.size(); i++) {
        printf("Depth Mkt Data Description [%d] - exchange: %s secType: %s listingExch: %s serviceDataType: %s aggGroup: %s\n", i, 
            depthMktDataDescriptions[i].exchange.c_str(), 
            depthMktDataDescriptions[i].secType.c_str(), 
            depthMktDataDescriptions[i].listingExch.c_str(), 
            depthMktDataDescriptions[i].serviceDataType.c_str(), 
            depthMktDataDescriptions[i].aggGroup != INT_MAX ? std::to_string(depthMktDataDescriptions[i].aggGroup).c_str() : "");
    }
}
//! [mktDepthExchanges]

//! [tickNews]
void InteractiveBrokers::tickNews(int tickerId, time_t timeStamp, const std::string& providerCode, const std::string& articleId, const std::string& headline, const std::string& extraData) {
    printf("News Tick. TickerId: %d, TimeStamp: %s, ProviderCode: %s, ArticleId: %s, Headline: %s, ExtraData: %s\n", tickerId, ctime(&(timeStamp /= 1000)), providerCode.c_str(), articleId.c_str(), headline.c_str(), extraData.c_str());
}
//! [tickNews]

//! [smartcomponents]]
void InteractiveBrokers::smartComponents(int reqId, const SmartComponentsMap& theMap) {
    printf("Smart components: (%lu):\n", theMap.size());

    for (SmartComponentsMap::const_iterator i = theMap.begin(); i != theMap.end(); i++) {
        printf(" bit number: %d exchange: %s exchange letter: %c\n", i->first, std::get<0>(i->second).c_str(), std::get<1>(i->second));
    }
}
//! [smartcomponents]

//! [tickReqParams]
void InteractiveBrokers::tickReqParams(int tickerId, double minTick, const std::string& bboExchange, int snapshotPermissions) {
    printf("tickerId: %d, minTick: %g, bboExchange: %s, snapshotPermissions: %u", tickerId, minTick, bboExchange.c_str(), snapshotPermissions);

    m_bboExchange = bboExchange;
}
//! [tickReqParams]

//! [newsProviders]
void InteractiveBrokers::newsProviders(const std::vector<NewsProvider> &newsProviders) {
    printf("News providers (%lu):\n", newsProviders.size());

    for (unsigned int i = 0; i < newsProviders.size(); i++) {
        printf("News provider [%d] - providerCode: %s providerName: %s\n", i, newsProviders[i].providerCode.c_str(), newsProviders[i].providerName.c_str());
    }
}
//! [newsProviders]

//! [newsArticle]
void InteractiveBrokers::newsArticle(int requestId, int articleType, const std::string& articleText) {
    printf("News Article. Request Id: %d, Article Type: %d\n", requestId, articleType);
    if (articleType == 0) {
        printf("News Article Text (text or html): %s\n", articleText.c_str());
    } else if (articleType == 1) {
        std::string path;
        #if defined(IB_WIN32)
            TCHAR s[200];
            GetCurrentDirectory(200, s);
            path = s + std::string("\\MST$06f53098.pdf");
        #elif defined(IB_POSIX)
            char s[1024];
            if (getcwd(s, sizeof(s)) == NULL) {
                printf("getcwd() error\n");
                return;
            }
            path = s + std::string("/MST$06f53098.pdf");
        #endif
        std::vector<std::uint8_t> bytes = Utils::base64_decode(articleText);
        std::ofstream outfile(path, std::ios::out | std::ios::binary); 
        outfile.write((const char*)bytes.data(), bytes.size());
        printf("Binary/pdf article was saved to: %s\n", path.c_str());
    }
}
//! [newsArticle]

//! [historicalNews]
void InteractiveBrokers::historicalNews(int requestId, const std::string& time, const std::string& providerCode, const std::string& articleId, const std::string& headline) {
    printf("Historical News. RequestId: %d, Time: %s, ProviderCode: %s, ArticleId: %s, Headline: %s\n", requestId, time.c_str(), providerCode.c_str(), articleId.c_str(), headline.c_str());
}
//! [historicalNews]

//! [historicalNewsEnd]
void InteractiveBrokers::historicalNewsEnd(int requestId, bool hasMore) {
    printf("Historical News End. RequestId: %d, HasMore: %s\n", requestId, (hasMore ? "true" : " false"));
}
//! [historicalNewsEnd]

//! [headTimestamp]
void InteractiveBrokers::headTimestamp(int reqId, const std::string& headTimestamp) {
    printf( "Head time stamp. ReqId: %d - Head time stamp: %s,\n", reqId, headTimestamp.c_str());

}
//! [headTimestamp]

//! [histogramData]
void InteractiveBrokers::histogramData(int reqId, const HistogramDataVector& data) {
    printf("Histogram. ReqId: %d, data length: %lu\n", reqId, data.size());

    for (auto item : data) {
        printf("\t price: %f, size: %lld\n", item.price, item.size);
    }
}
//! [histogramData]

//! [historicalDataUpdate]
void InteractiveBrokers::historicalDataUpdate(TickerId reqId, const Bar& bar) {
    printf( "HistoricalDataUpdate. ReqId: %ld - Date: %s, Open: %g, High: %g, Low: %g, Close: %g, Volume: %lld, Count: %d, WAP: %g\n", reqId, bar.time.c_str(), bar.open, bar.high, bar.low, bar.close, bar.volume, bar.count, bar.wap);
}
//! [historicalDataUpdate]

//! [rerouteMktDataReq]
void InteractiveBrokers::rerouteMktDataReq(int reqId, int conid, const std::string& exchange) {
    printf( "Re-route market data request. ReqId: %d, ConId: %d, Exchange: %s\n", reqId, conid, exchange.c_str());
}
//! [rerouteMktDataReq]

//! [rerouteMktDepthReq]
void InteractiveBrokers::rerouteMktDepthReq(int reqId, int conid, const std::string& exchange) {
    printf( "Re-route market depth request. ReqId: %d, ConId: %d, Exchange: %s\n", reqId, conid, exchange.c_str());
}
//! [rerouteMktDepthReq]

//! [marketRule]
void InteractiveBrokers::marketRule(int marketRuleId, const std::vector<PriceIncrement> &priceIncrements) {
    printf("Market Rule Id: %d\n", marketRuleId);
    for (unsigned int i = 0; i < priceIncrements.size(); i++) {
        printf("Low Edge: %g, Increment: %g\n", priceIncrements[i].lowEdge, priceIncrements[i].increment);
    }
}
//! [marketRule]

//! [pnl]
void InteractiveBrokers::pnl(int reqId, double dailyPnL, double unrealizedPnL, double realizedPnL) {
    printf("PnL. ReqId: %d, daily PnL: %g, unrealized PnL: %g, realized PnL: %g\n", reqId, dailyPnL, unrealizedPnL, realizedPnL);
}
//! [pnl]

//! [pnlsingle]
void InteractiveBrokers::pnlSingle(int reqId, int pos, double dailyPnL, double unrealizedPnL, double realizedPnL, double value) {
    printf("PnL Single. ReqId: %d, pos: %d, daily PnL: %g, unrealized PnL: %g, realized PnL: %g, value: %g\n", reqId, pos, dailyPnL, unrealizedPnL, realizedPnL, value);
}
//! [pnlsingle]

//! [historicalticks]
void InteractiveBrokers::historicalTicks(int reqId, const std::vector<HistoricalTick>& ticks, bool done) {
    for (HistoricalTick tick : ticks) {
    std::time_t t = tick.time;
        std::cout << "Historical tick. ReqId: " << reqId << ", time: " << ctime(&t) << ", price: "<< tick.price << ", size: " << tick.size << std::endl;
    }
}
//! [historicalticks]

//! [historicalticksbidask]
void InteractiveBrokers::historicalTicksBidAsk(int reqId, const std::vector<HistoricalTickBidAsk>& ticks, bool done) {
    for (HistoricalTickBidAsk tick : ticks) {
    std::time_t t = tick.time;
        std::cout << "Historical tick bid/ask. ReqId: " << reqId << ", time: " << ctime(&t) << ", price bid: "<< tick.priceBid <<
            ", price ask: "<< tick.priceAsk << ", size bid: " << tick.sizeBid << ", size ask: " << tick.sizeAsk <<
            ", bidPastLow: " << tick.tickAttribBidAsk.bidPastLow << ", askPastHigh: " << tick.tickAttribBidAsk.askPastHigh << std::endl;
    }
}
//! [historicalticksbidask]

//! [historicaltickslast]
void InteractiveBrokers::historicalTicksLast(int reqId, const std::vector<HistoricalTickLast>& ticks, bool done) {
    for (HistoricalTickLast tick : ticks) {
    std::time_t t = tick.time;
        std::cout << "Historical tick last. ReqId: " << reqId << ", time: " << ctime(&t) << ", price: "<< tick.price <<
            ", size: " << tick.size << ", exchange: " << tick.exchange << ", special conditions: " << tick.specialConditions <<
            ", unreported: " << tick.tickAttribLast.unreported << ", pastLimit: " << tick.tickAttribLast.pastLimit << std::endl;
    }
}
//! [historicaltickslast]

//! [tickbytickalllast]
void InteractiveBrokers::tickByTickAllLast(int reqId, int tickType, time_t time, double price, int size, const TickAttribLast& tickAttribLast, const std::string& exchange, const std::string& specialConditions) {
    printf("Tick-By-Tick. ReqId: %d, TickType: %s, Time: %s, Price: %g, Size: %d, PastLimit: %d, Unreported: %d, Exchange: %s, SpecialConditions:%s\n", 
        reqId, (tickType == 1 ? "Last" : "AllLast"), ctime(&time), price, size, tickAttribLast.pastLimit, tickAttribLast.unreported, exchange.c_str(), specialConditions.c_str());
}
//! [tickbytickalllast]

//! [tickbytickbidask]
void InteractiveBrokers::tickByTickBidAsk(int reqId, time_t time, double bidPrice, double askPrice, int bidSize, int askSize, const TickAttribBidAsk& tickAttribBidAsk) {
    printf("Tick-By-Tick. ReqId: %d, TickType: BidAsk, Time: %s, BidPrice: %g, AskPrice: %g, BidSize: %d, AskSize: %d, BidPastLow: %d, AskPastHigh: %d\n", 
        reqId, ctime(&time), bidPrice, askPrice, bidSize, askSize, tickAttribBidAsk.bidPastLow, tickAttribBidAsk.askPastHigh);
}
//! [tickbytickbidask]

//! [tickbytickmidpoint]
void InteractiveBrokers::tickByTickMidPoint(int reqId, time_t time, double midPoint) {
    printf("Tick-By-Tick. ReqId: %d, TickType: MidPoint, Time: %s, MidPoint: %g\n", reqId, ctime(&time), midPoint);
}
//! [tickbytickmidpoint]

//! [orderbound]
void InteractiveBrokers::orderBound(long long orderId, int apiClientId, int apiOrderId) {
    printf("Order bound. OrderId: %lld, ApiClientId: %d, ApiOrderId: %d\n", orderId, apiClientId, apiOrderId);
}
//! [orderbound]

//! [completedorder]
void InteractiveBrokers::completedOrder(const Contract& contract, const Order& order, const OrderState& orderState) {
    printf( "CompletedOrder. PermId: %d, ParentPermId: %lld, Account: %s, Symbol: %s, SecType: %s, Exchange: %s:, Action: %s, OrderType: %s, TotalQty: %g, CashQty: %g, FilledQty: %g, "
        "LmtPrice: %g, AuxPrice: %g, Status: %s, CompletedTime: %s, CompletedStatus: %s\n", 
        order.permId, order.parentPermId == UNSET_LONG ? 0 : order.parentPermId, order.account.c_str(), contract.symbol.c_str(), contract.secType.c_str(), contract.exchange.c_str(), 
        order.action.c_str(), order.orderType.c_str(), order.totalQuantity, order.cashQty == UNSET_DOUBLE ? 0 : order.cashQty, order.filledQuantity, 
        order.lmtPrice, order.auxPrice, orderState.status.c_str(), orderState.completedTime.c_str(), orderState.completedStatus.c_str());
}
//! [completedorder]

//! [completedordersend]
void InteractiveBrokers::completedOrdersEnd() {
    printf( "CompletedOrdersEnd\n");
}
//! [completedordersend]
