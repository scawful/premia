import SwiftUI
import PremiaModels

public struct AccountScreen: View {
    @EnvironmentObject private var session: PremiaAppSession
    @State private var snapshot: PremiaAccountDetailSnapshot?
    @State private var isLoading = false
    @State private var error: PremiaAPIClientError?

    public init() {}

    public var body: some View {
        AsyncStateView(isLoading: isLoading, error: error, retry: { Task { await load() } }) {
            ScrollView {
                VStack(alignment: .leading, spacing: 16) {
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

                    if let snapshot {
                        Text("Primary Brokerage · Charles Schwab")
                            .font(.caption.weight(.semibold))
                            .foregroundStyle(.secondary)

                        VStack(alignment: .leading, spacing: 6) {
                            Text("Account \(snapshot.accountID)")
                                .font(.title2.weight(.bold))
                            Text("Net liquidation: $\(snapshot.netLiquidation.amount)")
                            Text("Buying power: $\(snapshot.buyingPower.amount)")
                                .foregroundStyle(.secondary)
                        }

                        VStack(alignment: .leading, spacing: 8) {
                            Text("Positions")
                                .font(.headline)
                            ForEach(snapshot.positions) { position in
                                NavigationLink(value: PremiaRoute.quote(symbol: position.symbol)) {
                                    HStack {
                                        VStack(alignment: .leading) {
                                            Text(position.symbol)
                                                .font(.headline)
                                            Text(position.name)
                                                .font(.caption)
                                                .foregroundStyle(.secondary)
                                        }
                                        Spacer()
                                        VStack(alignment: .trailing) {
                                            Text("$\(position.marketValue.amount)")
                                            Text(position.quantity)
                                                .font(.caption)
                                                .foregroundStyle(.secondary)
                                        }
                                    }
                                    .padding(12)
                                    .background(.thinMaterial, in: RoundedRectangle(cornerRadius: 14))
                                }
                                .buttonStyle(.plain)
                            }
                        }
                    }
                }
                .padding()
            }
        }
        .navigationTitle("Account")
        .task { await load() }
    }

    @MainActor
    private func load() async {
        isLoading = true
        error = nil
        do {
            try await session.refreshAccountContext()
            snapshot = try await session.client.loadAccount(accountID: session.selectedAccountID)
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "ACCOUNT_LOAD_FAILED", message: caughtError.localizedDescription)
        }
        isLoading = false
    }
}
