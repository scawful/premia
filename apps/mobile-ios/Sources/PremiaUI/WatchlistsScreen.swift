import SwiftUI
import PremiaModels

public struct WatchlistsScreen: View {
    @EnvironmentObject private var session: PremiaAppSession
    @State private var watchlists: [PremiaWatchlistSummaryModel] = []
    @State private var isLoading = false
    @State private var error: PremiaAPIClientError?

    public init() {}

    public var body: some View {
        AsyncStateView(isLoading: isLoading, error: error, retry: { Task { await load() } }) {
            VStack(alignment: .leading, spacing: 16) {
                Text("Primary Brokerage · Charles Schwab")
                    .font(.caption.weight(.semibold))
                    .foregroundStyle(.secondary)

                List(watchlists) { watchlist in
                    NavigationLink(value: PremiaRoute.watchlist(id: watchlist.id)) {
                        VStack(alignment: .leading) {
                            Text(watchlist.name)
                            Text("\(watchlist.instrumentCount) instruments")
                                .font(.caption)
                                .foregroundStyle(.secondary)
                        }
                    }
                }
                .listStyle(.inset)
            }
            .padding(.horizontal)
        }
        .navigationTitle("Watchlists")
        .task { await load() }
    }

    @MainActor
    private func load() async {
        isLoading = true
        error = nil
        do {
            watchlists = try await session.client.loadWatchlists()
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "WATCHLISTS_LOAD_FAILED", message: caughtError.localizedDescription)
        }
        isLoading = false
    }
}
