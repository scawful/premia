#ifndef InteractiveBrokers_hpp
#define InteractiveBrokers_hpp

#include "../core.hpp"
#include "../../include/twsapi/EWrapper.h"
#include "../../include/twsapi/EReaderOSSignal.h"
#include "../../include/twsapi/EReader.h"

class EClientSocket;

enum IBKR_State {
    ST_CONNECT,
    ST_TICKDATAOPERATION,
    ST_TICKDATAOPERATION_ACK,
    ST_TICKOPTIONCOMPUTATIONOPERATION,
    ST_TICKOPTIONCOMPUTATIONOPERATION_ACK,
    ST_DELAYEDTICKDATAOPERATION,
    ST_DELAYEDTICKDATAOPERATION_ACK,
    ST_MARKETDEPTHOPERATION,
    ST_MARKETDEPTHOPERATION_ACK,
    ST_REALTIMEBARS,
    ST_REALTIMEBARS_ACK,
    ST_MARKETDATATYPE,
    ST_MARKETDATATYPE_ACK,
    ST_HISTORICALDATAREQUESTS,
    ST_HISTORICALDATAREQUESTS_ACK,
    ST_OPTIONSOPERATIONS,
    ST_OPTIONSOPERATIONS_ACK,
    ST_CONTRACTOPERATION,
    ST_CONTRACTOPERATION_ACK,
    ST_MARKETSCANNERS,
    ST_MARKETSCANNERS_ACK,
    ST_FUNDAMENTALS,
    ST_FUNDAMENTALS_ACK,
    ST_BULLETINS,
    ST_BULLETINS_ACK,
    ST_ACCOUNTOPERATIONS,
    ST_ACCOUNTOPERATIONS_ACK,
    ST_ORDEROPERATIONS,
    ST_ORDEROPERATIONS_ACK,
    ST_OCASAMPLES,
    ST_OCASAMPLES_ACK,
    ST_CONDITIONSAMPLES,
    ST_CONDITIONSAMPLES_ACK,
    ST_BRACKETSAMPLES,
    ST_BRACKETSAMPLES_ACK,
    ST_HEDGESAMPLES,
    ST_HEDGESAMPLES_ACK,
    ST_TESTALGOSAMPLES,
    ST_TESTALGOSAMPLES_ACK,
    ST_FAORDERSAMPLES,
    ST_FAORDERSAMPLES_ACK,
    ST_FAOPERATIONS,
    ST_FAOPERATIONS_ACK,
    ST_DISPLAYGROUPS,
    ST_DISPLAYGROUPS_ACK,
    ST_MISCELANEOUS,
    ST_MISCELANEOUS_ACK,
    ST_CANCELORDER,
    ST_CANCELORDER_ACK,
    ST_FAMILYCODES,
    ST_FAMILYCODES_ACK,
    ST_SYMBOLSAMPLES,
    ST_SYMBOLSAMPLES_ACK,
    ST_REQMKTDEPTHEXCHANGES,
    ST_REQMKTDEPTHEXCHANGES_ACK,
    ST_REQNEWSTICKS,
    ST_REQNEWSTICKS_ACK,
    ST_REQSMARTCOMPONENTS,
    ST_REQSMARTCOMPONENTS_ACK,
    ST_NEWSPROVIDERS,
    ST_NEWSPROVIDERS_ACK,
    ST_REQNEWSARTICLE,
    ST_REQNEWSARTICLE_ACK,
    ST_REQHISTORICALNEWS,
    ST_REQHISTORICALNEWS_ACK,
    ST_REQHEADTIMESTAMP,
    ST_REQHEADTIMESTAMP_ACK,
    ST_REQHISTOGRAMDATA,
    ST_REQHISTOGRAMDATA_ACK,
    ST_REROUTECFD,
    ST_REROUTECFD_ACK,
    ST_MARKETRULE,
    ST_MARKETRULE_ACK,
    ST_PNL,
    ST_PNL_ACK,
    ST_PNLSINGLE,
    ST_PNLSINGLE_ACK,
    ST_CONTFUT,
    ST_CONTFUT_ACK,
    ST_PING,
    ST_PING_ACK,
    ST_REQHISTORICALTICKS,
    ST_REQHISTORICALTICKS_ACK,
    ST_REQTICKBYTICKDATA,
    ST_REQTICKBYTICKDATA_ACK,
    ST_WHATIFSAMPLES,
    ST_WHATIFSAMPLES_ACK,
    ST_IDLE
};

class InteractiveBrokers : public EWrapper
{
private:
    EClientSocket * const client_socket;
    EReaderOSSignal os_signal;
    IBKR_State client_state;
    time_t sleep_deadline;

    OrderId order_id;
    EReader *p_reader;
    bool extra_auth;
    std::string m_bboExchange;

    void reqCurrentTime();

    void pnlOperation();
    void pnlSingleOperation();
    void tickDataOperation();
    void tickOptionComputationOperation();
    void delayedTickDataOperation();
    void marketDepthOperations();
    void realTimeBars();
    void marketDataType();
    void historicalDataRequests();
    void optionsOperations();
    void accountOperations();
    void orderOperations();
    void ocaSamples();
    void conditionSamples();
    void bracketSample();
    void hedgeSample();
    void contractOperations();
    void marketScanners();
    void fundamentals();
    void bulletins();
    void testAlgoSamples();
    void financialAdvisorOrderSamples();
    void financialAdvisorOperations();
    void testDisplayGroups();
    void miscelaneous();
    void reqFamilyCodes();
    void reqMatchingSymbols();
    void reqMktDepthExchanges();
    void reqNewsTicks();
    void reqSmartComponents();
    void reqNewsProviders();
    void reqNewsArticle();
    void reqHistoricalNews();
    void reqHeadTimestamp();
    void reqHistogramData();
    void rerouteCFDOperations();
    void marketRuleOperations();
    void continuousFuturesOperations();
    void reqHistoricalTicks();
    void reqTickByTickData();
    void whatIfSamples();

public:
    InteractiveBrokers();
    ~InteractiveBrokers();

    bool connect(const char *host, int port, int clientId);
    void disconnect() const;
    bool isConnected() const;
    
    void setConnectOptions(const std::string&);
    void processMessages();

    #include "../../include/twsapi/EWrapper_prototypes.h"

    void printContractMsg(const Contract& contract);
	void printContractDetailsMsg(const ContractDetails& contractDetails);
	void printContractDetailsSecIdList(const TagValueListSPtr &secIdList);
	void printBondContractDetailsMsg(const ContractDetails& contractDetails);
};

#endif