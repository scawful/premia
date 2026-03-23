import XCTest
@testable import PremiaModels

final class OrderSnapshotsTests: XCTestCase {
    func testOrderIntentCodableRoundTripPreservesTradingFields() throws {
        let original = PremiaOrderIntent(
            accountID: "acct-1",
            symbol: "AAPL",
            assetType: .equity,
            instruction: .buy,
            quantity: "10",
            orderType: .limit,
            limitPrice: "217.00",
            duration: "DAY",
            session: "NORMAL",
            confirmLive: true
        )

        let data = try JSONEncoder().encode(original)
        let decoded = try JSONDecoder().decode(PremiaOrderIntent.self, from: data)

        XCTAssertEqual(decoded, original)
    }

    func testOrderRecordCodableRoundTripPreservesIdentity() throws {
        let original = PremiaOrderRecord(
            id: "submission_123",
            accountID: "acct-1",
            symbol: "SPY",
            assetType: .equity,
            instruction: .buy,
            quantity: "2",
            orderType: .limit,
            limitPrice: "519.50",
            mode: "simulated",
            status: "accepted",
            submittedAt: Date(timeIntervalSince1970: 100),
            updatedAt: Date(timeIntervalSince1970: 120),
            message: "Stored locally."
        )

        let data = try JSONEncoder().encode(original)
        let decoded = try JSONDecoder().decode(PremiaOrderRecord.self, from: data)

        XCTAssertEqual(decoded, original)
    }
}
