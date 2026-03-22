import Foundation
import PremiaModels

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
}
