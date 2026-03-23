import Foundation
import XCTest
@testable import PremiaAPIClient
@testable import PremiaModels

@MainActor
final class AccountAndTradingIntegrationTests: XCTestCase {
    func testLoadAccountAndOptionChainSnapshots() async throws {
        let server = try await PremiaAPITestServer.start()
        defer { server.stop() }

        let client = PremiaAPIClient(configuration: PremiaAPIConfiguration(baseURL: server.baseURL))

        let account = try await client.loadAccount()
        let options = try await client.loadOptionChain(symbol: "SPY")

        XCTAssertEqual(account.accountID, "local_acc")
        XCTAssertEqual(account.cash.amount, "14320.00")
        XCTAssertFalse(account.positions.isEmpty)

        XCTAssertEqual(options.symbol, "SPY")
        XCTAssertEqual(options.description, "SPDR S&P 500 ETF")
        XCTAssertFalse(options.expirations.isEmpty)
        XCTAssertFalse(options.expirations[0].rows.isEmpty)
    }

    func testOrderLifecycleThroughClientWrappers() async throws {
        let server = try await PremiaAPITestServer.start()
        defer { server.stop() }

        let client = PremiaAPIClient(configuration: PremiaAPIConfiguration(baseURL: server.baseURL))
        let intent = PremiaOrderIntent(symbol: "AAPL", assetType: .equity, instruction: .buy, quantity: "2", orderType: .limit, limitPrice: "217.00")

        let submission = try await client.submitOrder(intent)
        var openOrders = try await client.loadOpenOrders()
        var history = try await client.loadOrderHistory()

        XCTAssertEqual(submission.status, "accepted")
        XCTAssertTrue(openOrders.contains(where: { $0.id == submission.submissionID }))
        XCTAssertTrue(history.contains(where: { $0.id == submission.submissionID }))

        let cancellation = try await client.cancelOrder(orderID: submission.submissionID)
        openOrders = try await client.loadOpenOrders()
        history = try await client.loadOrderHistory()

        XCTAssertEqual(cancellation.status, "cancelled")
        XCTAssertFalse(openOrders.contains(where: { $0.id == submission.submissionID }))
        XCTAssertTrue(history.contains(where: { $0.id == submission.submissionID && $0.status == "cancelled" }))

        let replacement = try await client.replaceOrder(
            PremiaOrderReplaceIntent(
                orderID: submission.submissionID,
                replacement: PremiaOrderIntent(symbol: "AAPL", assetType: .equity, instruction: .buy, quantity: "1", orderType: .limit, limitPrice: "216.50")
            )
        )

        openOrders = try await client.loadOpenOrders()
        XCTAssertTrue(openOrders.contains(where: { $0.id == replacement.replacementID }))
    }
}
