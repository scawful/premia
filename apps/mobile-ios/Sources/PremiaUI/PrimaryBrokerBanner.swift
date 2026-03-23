import SwiftUI
import PremiaModels

public struct PrimaryBrokerBanner: View {
    private let connection: PremiaConnectionSummary?

    public init(connections: [PremiaConnectionSummary]) {
        self.connection = connections.first(where: { $0.provider == .schwab })
    }

    public var body: some View {
        if let connection {
            HStack(spacing: 10) {
                Image(systemName: "building.columns")
                VStack(alignment: .leading, spacing: 2) {
                    Text("Primary Brokerage")
                        .font(.caption.weight(.semibold))
                        .foregroundStyle(.secondary)
                    Text("\(connection.displayName) · \(label(for: connection.status))")
                        .font(.headline)
                }
                Spacer()
            }
            .padding(14)
            .background(
                LinearGradient(
                    colors: [Color.blue.opacity(0.14), Color.cyan.opacity(0.08)],
                    startPoint: .topLeading,
                    endPoint: .bottomTrailing
                ),
                in: RoundedRectangle(cornerRadius: 18)
            )
        }
    }

    private func label(for status: PremiaConnectionStatus) -> String {
        switch status {
        case .connected:
            return "Connected"
        case .connecting:
            return "Connecting"
        case .notConnected:
            return "Not Connected"
        case .degraded:
            return "Degraded"
        case .reauthRequired:
            return "Reauth Required"
        }
    }
}
