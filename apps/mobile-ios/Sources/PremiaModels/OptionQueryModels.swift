import Foundation

public struct PremiaOptionChainQuery: Codable, Sendable, Equatable {
    public let symbol: String
    public let strikeCount: String
    public let strategy: String
    public let range: String
    public let expMonth: String
    public let optionType: String

    public init(
        symbol: String,
        strikeCount: String = "8",
        strategy: String = "SINGLE",
        range: String = "ALL",
        expMonth: String = "ALL",
        optionType: String = "ALL"
    ) {
        self.symbol = symbol
        self.strikeCount = strikeCount
        self.strategy = strategy
        self.range = range
        self.expMonth = expMonth
        self.optionType = optionType
    }
}
