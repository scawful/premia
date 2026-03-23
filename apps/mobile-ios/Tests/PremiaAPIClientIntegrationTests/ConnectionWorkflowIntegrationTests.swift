import Foundation
import XCTest
@testable import PremiaAPIClient
@testable import PremiaModels

@MainActor
final class ConnectionWorkflowIntegrationTests: XCTestCase {
    func testSchwabAndPlaidWorkflowWrappersUpdateBootstrapState() async throws {
        let server = try await PremiaAPITestServer.start()
        defer { server.stop() }

        let client = PremiaAPIClient(configuration: PremiaAPIConfiguration(baseURL: server.baseURL))

        let schwabLaunch = try await client.startSchwabOAuth(redirectURI: "premia://schwab/callback", clientPlatform: "desktop")
        XCTAssertTrue(schwabLaunch.authURL.contains("oauth/authorize"))
        XCTAssertFalse(schwabLaunch.state.isEmpty)

        let schwabConnection = try await client.completeSchwabOAuth(callback: "demo-auth-code")
        XCTAssertEqual(schwabConnection.provider, .schwab)
        XCTAssertEqual(schwabConnection.status, .connected)

        let plaidLaunch = try await client.createPlaidLinkToken(userID: "integration-user")
        XCTAssertTrue(plaidLaunch.linkToken.contains("link-sandbox-"))

        let plaidConnection = try await client.completePlaidLink(publicToken: "public-sandbox-token", institutionID: "ins_123")
        XCTAssertEqual(plaidConnection.provider, .plaid)
        XCTAssertEqual(plaidConnection.status, .connected)

        let bootstrap = try await client.loadBootstrap()
        XCTAssertTrue(bootstrap.connections.contains(where: { $0.provider == .schwab && $0.status == .connected }))
        XCTAssertTrue(bootstrap.connections.contains(where: { $0.provider == .plaid && $0.status == .connected }))
    }

    func testSeededBootstrapReflectsSchwabReauthAndPlaidConnected() async throws {
        let server = try await PremiaAPITestServer.start(assetOverrides: [
            "schwab.json": "{\n  \"app_key\": \"real-looking-key\",\n  \"app_secret\": \"real-looking-secret\"\n}\n",
            "schwab_tokens.json": "{\n  \"refresh_token\": \"expired-refresh\",\n  \"refresh_token_expires_at\": 1\n}\n",
            "plaid.json": "{\n  \"client_id\": \"real-looking-client-id\",\n  \"secret\": \"real-looking-secret\"\n}\n",
            "plaid_tokens.json": "{\n  \"access_token\": \"access-sandbox-demo\"\n}\n"
        ])
        defer { server.stop() }

        let client = PremiaAPIClient(configuration: PremiaAPIConfiguration(baseURL: server.baseURL))
        let bootstrap = try await client.loadBootstrap()

        XCTAssertTrue(bootstrap.connections.contains(where: {
            $0.provider == .schwab && $0.status == .reauthRequired && $0.reauthRequired
        }))
        XCTAssertTrue(bootstrap.connections.contains(where: {
            $0.provider == .plaid && $0.status == .connected && !$0.reauthRequired
        }))
    }
}
