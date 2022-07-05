/* Copyright (C) 2019 Interactive Brokers LLC. All rights reserved. This code is
 * subject to the terms and conditions of the IB API Non-Commercial License or
 * the IB API Commercial License, as applicable. */
#include "ScannerSubscriptionSamples.hpp"

#include "StdAfx.h"
#include "tws/CommissionReport.h"
#include "tws/CommonDefs.h"
#include "tws/Contract.h"
#include "tws/EClientSocket.h"
#include "tws/EPosixClientSocketPlatform.h"
#include "tws/EReader.h"
#include "tws/EReaderOSSignal.h"
#include "tws/EWrapper.h"
#include "tws/Execution.h"
#include "tws/MarginCondition.h"
#include "tws/Order.h"
#include "tws/OrderState.h"
#include "tws/PercentChangeCondition.h"
#include "tws/PriceCondition.h"
#include "tws/ScannerSubscription.h"
#include "tws/StdAfx.h"
#include "tws/TimeCondition.h"
#include "tws/VolumeCondition.h"
#include "tws/executioncondition.h"
#include "tws/platformspecific.h"

ScannerSubscription ScannerSubscriptionSamples::HotUSStkByVolume() {
  //! [hotusvolume]
  // Hot US stocks by volume
  ScannerSubscription scanSub;
  scanSub.instrument = "STK";
  scanSub.locationCode = "STK.US.MAJOR";
  scanSub.scanCode = "HOT_BY_VOLUME";
  //! [hotusvolume]
  return scanSub;
}

ScannerSubscription ScannerSubscriptionSamples::TopPercentGainersIbis() {
  //! [toppercentgaineribis]
  // Top % gainers at IBIS
  ScannerSubscription scanSub;
  scanSub.instrument = "STOCK.EU";
  scanSub.locationCode = "STK.EU.IBIS";
  scanSub.scanCode = "TOP_PERC_GAIN";
  //! [toppercentgaineribis]
  return scanSub;
}

ScannerSubscription ScannerSubscriptionSamples::MostActiveFutSoffex() {
  //! [mostactivefutsoffex]
  // Most active futures at SOFFEX
  ScannerSubscription scanSub;
  scanSub.instrument = "FUT.EU";
  scanSub.locationCode = "FUT.EU.SOFFEX";
  scanSub.scanCode = "MOST_ACTIVE";
  //! [mostactivefutsoffex]
  return scanSub;
}

ScannerSubscription
ScannerSubscriptionSamples::HighOptVolumePCRatioUSIndexes() {
  //! [highoptvolume]
  // High option volume P/C ratio US indexes
  ScannerSubscription scanSub;
  scanSub.instrument = "IND.US";
  scanSub.locationCode = "IND.US";
  scanSub.scanCode = "HIGH_OPT_VOLUME_PUT_CALL_RATIO";
  //! [highoptvolume]
  return scanSub;
}

ScannerSubscription ScannerSubscriptionSamples::ComplexOrdersAndTrades() {
  //! [combolatesttrade]
  // Complex orders and trades scan, latest trades
  ScannerSubscription scanSub;
  scanSub.instrument = "NATCOMB";
  scanSub.locationCode = "NATCOMB.OPT.US";
  scanSub.scanCode = "COMBO_LATEST_TRADE";
  //! [combolatesttrade]
  return scanSub;
}