import Foundation
import PremiaAPIClient
import PremiaModels

@MainActor
public final class PremiaAppSession: ObservableObject {
    public let client: PremiaAPIClient

    @Published public var selectedAccountID: String?
    @Published public var brokerageAccounts: [PremiaBrokerageAccountSummaryModel] = []

    public init(client: PremiaAPIClient) {
        self.client = client
    }

    public convenience init(baseURL: URL) {
        self.init(client: PremiaAPIClient(configuration: PremiaAPIConfiguration(baseURL: baseURL)))
    }

    public func refreshAccountContext() async throws {
        let home = try await client.loadHome(accountID: selectedAccountID)
        brokerageAccounts = home.brokerageAccounts
        if selectedAccountID == nil || !brokerageAccounts.contains(where: { $0.id == selectedAccountID }) {
          selectedAccountID = home.activeAccountID
        }
    }
}
