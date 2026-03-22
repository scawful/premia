import Foundation
import PremiaModels
import PremiaAPIClientGenerated

public struct PremiaAPIConfiguration: Sendable, Equatable {
    public let baseURL: URL

    public init(baseURL: URL) {
        self.baseURL = baseURL
    }
}

public final class PremiaAPIClient: @unchecked Sendable {
    public let configuration: PremiaAPIConfiguration

    public init(configuration: PremiaAPIConfiguration) {
        self.configuration = configuration
        PremiaAPIClientGeneratedAPI.basePath = configuration.baseURL.absoluteString
    }

    public func bootstrapURL() -> URL {
        configuration.baseURL.appending(path: "v1/bootstrap")
    }

    public func homeURL() -> URL {
        configuration.baseURL.appending(path: "v1/screens/home")
    }

    public func quoteURL(symbol: String) -> URL {
        configuration.baseURL.appending(path: "v1/screens/quotes").appending(path: symbol)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func loadBootstrap() async throws -> PremiaBootstrapSnapshot {
        let response = try await BootstrapAPI.getBootstrap()
        let connections = response.data.connections.map(mapConnection)
        return PremiaBootstrapSnapshot(
            environment: response.data.session.environment,
            featureFlags: response.data.session.featureFlags,
            connections: connections,
            asOf: response.meta.asOf
        )
    }

    private func mapConnection(_ connection: PremiaAPIClientGeneratedAPI.ConnectionSummary) -> PremiaConnectionSummary {
        PremiaConnectionSummary(
            id: connection.provider.rawValue,
            provider: mapProvider(connection.provider),
            status: mapStatus(connection.status),
            displayName: connection.displayName,
            lastSyncAt: connection.lastSyncAt,
            reauthRequired: connection.reauthRequired,
            capabilities: connection.capabilities
        )
    }

    private func mapProvider(_ provider: PremiaAPIClientGeneratedAPI.Provider) -> PremiaProvider {
        switch provider {
        case .tda:
            return .tda
        case .schwab:
            return .schwab
        case .ibkr:
            return .ibkr
        case .plaid:
            return .plaid
        case ._internal, .unknownDefaultOpenApi:
            return .internal
        }
    }

    private func mapStatus(_ status: PremiaAPIClientGeneratedAPI.ConnectionStatus) -> PremiaConnectionStatus {
        switch status {
        case .connected:
            return .connected
        case .connecting:
            return .connecting
        case .notConnected:
            return .notConnected
        case .degraded:
            return .degraded
        case .reauthRequired, .unknownDefaultOpenApi:
            return .reauthRequired
        }
    }
}
