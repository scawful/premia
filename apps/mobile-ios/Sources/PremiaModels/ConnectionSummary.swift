import Foundation

public enum PremiaProvider: String, Codable, Sendable {
    case tda
    case schwab
    case ibkr
    case plaid
    case `internal`
}

public enum PremiaConnectionStatus: String, Codable, Sendable {
    case connected
    case connecting
    case notConnected = "not_connected"
    case degraded
    case reauthRequired = "reauth_required"
}

public struct PremiaConnectionSummary: Codable, Identifiable, Sendable, Equatable {
    public let id: String
    public let provider: PremiaProvider
    public let status: PremiaConnectionStatus
    public let displayName: String
    public let lastSyncAt: Date?
    public let reauthRequired: Bool
    public let capabilities: [String: Bool]

    public init(
        id: String,
        provider: PremiaProvider,
        status: PremiaConnectionStatus,
        displayName: String,
        lastSyncAt: Date?,
        reauthRequired: Bool,
        capabilities: [String: Bool]
    ) {
        self.id = id
        self.provider = provider
        self.status = status
        self.displayName = displayName
        self.lastSyncAt = lastSyncAt
        self.reauthRequired = reauthRequired
        self.capabilities = capabilities
    }
}
