import Foundation

public struct PremiaBootstrapSnapshot: Codable, Sendable, Equatable {
    public let environment: String
    public let featureFlags: [String: Bool]
    public let connections: [PremiaConnectionSummary]
    public let asOf: Date?

    public init(
        environment: String,
        featureFlags: [String: Bool],
        connections: [PremiaConnectionSummary],
        asOf: Date?
    ) {
        self.environment = environment
        self.featureFlags = featureFlags
        self.connections = connections
        self.asOf = asOf
    }
}
