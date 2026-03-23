import SwiftUI
import PremiaModels

public struct QuoteScreen: View {
    @EnvironmentObject private var session: PremiaAppSession
    private let symbol: String
    @State private var snapshot: PremiaQuoteScreenSnapshot?
    @State private var isLoading = false
    @State private var error: PremiaAPIClientError?

    public init(symbol: String) {
        self.symbol = symbol
    }

    public var body: some View {
        AsyncStateView(isLoading: isLoading, error: error, retry: { Task { await load() } }) {
            ScrollView {
                VStack(alignment: .leading, spacing: 16) {
                    if let snapshot {
                        Text(snapshot.instrument.name)
                            .font(.title2.weight(.bold))
                        Text("$\(snapshot.quote.lastPrice.amount)")
                            .font(.system(size: 34, weight: .bold, design: .rounded))
                        Text("Bid $\(snapshot.quote.bid.amount) / Ask $\(snapshot.quote.ask.amount)")
                            .foregroundStyle(.secondary)

                        NavigationLink(value: PremiaRoute.chart(symbol: symbol)) {
                            Label("View Chart", systemImage: "chart.xyaxis.line")
                                .frame(maxWidth: .infinity)
                        }
                        .buttonStyle(.borderedProminent)
                    }
                }
                .padding()
            }
        }
        .navigationTitle(symbol)
        .task { await load() }
    }

    @MainActor
    private func load() async {
        isLoading = true
        error = nil
        do {
            snapshot = try await session.client.loadQuote(symbol: symbol)
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "QUOTE_LOAD_FAILED", message: caughtError.localizedDescription)
        }
        isLoading = false
    }
}
