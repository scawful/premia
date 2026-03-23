import SwiftUI
import PremiaModels

public struct WatchlistDetailScreen: View {
    @EnvironmentObject private var session: PremiaAppSession
    private let watchlistID: String
    @State private var snapshot: PremiaWatchlistScreenSnapshot?
    @State private var isLoading = false
    @State private var error: PremiaAPIClientError?

    public init(watchlistID: String) {
        self.watchlistID = watchlistID
    }

    public var body: some View {
        AsyncStateView(isLoading: isLoading, error: error, retry: { Task { await load() } }) {
            List(snapshot?.rows ?? []) { row in
                NavigationLink(value: PremiaRoute.quote(symbol: row.symbol)) {
                    HStack {
                        VStack(alignment: .leading) {
                            Text(row.symbol)
                            Text(row.name)
                                .font(.caption)
                                .foregroundStyle(.secondary)
                        }
                        Spacer()
                        Text("$\(row.lastPrice.amount)")
                    }
                }
            }
            .listStyle(.inset)
        }
        .navigationTitle(snapshot?.watchlist.name ?? watchlistID)
        .task { await load() }
    }

    @MainActor
    private func load() async {
        isLoading = true
        error = nil
        do {
            snapshot = try await session.client.loadWatchlist(id: watchlistID)
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "WATCHLIST_LOAD_FAILED", message: caughtError.localizedDescription)
        }
        isLoading = false
    }
}
