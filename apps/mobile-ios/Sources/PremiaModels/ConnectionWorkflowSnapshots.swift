import Foundation

public struct PremiaSchwabOAuthLaunch: Codable, Sendable, Equatable {
    public let authURL: String
    public let state: String
    public let expiresAt: Date?

    public init(authURL: String, state: String, expiresAt: Date?) {
        self.authURL = authURL
        self.state = state
        self.expiresAt = expiresAt
    }
}

public struct PremiaPlaidLinkLaunch: Codable, Sendable, Equatable {
    public let linkToken: String
    public let expiration: Date?

    public init(linkToken: String, expiration: Date?) {
        self.linkToken = linkToken
        self.expiration = expiration
    }
}
