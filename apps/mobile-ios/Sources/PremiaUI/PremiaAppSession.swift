import Foundation
import PremiaAPIClient

@MainActor
public final class PremiaAppSession: ObservableObject {
    public let client: PremiaAPIClient

    @Published public var selectedAccountID: String?

    public init(client: PremiaAPIClient) {
        self.client = client
    }

    public convenience init(baseURL: URL) {
        self.init(client: PremiaAPIClient(configuration: PremiaAPIConfiguration(baseURL: baseURL)))
    }
}
