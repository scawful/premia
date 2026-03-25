import SwiftUI
import PremiaModels

public struct HomeScreen: View {
    @EnvironmentObject private var session: PremiaAppSession
    @State private var snapshot: PremiaHomeSnapshot?
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
                        PrimaryBrokerBanner(connections: snapshot.connections)

                        VStack(alignment: .leading, spacing: 8) {
                            Text("Portfolio")
                                .font(.title2.weight(.bold))
                            Text("$\(snapshot.portfolio.totalValue.amount)")
                                .font(.system(size: 34, weight: .bold, design: .rounded))
                            Text("Day change: $\(snapshot.portfolio.dayChange.absolute.amount) (\(snapshot.portfolio.dayChange.percent)%)")
                                .foregroundStyle(.secondary)
                        }

                        ScrollView(.horizontal, showsIndicators: false) {
                            HStack(spacing: 12) {
                                ForEach(snapshot.connections) { connection in
                                    ConnectionStatusBadge(connection: connection)
                                }
                            }
                        }

                        VStack(alignment: .leading, spacing: 8) {
                            Text("Top Holdings")
                                .font(.headline)
                            ForEach(snapshot.topHoldings) { holding in
                                NavigationLink(value: PremiaRoute.quote(symbol: holding.symbol)) {
                                    HStack {
                                        VStack(alignment: .leading) {
                                            Text(holding.symbol)
                                                .font(.headline)
                                            Text(holding.name)
                                                .font(.caption)
                                                .foregroundStyle(.secondary)
                                        }
                                        Spacer()
                                        VStack(alignment: .trailing) {
                                            Text("$\(holding.marketValue.amount)")
                                            Text("\(holding.dayChange.percent)%")
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
        .navigationTitle("Home")
        .task { await load() }
    }

    @MainActor
    private func load() async {
        isLoading = true
        error = nil
        do {
            try await session.refreshAccountContext()
            snapshot = try await session.client.loadHome(accountID: session.selectedAccountID)
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "HOME_LOAD_FAILED", message: caughtError.localizedDescription)
        }
        isLoading = false
    }
}
