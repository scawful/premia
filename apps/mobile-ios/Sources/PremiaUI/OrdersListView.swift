import SwiftUI
import PremiaModels

public struct OrdersListView: View {
    private let title: String
    private let orders: [PremiaOrderRecord]

    public init(title: String, orders: [PremiaOrderRecord]) {
        self.title = title
        self.orders = orders
    }

    public var body: some View {
        VStack(alignment: .leading, spacing: 12) {
            Text(title)
                .font(.title3.weight(.semibold))

            if orders.isEmpty {
                Text("No orders available.")
                    .foregroundStyle(.secondary)
            } else {
                ForEach(orders) { order in
                    VStack(alignment: .leading, spacing: 6) {
                        HStack {
                            Text(order.symbol)
                                .font(.headline)
                            Spacer()
                            Text(order.status)
                                .font(.caption.weight(.semibold))
                                .foregroundStyle(.secondary)
                        }
                        Text("\(order.instruction.rawValue) \(order.quantity) @ \(order.limitPrice)")
                            .font(.subheadline)
                        Text(order.message)
                            .font(.caption)
                            .foregroundStyle(.secondary)
                    }
                    .padding(12)
                    .background(.thinMaterial, in: RoundedRectangle(cornerRadius: 14))
                }
            }
        }
    }
}
