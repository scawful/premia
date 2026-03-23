import Foundation
import XCTest
@testable import PremiaUI
@testable import PremiaModels

@MainActor
final class PremiaUIScreenSmokeTests: XCTestCase {
    func testPremiaAppSessionUsesConfiguredBaseURL() {
        let url = URL(string: "http://127.0.0.1:8080")!
        let session = PremiaAppSession(baseURL: url)

        XCTAssertEqual(session.client.configuration.baseURL, url)
    }

    func testPremiaRootViewConstructs() {
        let view = PremiaRootView(baseURL: URL(string: "http://127.0.0.1:8080")!)
        XCTAssertFalse(String(describing: type(of: view.body)).isEmpty)
    }

    func testOrdersListViewConstructsWithRecords() {
        let record = PremiaOrderRecord(
            id: "order-1",
            accountID: "acct-1",
            symbol: "AAPL",
            assetType: .equity,
            instruction: .buy,
            quantity: "1",
            orderType: .limit,
            limitPrice: "217.00",
            mode: "simulated",
            status: "accepted",
            submittedAt: Date(timeIntervalSince1970: 100),
            updatedAt: Date(timeIntervalSince1970: 100),
            message: "Stored locally."
        )

        let view = OrdersListView(title: "Open Orders", orders: [record])
        XCTAssertFalse(String(describing: type(of: view.body)).isEmpty)
    }
}
