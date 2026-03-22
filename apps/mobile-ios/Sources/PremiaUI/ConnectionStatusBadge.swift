import SwiftUI
import PremiaModels

public struct ConnectionStatusBadge: View {
    private let connection: PremiaConnectionSummary

    public init(connection: PremiaConnectionSummary) {
        self.connection = connection
    }

    public var body: some View {
        HStack(spacing: 8) {
            Circle()
                .fill(color)
                .frame(width: 10, height: 10)
            Text(connection.displayName)
                .font(.headline)
            Text(label)
                .font(.caption.weight(.semibold))
                .foregroundStyle(.secondary)
        }
        .padding(.horizontal, 10)
        .padding(.vertical, 6)
        .background(.thinMaterial, in: Capsule())
    }

    private var label: String {
        switch connection.status {
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

    private var color: Color {
        switch connection.status {
        case .connected:
            return .green
        case .connecting:
            return .orange
        case .notConnected:
            return .gray
        case .degraded:
            return .yellow
        case .reauthRequired:
            return .red
        }
    }
}
