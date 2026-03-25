import SwiftUI
import PremiaModels

public struct WatchlistDetailScreen: View {
    @EnvironmentObject private var session: PremiaAppSession
    private let watchlistID: String
    @State private var snapshot: PremiaWatchlistScreenSnapshot?
    @State private var isLoading = false
    @State private var error: PremiaAPIClientError?
    @State private var addSymbol = ""
    @State private var selectedTransferWatchlistID = ""

    public init(watchlistID: String) {
        self.watchlistID = watchlistID
    }

    public var body: some View {
        AsyncStateView(isLoading: isLoading, error: error, retry: { Task { await load() } }) {
            VStack(spacing: 16) {
                if let snapshot {
                    Form {
                        if !session.brokerageAccounts.isEmpty {
                            Section("Account Context") {
                                Picker("Selected Account", selection: Binding(
                                    get: { session.selectedAccountID ?? session.brokerageAccounts.first?.id ?? "" },
                                    set: { session.selectedAccountID = $0 }
                                )) {
                                    ForEach(session.brokerageAccounts) { account in
                                        Text(account.displayName).tag(account.id)
                                    }
                                }
                            }
                        }

                        Section("Manage Symbols") {
                            HStack {
                                TextField("Add Symbol", text: $addSymbol)
                                Button("Add") {
                                    Task { await addSelectedSymbol() }
                                }
                                .disabled(addSymbol.isEmpty)
                            }
                            if !snapshot.availableWatchlists.filter({ $0.id != snapshot.watchlist.id && !$0.isArchived }).isEmpty {
                                Picker("Transfer To", selection: $selectedTransferWatchlistID) {
                                    ForEach(snapshot.availableWatchlists.filter { $0.id != snapshot.watchlist.id && !$0.isArchived }) { watchlist in
                                        Text(watchlist.name).tag(watchlist.id)
                                    }
                                }
                            }
                        }

                        Section("Symbols") {
                            ForEach(snapshot.rows) { row in
                                NavigationLink(value: PremiaRoute.quote(symbol: row.symbol)) {
                                    HStack {
                                        VStack(alignment: .leading) {
                                            HStack {
                                                Text(row.symbol)
                                                if row.isPinned {
                                                    Image(systemName: "pin.fill")
                                                        .font(.caption2)
                                                        .foregroundStyle(.yellow)
                                                }
                                            }
                                            Text(row.name)
                                                .font(.caption)
                                                .foregroundStyle(.secondary)
                                        }
                                        Spacer()
                                        VStack(alignment: .trailing) {
                                            Text("$\(row.lastPrice.amount)")
                                            Text("\(row.dayChange.absolute.amount) (\(row.dayChange.percent)%)")
                                                .font(.caption)
                                                .foregroundStyle(.secondary)
                                        }
                                    }
                                }
                                .swipeActions(edge: .leading, allowsFullSwipe: false) {
                                    Button(row.isPinned ? "Unpin" : "Pin") {
                                        Task { await pin(row: row, pinned: !row.isPinned) }
                                    }
                                    .tint(.yellow)
                                }
                                .swipeActions(edge: .trailing, allowsFullSwipe: false) {
                                    if !selectedTransferWatchlistID.isEmpty {
                                        Button("Transfer") {
                                            Task { await transfer(row: row, to: selectedTransferWatchlistID) }
                                        }
                                        .tint(.blue)
                                    }
                                    Button("Remove", role: .destructive) {
                                        Task { await remove(row: row) }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        .navigationTitle(snapshot?.watchlist.name ?? watchlistID)
        .toolbar {
            if snapshot?.watchlist.isArchived == true {
                ToolbarItem(placement: .automatic) {
                    Text("Archived")
                        .font(.caption.weight(.semibold))
                        .foregroundStyle(.secondary)
                }
            }
        }
        .task { await load() }
    }

    @MainActor
    private func load() async {
        isLoading = true
        error = nil
        do {
            try await session.refreshAccountContext()
            snapshot = try await session.client.loadWatchlist(id: watchlistID)
            if selectedTransferWatchlistID.isEmpty {
                selectedTransferWatchlistID = snapshot?.availableWatchlists.first(where: { $0.id != watchlistID && !$0.isArchived })?.id ?? ""
            }
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "WATCHLIST_LOAD_FAILED", message: caughtError.localizedDescription)
        }
        isLoading = false
    }

    @MainActor
    private func addSelectedSymbol() async {
        do {
            _ = try await session.client.addSymbol(addSymbol, toWatchlist: watchlistID)
            addSymbol = ""
            await load()
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "WATCHLIST_ADD_FAILED", message: caughtError.localizedDescription)
        }
    }

    @MainActor
    private func remove(row: PremiaWatchlistRowModel) async {
        do {
            _ = try await session.client.removeSymbol(row.symbol, fromWatchlist: watchlistID)
            await load()
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "WATCHLIST_REMOVE_FAILED", message: caughtError.localizedDescription)
        }
    }

    @MainActor
    private func pin(row: PremiaWatchlistRowModel, pinned: Bool) async {
        do {
            _ = try await session.client.pinSymbol(row.symbol, inWatchlist: watchlistID, pinned: pinned)
            await load()
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "WATCHLIST_PIN_FAILED", message: caughtError.localizedDescription)
        }
    }

    @MainActor
    private func transfer(row: PremiaWatchlistRowModel, to destinationID: String) async {
        do {
            _ = try await session.client.transferSymbol(row.symbol, fromWatchlist: watchlistID, toWatchlist: destinationID)
            await load()
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "WATCHLIST_TRANSFER_FAILED", message: caughtError.localizedDescription)
        }
    }
}
