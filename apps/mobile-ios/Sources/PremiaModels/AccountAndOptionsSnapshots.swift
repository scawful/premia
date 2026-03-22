import Foundation

public struct PremiaAccountPositionModel: Codable, Sendable, Equatable, Identifiable {
    public let id: String
    public let symbol: String
    public let name: String
    public let dayProfitLoss: PremiaMoney
    public let dayProfitLossPercent: String
    public let averagePrice: PremiaMoney
    public let marketValue: PremiaMoney
    public let quantity: String

    public init(symbol: String, name: String, dayProfitLoss: PremiaMoney, dayProfitLossPercent: String, averagePrice: PremiaMoney, marketValue: PremiaMoney, quantity: String) {
        self.id = symbol
        self.symbol = symbol
        self.name = name
        self.dayProfitLoss = dayProfitLoss
        self.dayProfitLossPercent = dayProfitLossPercent
        self.averagePrice = averagePrice
        self.marketValue = marketValue
        self.quantity = quantity
    }
}

public struct PremiaAccountDetailSnapshot: Codable, Sendable, Equatable {
    public let accountID: String
    public let cash: PremiaMoney
    public let netLiquidation: PremiaMoney
    public let availableFunds: PremiaMoney
    public let longMarketValue: PremiaMoney
    public let shortMarketValue: PremiaMoney
    public let buyingPower: PremiaMoney
    public let equity: PremiaMoney
    public let equityPercentage: String
    public let marginBalance: PremiaMoney
    public let positions: [PremiaAccountPositionModel]
    public let asOf: Date?

    public init(accountID: String, cash: PremiaMoney, netLiquidation: PremiaMoney, availableFunds: PremiaMoney, longMarketValue: PremiaMoney, shortMarketValue: PremiaMoney, buyingPower: PremiaMoney, equity: PremiaMoney, equityPercentage: String, marginBalance: PremiaMoney, positions: [PremiaAccountPositionModel], asOf: Date?) {
        self.accountID = accountID
        self.cash = cash
        self.netLiquidation = netLiquidation
        self.availableFunds = availableFunds
        self.longMarketValue = longMarketValue
        self.shortMarketValue = shortMarketValue
        self.buyingPower = buyingPower
        self.equity = equity
        self.equityPercentage = equityPercentage
        self.marginBalance = marginBalance
        self.positions = positions
        self.asOf = asOf
    }
}

public struct PremiaOptionContractPairRowModel: Codable, Sendable, Equatable, Identifiable {
    public let id: String
    public let strike: String
    public let callBid: String
    public let callAsk: String
    public let callLast: String
    public let callChange: String
    public let callDelta: String
    public let callGamma: String
    public let callTheta: String
    public let callVega: String
    public let callOpenInterest: String
    public let putBid: String
    public let putAsk: String
    public let putLast: String
    public let putChange: String
    public let putDelta: String
    public let putGamma: String
    public let putTheta: String
    public let putVega: String
    public let putOpenInterest: String
}

public struct PremiaOptionExpirationSnapshotModel: Codable, Sendable, Equatable, Identifiable {
    public let id: String
    public let label: String
    public let gammaAtExpiry: String
    public let rows: [PremiaOptionContractPairRowModel]
}

public struct PremiaOptionChainSnapshotModel: Codable, Sendable, Equatable {
    public let symbol: String
    public let description: String
    public let bid: String
    public let ask: String
    public let openPrice: String
    public let closePrice: String
    public let highPrice: String
    public let lowPrice: String
    public let totalVolume: String
    public let volatility: String
    public let gammaExposure: String
    public let expirations: [PremiaOptionExpirationSnapshotModel]
    public let asOf: Date?
}
