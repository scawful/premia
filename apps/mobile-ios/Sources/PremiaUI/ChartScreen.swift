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
    @State private var draggingAnnotationID: String?
    @State private var draftQuantity = "1"
    @State private var draftInstruction: PremiaOrderInstruction = .buy
    @State private var draftLimitPrice = ""
    @State private var orderPreview: PremiaOrderPreview?
    @State private var orderSubmission: PremiaOrderSubmission?

    public init(symbol: String) {
        self.symbol = symbol
    }

    public var body: some View {
        AsyncStateView(isLoading: isLoading, error: error, retry: { Task { await load() } }) {
            VStack(alignment: .leading, spacing: 12) {
                Text("Primary Brokerage · Charles Schwab Market Data")
                    .font(.caption.weight(.semibold))
                    .foregroundStyle(.secondary)

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
                    .chartOverlay { proxy in
                        GeometryReader { geometry in
                            Rectangle()
                                .fill(.clear)
                                .contentShape(Rectangle())
                                .gesture(
                                    DragGesture(minimumDistance: 0)
                                        .onChanged { value in
                                            guard let plotFrame = proxy.plotFrame else { return }
                                            let frame = geometry[plotFrame]
                                            guard frame.contains(value.location) else { return }
                                            let localY = value.location.y - frame.origin.y
                                            guard let price: Double = proxy.value(atY: localY) else { return }

                                            if draggingAnnotationID == nil {
                                                draggingAnnotationID = nearestEditableAnnotationID(for: price)
                                            }
                                            guard let draggingAnnotationID else { return }
                                            updateAnnotation(annotationID: draggingAnnotationID, price: price)
                                        }
                                        .onEnded { _ in
                                            let annotationID = draggingAnnotationID
                                            draggingAnnotationID = nil
                                            if let annotationID {
                                                Task { await persistAnnotation(annotationID: annotationID) }
                                            }
                                        }
                                )
                        }
                    }
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

                VStack(alignment: .leading, spacing: 12) {
                    Text("Order Template")
                        .font(.headline)
                    Text("Turn saved entry, stop, and target levels into previewable order drafts for the selected account.")
                        .font(.caption)
                        .foregroundStyle(.secondary)

                    HStack {
                        Button("Use Entry Buy") {
                            applyTemplate(kind: "entry")
                        }
                        .buttonStyle(.bordered)
                        Button("Use Stop Sell") {
                            applyTemplate(kind: "stop")
                        }
                        .buttonStyle(.bordered)
                        Button("Use Target Sell") {
                            applyTemplate(kind: "target")
                        }
                        .buttonStyle(.bordered)
                    }

                    HStack {
                        TextField("Quantity", text: $draftQuantity)
                            .frame(width: 92)
                        TextField("Limit", text: $draftLimitPrice)
                            .frame(width: 92)
                        Picker("Side", selection: $draftInstruction) {
                            Text("Buy").tag(PremiaOrderInstruction.buy)
                            Text("Sell").tag(PremiaOrderInstruction.sell)
                        }
                        .pickerStyle(.menu)
                    }

                    HStack {
                        Button("Preview Draft") {
                            Task { await previewDraft() }
                        }
                        .buttonStyle(.borderedProminent)
                        .disabled(draftLimitPrice.isEmpty || draftQuantity.isEmpty)
                        Button("Stage Draft") {
                            Task { await submitDraft() }
                        }
                        .buttonStyle(.bordered)
                        .disabled(draftLimitPrice.isEmpty || draftQuantity.isEmpty)
                    }

                    if let orderPreview {
                        Text("Preview: \(orderPreview.instruction.rawValue) \(orderPreview.quantity) @ $\(orderPreview.limitPrice) · \(orderPreview.status)")
                            .font(.caption)
                    }
                    if let orderSubmission {
                        Text("Latest submission: \(orderSubmission.status) · \(orderSubmission.message)")
                            .font(.caption)
                            .foregroundStyle(.secondary)
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

    private func nearestEditableAnnotationID(for price: Double) -> String? {
        let editable = annotations.filter(isEditable)
        guard !editable.isEmpty else { return nil }
        return editable.min(by: { abs((Double($0.price) ?? 0) - price) < abs((Double($1.price) ?? 0) - price) })?.id
    }

    private func updateAnnotation(annotationID: String, price: Double) {
        let formattedPrice = String(format: "%.2f", price)
        annotations = annotations.map { annotation in
            guard annotation.id == annotationID else { return annotation }
            return PremiaChartAnnotation(
                id: annotation.id,
                label: annotation.label,
                price: formattedPrice,
                kind: annotation.kind
            )
        }
        if let entry = annotations.first(where: { $0.kind == "entry" })?.price {
            tradeEntryPrice = entry
        }
        if let stop = annotations.first(where: { $0.kind == "stop" })?.price {
            tradeStopPrice = stop
        }
        if let target = annotations.first(where: { $0.kind == "target" })?.price {
            tradeTargetPrice = target
        }
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
        Task { await persistAnnotation(annotationID: annotations.last?.id ?? "") }
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
        Task { await persistAnnotation(annotationID: annotationID) }
    }

    private func remove(annotationID: String) {
        annotations.removeAll { $0.id == annotationID }
        Task { await deleteAnnotation(annotationID: annotationID) }
    }

    private func applyRiskBox() {
        upsertRiskMarker(kind: "entry", label: "Entry", price: tradeEntryPrice)
        if !tradeStopPrice.isEmpty {
            upsertRiskMarker(kind: "stop", label: "Stop", price: tradeStopPrice)
        }
        if !tradeTargetPrice.isEmpty {
            upsertRiskMarker(kind: "target", label: "Target", price: tradeTargetPrice)
        }
        Task { await persistRiskBox() }
    }

    private func clearRiskBox() {
        annotations.removeAll { ["entry", "stop", "target"].contains($0.kind) }
        tradeEntryPrice = ""
        tradeStopPrice = ""
        tradeTargetPrice = ""
        Task { await clearPersistedRiskBox() }
    }

    private func upsertRiskMarker(kind: String, label: String, price: String) {
        if let index = annotations.firstIndex(where: { $0.kind == kind }) {
            annotations[index] = PremiaChartAnnotation(id: annotations[index].id, label: label, price: price, kind: kind)
        } else {
            annotations.append(PremiaChartAnnotation(id: "mobile-\(kind)", label: label, price: price, kind: kind))
        }
    }

    private func applyTemplate(kind: String) {
        guard let annotation = annotations.first(where: { $0.kind == kind }) else { return }
        draftLimitPrice = annotation.price
        switch kind {
        case "entry":
            draftInstruction = .buy
        case "stop", "target":
            draftInstruction = .sell
        default:
            break
        }
    }

    @MainActor
    private func previewDraft() async {
        do {
            orderPreview = try await session.client.previewOrder(
                PremiaOrderIntent(
                    accountID: session.selectedAccountID,
                    symbol: symbol,
                    assetType: .equity,
                    instruction: draftInstruction,
                    quantity: draftQuantity,
                    orderType: .limit,
                    limitPrice: draftLimitPrice,
                    duration: "DAY",
                    session: "NORMAL",
                    confirmLive: false
                )
            )
            orderSubmission = nil
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "CHART_TEMPLATE_PREVIEW_FAILED", message: caughtError.localizedDescription)
        }
    }

    @MainActor
    private func submitDraft() async {
        do {
            orderSubmission = try await session.client.submitOrder(
                PremiaOrderIntent(
                    accountID: session.selectedAccountID,
                    symbol: symbol,
                    assetType: .equity,
                    instruction: draftInstruction,
                    quantity: draftQuantity,
                    orderType: .limit,
                    limitPrice: draftLimitPrice,
                    duration: "DAY",
                    session: "NORMAL",
                    confirmLive: false
                )
            )
            orderPreview = nil
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "CHART_TEMPLATE_SUBMIT_FAILED", message: caughtError.localizedDescription)
        }
    }

    @MainActor
    private func persistAnnotation(annotationID: String) async {
        guard let annotation = annotations.first(where: { $0.id == annotationID }),
              isEditable(annotation) else { return }
        do {
            let updated = try await session.client.upsertChartAnnotation(symbol: symbol, accountID: session.selectedAccountID, annotation: annotation)
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
    private func deleteAnnotation(annotationID: String) async {
        do {
            let updated = try await session.client.deleteChartAnnotation(symbol: symbol, accountID: session.selectedAccountID, annotationID: annotationID)
            snapshot = updated
            annotations = updated.annotations
            tradeEntryPrice = annotations.first(where: { $0.kind == "entry" })?.price ?? ""
            tradeStopPrice = annotations.first(where: { $0.kind == "stop" })?.price ?? ""
            tradeTargetPrice = annotations.first(where: { $0.kind == "target" })?.price ?? ""
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "CHART_ANNOTATION_DELETE_FAILED", message: caughtError.localizedDescription)
        }
    }

    @MainActor
    private func persistRiskBox() async {
        do {
            for kind in ["entry", "stop", "target"] {
                guard let annotation = annotations.first(where: { $0.kind == kind }) else { continue }
                let updated = try await session.client.upsertChartAnnotation(symbol: symbol, accountID: session.selectedAccountID, annotation: annotation)
                snapshot = updated
                annotations = updated.annotations
            }
            tradeEntryPrice = annotations.first(where: { $0.kind == "entry" })?.price ?? ""
            tradeStopPrice = annotations.first(where: { $0.kind == "stop" })?.price ?? ""
            tradeTargetPrice = annotations.first(where: { $0.kind == "target" })?.price ?? ""
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "CHART_RISKBOX_SAVE_FAILED", message: caughtError.localizedDescription)
        }
    }

    @MainActor
    private func clearPersistedRiskBox() async {
        do {
            for kind in ["entry", "stop", "target"] {
                if let annotation = snapshot?.annotations.first(where: { $0.kind == kind }) {
                    let updated = try await session.client.deleteChartAnnotation(symbol: symbol, accountID: session.selectedAccountID, annotationID: annotation.id)
                    snapshot = updated
                    annotations = updated.annotations
                }
            }
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "CHART_RISKBOX_DELETE_FAILED", message: caughtError.localizedDescription)
        }
    }

    @MainActor
    private func load() async {
        isLoading = true
        error = nil
        do {
            try await session.refreshAccountContext()
            snapshot = try await session.client.loadChart(symbol: symbol, accountID: session.selectedAccountID)
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
