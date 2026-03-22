import Foundation

public enum PremiaAPIErrorAction: String, Codable, Sendable, Equatable {
    case none
    case retry
    case reconnect
    case reauth
    case relink
    case unknown
}

public struct PremiaAPIClientError: Error, Codable, Sendable, Equatable, LocalizedError {
    public let code: String
    public let message: String
    public let statusCode: Int?
    public let provider: PremiaProvider?
    public let retryable: Bool
    public let action: PremiaAPIErrorAction

    public init(
        code: String,
        message: String,
        statusCode: Int? = nil,
        provider: PremiaProvider? = nil,
        retryable: Bool = false,
        action: PremiaAPIErrorAction = .unknown
    ) {
        self.code = code
        self.message = message
        self.statusCode = statusCode
        self.provider = provider
        self.retryable = retryable
        self.action = action
    }

    public var errorDescription: String? {
        message
    }
}
