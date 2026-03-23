import SwiftUI
import PremiaAPIClient

public struct PremiaRootView: View {
    @StateObject private var session: PremiaAppSession

    public init(baseURL: URL) {
        _session = StateObject(wrappedValue: PremiaAppSession(baseURL: baseURL))
    }

    public init(client: PremiaAPIClient) {
        _session = StateObject(wrappedValue: PremiaAppSession(client: client))
    }

    public var body: some View {
        TabView {
            NavigationStack {
                HomeScreen()
                    .navigationDestination(for: PremiaRoute.self, destination: destination)
            }
            .tabItem { Label("Home", systemImage: "house") }

            NavigationStack {
                WatchlistsScreen()
                    .navigationDestination(for: PremiaRoute.self, destination: destination)
            }
            .tabItem { Label("Watchlists", systemImage: "list.bullet.rectangle") }

            NavigationStack {
                OrdersScreen()
                    .navigationDestination(for: PremiaRoute.self, destination: destination)
            }
            .tabItem { Label("Orders", systemImage: "arrow.left.arrow.right.circle") }

            NavigationStack {
                AccountScreen()
                    .navigationDestination(for: PremiaRoute.self, destination: destination)
            }
            .tabItem { Label("Account", systemImage: "person.crop.circle") }
        }
        .environmentObject(session)
    }

    @ViewBuilder
    private func destination(for route: PremiaRoute) -> some View {
        switch route {
        case .watchlist(let id):
            WatchlistDetailScreen(watchlistID: id)
        case .quote(let symbol):
            QuoteScreen(symbol: symbol)
        case .chart(let symbol):
            ChartScreen(symbol: symbol)
        }
    }
}
