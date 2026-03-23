import SwiftUI
import PremiaModels

public struct ChartScreen: View {
    @EnvironmentObject private var session: PremiaAppSession
    private let symbol: String
    @State private var snapshot: PremiaChartSnapshot?
    @State private var isLoading = false
    @State private var error: PremiaAPIClientError?

    public init(symbol: String) {
        self.symbol = symbol
    }

    public var body: some View {
        AsyncStateView(isLoading: isLoading, error: error, retry: { Task { await load() } }) {
            List(snapshot?.candles ?? []) { candle in
                VStack(alignment: .leading) {
                    Text(candle.time)
                        .font(.caption)
                        .foregroundStyle(.secondary)
                    Text("O \(candle.open) H \(candle.high) L \(candle.low) C \(candle.close)")
                }
            }
            .listStyle(.inset)
        }
        .navigationTitle("\(symbol) Chart")
        .task { await load() }
    }

    @MainActor
    private func load() async {
        isLoading = true
        error = nil
        do {
            snapshot = try await session.client.loadChart(symbol: symbol)
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "CHART_LOAD_FAILED", message: caughtError.localizedDescription)
        }
        isLoading = false
    }
}
