#ifndef TraderWorkstationApiInclude_hpp
#define TraderWorkstationApiInclude_hpp

#include "../libs/tws/EClientSocket.h"
#include "../libs/tws/EPosixClientSocketPlatform.h"
#include "../libs/tws/EWrapper.h"
#include "../libs/tws/EReaderOSSignal.h"
#include "../libs/tws/EReader.h"
#include "../libs/tws/Contract.h"
#include "../libs/tws/Order.h"
#include "../libs/tws/OrderState.h"
#include "../libs/tws/Execution.h"
#include "../libs/tws/CommissionReport.h"
#include "../libs/tws/ScannerSubscription.h"
#include "../libs/tws/executioncondition.h"
#include "../libs/tws/PriceCondition.h"
#include "../libs/tws/MarginCondition.h"
#include "../libs/tws/PercentChangeCondition.h"
#include "../libs/tws/TimeCondition.h"
#include "../libs/tws/VolumeCondition.h"
#include "../libs/tws/CommonDefs.h"
#include "../libs/tws/StdAfx.h"
#include "../libs/tws/platformspecific.h"

#ifndef TWSAPIDLL
#ifndef TWSAPIDLLEXP
#ifdef _MSC_VER
#define TWSAPIDLLEXP __declspec(dllimport)
#else
#define TWSAPIDLLEXP
#endif
#endif
#endif

#endif