import Foundation

public struct PremiaMoney: Codable, Sendable, Equatable {
    public let amount: String
    public let currency: String

    public init(amount: String, currency: String) {
        self.amount = amount
        self.currency = currency
    }
}

public struct PremiaAbsolutePercentChange: Codable, Sendable, Equatable {
    public let absolute: PremiaMoney
    public let percent: String

    public init(absolute: PremiaMoney, percent: String) {
        self.absolute = absolute
        self.percent = percent
    }
}

public struct PremiaInstrument: Codable, Sendable, Equatable {
    public let symbol: String
    public let name: String
    public let assetType: String
    public let primaryExchange: String?

    public init(symbol: String, name: String, assetType: String, primaryExchange: String?) {
        self.symbol = symbol
        self.name = name
        self.assetType = assetType
        self.primaryExchange = primaryExchange
    }
}

public struct PremiaPositionSummary: Codable, Sendable, Equatable {
    public let quantity: String?
    public let marketValue: PremiaMoney?
    public let costBasis: PremiaMoney?
    public let unrealizedGain: PremiaMoney?

    public init(quantity: String?, marketValue: PremiaMoney?, costBasis: PremiaMoney?, unrealizedGain: PremiaMoney?) {
        self.quantity = quantity
        self.marketValue = marketValue
        self.costBasis = costBasis
        self.unrealizedGain = unrealizedGain
    }
}

public struct PremiaCandle: Codable, Sendable, Equatable, Identifiable {
    public let id: String
    public let time: String
    public let open: String
    public let high: String
    public let low: String
    public let close: String
    public let volume: String

    public init(time: String, open: String, high: String, low: String, close: String, volume: String) {
        self.id = time
        self.time = time
        self.open = open
        self.high = high
        self.low = low
        self.close = close
        self.volume = volume
    }
}
