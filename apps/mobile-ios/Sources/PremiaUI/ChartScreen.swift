import SwiftUI
import Charts
import PremiaModels

public struct ChartScreen: View {
    @EnvironmentObject private var session: PremiaAppSession
    private let symbol: String
    @State private var snapshot: PremiaChartSnapshot?
    @State private var annotations: [PremiaChartAnnotation] = []
    @State private var isLoading = false
    @State private var error: PremiaAPIClientError?
    @State private var newAnnotationLabel = ""
    @State private var newAnnotationPrice = ""
    @State private var tradeEntryPrice = ""
    @State private var tradeStopPrice = ""
    @State private var tradeTargetPrice = ""

    public init(symbol: String) {
        self.symbol = symbol
    }

    public var body: some View {
        AsyncStateView(isLoading: isLoading, error: error, retry: { Task { await load() } }) {
            VStack(alignment: .leading, spacing: 12) {
                Text("Primary Brokerage · Charles Schwab Market Data")
                    .font(.caption.weight(.semibold))
                    .foregroundStyle(.secondary)

                if let snapshot {
                    Chart(snapshot.candles) { candle in
                        LineMark(
                            x: .value("Time", candle.time),
                            y: .value("Close", closeValue(for: candle))
                        )
                        .foregroundStyle(.blue)

                        ForEach(annotations) { annotation in
                            RuleMark(y: .value("Price", priceValue(annotation.price)))
                                .foregroundStyle(annotationColor(annotation.kind))
                                .lineStyle(StrokeStyle(lineWidth: 1, dash: annotation.kind == "annotation" ? [4, 4] : []))
                                .annotation(position: .leading, spacing: 4) {
                                    Text(annotation.label)
                                        .font(.caption2)
                                        .padding(.horizontal, 6)
                                        .padding(.vertical, 2)
                                        .background(.thinMaterial, in: Capsule())
                                }
                        }
                    }
                    .frame(height: 240)
                }

                VStack(alignment: .leading, spacing: 12) {
                    Text("Overlay Editor")
                        .font(.headline)
                    Text("Mobile edits now write through the API and sync back into shared chart annotations.")
                        .font(.caption)
                        .foregroundStyle(.secondary)

                    HStack {
                        TextField("Marker label", text: $newAnnotationLabel)
                        TextField("Price", text: $newAnnotationPrice)
                            .frame(width: 96)
                        Button("Add") {
                            addAnnotation()
                        }
                        .disabled(newAnnotationLabel.isEmpty || newAnnotationPrice.isEmpty)
                    }

                    HStack {
                        TextField("Entry", text: $tradeEntryPrice)
                            .frame(width: 92)
                        TextField("Stop", text: $tradeStopPrice)
                            .frame(width: 92)
                        TextField("Target", text: $tradeTargetPrice)
                            .frame(width: 92)
                        Button("Apply Risk Box") {
                            applyRiskBox()
                        }
                        .disabled(tradeEntryPrice.isEmpty)
                        Button("Clear") {
                            clearRiskBox()
                        }
                    }
                }
                .padding()
                .background(.thinMaterial, in: RoundedRectangle(cornerRadius: 16, style: .continuous))

                if !annotations.isEmpty {
                    VStack(alignment: .leading, spacing: 8) {
                        Text("Chart Markers")
                            .font(.headline)
                        ForEach(annotations) { annotation in
                            HStack {
                                Text(annotation.label)
                                Spacer()
                                Text(annotation.kind.uppercased())
                                    .font(.caption2.weight(.semibold))
                                    .foregroundStyle(.secondary)
                                Text("$\(annotation.price)")
                                if isEditable(annotation) {
                                    Button("-0.50") {
                                        nudge(annotationID: annotation.id, delta: -0.5)
                                    }
                                    .buttonStyle(.bordered)
                                    Button("+0.50") {
                                        nudge(annotationID: annotation.id, delta: 0.5)
                                    }
                                    .buttonStyle(.bordered)
                                    Button(role: .destructive) {
                                        remove(annotationID: annotation.id)
                                    } label: {
                                        Image(systemName: "trash")
                                    }
                                } else {
                                    Text("System")
                                        .font(.caption2)
                                        .foregroundStyle(.secondary)
                                }
                            }
                            .font(.caption)
                        }
                    }
                    .padding()
                    .background(.thinMaterial, in: RoundedRectangle(cornerRadius: 16, style: .continuous))
                }

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
        }
        .navigationTitle("\(symbol) Chart")
        .task { await load() }
    }

    private func closeValue(for candle: PremiaCandle) -> Double {
        Double(candle.close) ?? 0
    }

    private func priceValue(_ raw: String) -> Double {
        Double(raw) ?? 0
    }

    private func annotationColor(_ kind: String) -> Color {
        switch kind {
        case "avg_cost": return .blue
        case "order": return .green
        case "fill": return .orange
        case "entry": return .yellow
        case "stop": return .red
        case "target": return .mint
        default: return .secondary
        }
    }

    private func isEditable(_ annotation: PremiaChartAnnotation) -> Bool {
        ["annotation", "entry", "stop", "target"].contains(annotation.kind)
    }

    private func addAnnotation() {
        annotations.append(
            PremiaChartAnnotation(
                id: "mobile-\(UUID().uuidString)",
                label: newAnnotationLabel,
                price: newAnnotationPrice,
                kind: "annotation"
            )
        )
        newAnnotationLabel = ""
        newAnnotationPrice = ""
        Task { await persistAnnotations() }
    }

    private func nudge(annotationID: String, delta: Double) {
        annotations = annotations.map { annotation in
            guard annotation.id == annotationID else { return annotation }
            let updated = (Double(annotation.price) ?? 0) + delta
            return PremiaChartAnnotation(
                id: annotation.id,
                label: annotation.label,
                price: String(format: "%.2f", updated),
                kind: annotation.kind
            )
        }
        Task { await persistAnnotations() }
    }

    private func remove(annotationID: String) {
        annotations.removeAll { $0.id == annotationID }
        Task { await persistAnnotations() }
    }

    private func applyRiskBox() {
        upsertRiskMarker(kind: "entry", label: "Entry", price: tradeEntryPrice)
        if !tradeStopPrice.isEmpty {
            upsertRiskMarker(kind: "stop", label: "Stop", price: tradeStopPrice)
        }
        if !tradeTargetPrice.isEmpty {
            upsertRiskMarker(kind: "target", label: "Target", price: tradeTargetPrice)
        }
        Task { await persistAnnotations() }
    }

    private func clearRiskBox() {
        annotations.removeAll { ["entry", "stop", "target"].contains($0.kind) }
        tradeEntryPrice = ""
        tradeStopPrice = ""
        tradeTargetPrice = ""
        Task { await persistAnnotations() }
    }

    private func upsertRiskMarker(kind: String, label: String, price: String) {
        if let index = annotations.firstIndex(where: { $0.kind == kind }) {
            annotations[index] = PremiaChartAnnotation(id: annotations[index].id, label: label, price: price, kind: kind)
        } else {
            annotations.append(PremiaChartAnnotation(id: "mobile-\(kind)", label: label, price: price, kind: kind))
        }
    }

    @MainActor
    private func persistAnnotations() async {
        do {
            let editableAnnotations = annotations.filter(isEditable)
            let updated = try await session.client.replaceChartAnnotations(symbol: symbol, annotations: editableAnnotations)
            snapshot = updated
            annotations = updated.annotations
            tradeEntryPrice = annotations.first(where: { $0.kind == "entry" })?.price ?? ""
            tradeStopPrice = annotations.first(where: { $0.kind == "stop" })?.price ?? ""
            tradeTargetPrice = annotations.first(where: { $0.kind == "target" })?.price ?? ""
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "CHART_ANNOTATION_SAVE_FAILED", message: caughtError.localizedDescription)
        }
    }

    @MainActor
    private func load() async {
        isLoading = true
        error = nil
        do {
            snapshot = try await session.client.loadChart(symbol: symbol)
            annotations = snapshot?.annotations ?? []
            tradeEntryPrice = annotations.first(where: { $0.kind == "entry" })?.price ?? ""
            tradeStopPrice = annotations.first(where: { $0.kind == "stop" })?.price ?? ""
            tradeTargetPrice = annotations.first(where: { $0.kind == "target" })?.price ?? ""
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "CHART_LOAD_FAILED", message: caughtError.localizedDescription)
        }
        isLoading = false
    }
}
