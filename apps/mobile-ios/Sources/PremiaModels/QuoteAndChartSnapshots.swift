import Foundation

public struct PremiaQuoteSnapshotModel: Codable, Sendable, Equatable {
    public let lastPrice: PremiaMoney
    public let bid: PremiaMoney
    public let ask: PremiaMoney
    public let open: PremiaMoney
    public let high: PremiaMoney
    public let low: PremiaMoney
    public let previousClose: PremiaMoney
    public let volume: String
    public let updatedAt: Date?

    public init(lastPrice: PremiaMoney, bid: PremiaMoney, ask: PremiaMoney, open: PremiaMoney, high: PremiaMoney, low: PremiaMoney, previousClose: PremiaMoney, volume: String, updatedAt: Date?) {
        self.lastPrice = lastPrice
        self.bid = bid
        self.ask = ask
        self.open = open
        self.high = high
        self.low = low
        self.previousClose = previousClose
        self.volume = volume
        self.updatedAt = updatedAt
    }
}

public struct PremiaQuoteScreenSnapshot: Codable, Sendable, Equatable {
    public let instrument: PremiaInstrument
    public let quote: PremiaQuoteSnapshotModel
    public let position: PremiaPositionSummary?
    public let watchlistIds: [String]
    public let asOf: Date?

    public init(instrument: PremiaInstrument, quote: PremiaQuoteSnapshotModel, position: PremiaPositionSummary?, watchlistIds: [String], asOf: Date?) {
        self.instrument = instrument
        self.quote = quote
        self.position = position
        self.watchlistIds = watchlistIds
        self.asOf = asOf
    }
}

public struct PremiaChartSnapshot: Codable, Sendable, Equatable {
    public let instrument: PremiaInstrument
    public let range: String
    public let interval: String
    public let timezone: String
    public let seriesType: String
    public let candles: [PremiaCandle]
    public let annotations: [PremiaChartAnnotation]
    public let change: PremiaAbsolutePercentChange?
    public let asOf: Date?

    public init(instrument: PremiaInstrument, range: String, interval: String, timezone: String, seriesType: String, candles: [PremiaCandle], annotations: [PremiaChartAnnotation], change: PremiaAbsolutePercentChange?, asOf: Date?) {
        self.instrument = instrument
        self.range = range
        self.interval = interval
        self.timezone = timezone
        self.seriesType = seriesType
        self.candles = candles
        self.annotations = annotations
        self.change = change
        self.asOf = asOf
    }
}

public struct PremiaChartAnnotation: Codable, Sendable, Equatable, Identifiable {
    public let id: String
    public let label: String
    public let price: String
    public let kind: String

    public init(id: String, label: String, price: String, kind: String) {
        self.id = id
        self.label = label
        self.price = price
        self.kind = kind
    }
}

public enum PremiaChartRange: String, Codable, Sendable {
    case oneDay = "1D"
    case fiveDays = "5D"
    case oneMonth = "1M"
    case threeMonths = "3M"
    case sixMonths = "6M"
    case oneYear = "1Y"
    case fiveYears = "5Y"
    case max = "MAX"
}

public enum PremiaChartInterval: String, Codable, Sendable {
    case oneMinute = "1M"
    case fiveMinutes = "5M"
    case fifteenMinutes = "15M"
    case oneHour = "1H"
    case oneDay = "1D"
    case oneWeek = "1W"
}
