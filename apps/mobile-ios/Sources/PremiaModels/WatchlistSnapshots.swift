import Foundation

public struct PremiaWatchlistRowModel: Codable, Sendable, Equatable, Identifiable {
    public let id: String
    public let symbol: String
    public let name: String
    public let lastPrice: PremiaMoney
    public let dayChange: PremiaAbsolutePercentChange
    public let bid: PremiaMoney?
    public let ask: PremiaMoney?
    public let updatedAt: Date?
    public let isPinned: Bool

    public init(
        id: String,
        symbol: String,
        name: String,
        lastPrice: PremiaMoney,
        dayChange: PremiaAbsolutePercentChange,
        bid: PremiaMoney?,
        ask: PremiaMoney?,
        updatedAt: Date?,
        isPinned: Bool
    ) {
        self.id = id
        self.symbol = symbol
        self.name = name
        self.lastPrice = lastPrice
        self.dayChange = dayChange
        self.bid = bid
        self.ask = ask
        self.updatedAt = updatedAt
        self.isPinned = isPinned
    }
}

public struct PremiaWatchlistScreenSnapshot: Codable, Sendable, Equatable {
    public let watchlist: PremiaWatchlistSummaryModel
    public let availableWatchlists: [PremiaWatchlistSummaryModel]
    public let rows: [PremiaWatchlistRowModel]
    public let asOf: Date?

    public init(
        watchlist: PremiaWatchlistSummaryModel,
        availableWatchlists: [PremiaWatchlistSummaryModel],
        rows: [PremiaWatchlistRowModel],
        asOf: Date?
    ) {
        self.watchlist = watchlist
        self.availableWatchlists = availableWatchlists
        self.rows = rows
        self.asOf = asOf
    }
}
