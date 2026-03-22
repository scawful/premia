import Foundation

public struct PremiaHoldingSummary: Codable, Sendable, Equatable, Identifiable {
    public let id: String
    public let symbol: String
    public let name: String
    public let quantity: String
    public let marketValue: PremiaMoney
    public let dayChange: PremiaAbsolutePercentChange

    public init(id: String, symbol: String, name: String, quantity: String, marketValue: PremiaMoney, dayChange: PremiaAbsolutePercentChange) {
        self.id = id
        self.symbol = symbol
        self.name = name
        self.quantity = quantity
        self.marketValue = marketValue
        self.dayChange = dayChange
    }
}

public struct PremiaWatchlistSummaryModel: Codable, Sendable, Equatable, Identifiable {
    public let id: String
    public let name: String
    public let instrumentCount: Int

    public init(id: String, name: String, instrumentCount: Int) {
        self.id = id
        self.name = name
        self.instrumentCount = instrumentCount
    }
}

public struct PremiaPortfolioSummary: Codable, Sendable, Equatable {
    public let totalValue: PremiaMoney
    public let dayChange: PremiaAbsolutePercentChange
    public let cash: PremiaMoney
    public let buyingPower: PremiaMoney
    public let holdingsCount: Int

    public init(totalValue: PremiaMoney, dayChange: PremiaAbsolutePercentChange, cash: PremiaMoney, buyingPower: PremiaMoney, holdingsCount: Int) {
        self.totalValue = totalValue
        self.dayChange = dayChange
        self.cash = cash
        self.buyingPower = buyingPower
        self.holdingsCount = holdingsCount
    }
}

public struct PremiaMarketSession: Codable, Sendable, Equatable {
    public let session: String
    public let nextTransitionAt: Date?

    public init(session: String, nextTransitionAt: Date?) {
        self.session = session
        self.nextTransitionAt = nextTransitionAt
    }
}

public struct PremiaHomeSnapshot: Codable, Sendable, Equatable {
    public let connections: [PremiaConnectionSummary]
    public let portfolio: PremiaPortfolioSummary
    public let topHoldings: [PremiaHoldingSummary]
    public let watchlists: [PremiaWatchlistSummaryModel]
    public let market: PremiaMarketSession
    public let asOf: Date?

    public init(
        connections: [PremiaConnectionSummary],
        portfolio: PremiaPortfolioSummary,
        topHoldings: [PremiaHoldingSummary],
        watchlists: [PremiaWatchlistSummaryModel],
        market: PremiaMarketSession,
        asOf: Date?
    ) {
        self.connections = connections
        self.portfolio = portfolio
        self.topHoldings = topHoldings
        self.watchlists = watchlists
        self.market = market
        self.asOf = asOf
    }
}
