import SwiftUI
import PremiaModels

public struct OrdersScreen: View {
    @EnvironmentObject private var session: PremiaAppSession
    @State private var selection = 0
    @State private var openOrders: [PremiaOrderRecord] = []
    @State private var orderHistory: [PremiaOrderRecord] = []
    @State private var isLoading = false
    @State private var error: PremiaAPIClientError?

    public init() {}

    public var body: some View {
        AsyncStateView(isLoading: isLoading, error: error, retry: { Task { await load() } }) {
            ScrollView {
                VStack(alignment: .leading, spacing: 16) {
                    Text("Primary Brokerage · Charles Schwab")
                        .font(.caption.weight(.semibold))
                        .foregroundStyle(.secondary)

                    if !session.brokerageAccounts.isEmpty {
                        Picker("Account", selection: Binding(
                            get: { session.selectedAccountID ?? session.brokerageAccounts.first?.id ?? "" },
                            set: { newValue in
                                session.selectedAccountID = newValue
                                Task { await load() }
                            }
                        )) {
                            ForEach(session.brokerageAccounts) { account in
                                Text(account.displayName).tag(account.id)
                            }
                        }
                        .pickerStyle(.menu)
                    }

                    Picker("Orders", selection: $selection) {
                        Text("Open").tag(0)
                        Text("History").tag(1)
                    }
                    .pickerStyle(.segmented)

                    OrdersListView(title: selection == 0 ? "Open Orders" : "Order History",
                                   orders: selection == 0 ? openOrders : orderHistory)
                }
                .padding()
            }
        }
        .navigationTitle("Orders")
        .task { await load() }
    }

    @MainActor
    private func load() async {
        isLoading = true
        error = nil
        do {
            try await session.refreshAccountContext()
            async let open = session.client.loadOpenOrders(accountID: session.selectedAccountID)
            async let history = session.client.loadOrderHistory(accountID: session.selectedAccountID)
            openOrders = try await open
            orderHistory = try await history
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "ORDERS_LOAD_FAILED", message: caughtError.localizedDescription)
        }
        isLoading = false
    }
}
