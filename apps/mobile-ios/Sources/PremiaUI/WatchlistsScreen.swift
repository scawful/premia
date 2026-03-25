import SwiftUI
import PremiaModels

public struct WatchlistsScreen: View {
    @EnvironmentObject private var session: PremiaAppSession
    @State private var watchlists: [PremiaWatchlistSummaryModel] = []
    @State private var isLoading = false
    @State private var error: PremiaAPIClientError?
    @State private var isCreatingWatchlist = false
    @State private var newWatchlistName = ""

    public init() {}

    public var body: some View {
        AsyncStateView(isLoading: isLoading, error: error, retry: { Task { await load() } }) {
            VStack(alignment: .leading, spacing: 16) {
                Text("Primary Brokerage · Charles Schwab")
                    .font(.caption.weight(.semibold))
                    .foregroundStyle(.secondary)

                List {
                    if !activeWatchlists.isEmpty {
                        Section("Active") {
                            ForEach(activeWatchlists) { watchlist in
                                watchlistRow(watchlist)
                            }
                        }
                    }

                    if !archivedWatchlists.isEmpty {
                        Section("Archived") {
                            ForEach(archivedWatchlists) { watchlist in
                                watchlistRow(watchlist)
                            }
                        }
                    }
                }
                .listStyle(.inset)
            }
            .padding(.horizontal)
        }
        .navigationTitle("Watchlists")
        .toolbar {
            ToolbarItem(placement: .automatic) {
                Button("New") {
                    isCreatingWatchlist = true
                }
            }
        }
        .sheet(isPresented: $isCreatingWatchlist) {
            NavigationStack {
                Form {
                    TextField("Watchlist Name", text: $newWatchlistName)
                }
                .navigationTitle("New Watchlist")
                .toolbar {
                    ToolbarItem(placement: .cancellationAction) {
                        Button("Cancel") {
                            isCreatingWatchlist = false
                        }
                    }
                    ToolbarItem(placement: .confirmationAction) {
                        Button("Create") {
                            Task { await createWatchlist() }
                        }
                        .disabled(newWatchlistName.isEmpty)
                    }
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
            watchlists = try await session.client.loadWatchlists()
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "WATCHLISTS_LOAD_FAILED", message: caughtError.localizedDescription)
        }
        isLoading = false
    }

    private var activeWatchlists: [PremiaWatchlistSummaryModel] {
        watchlists.filter { !$0.isArchived }
    }

    private var archivedWatchlists: [PremiaWatchlistSummaryModel] {
        watchlists.filter(\.isArchived)
    }

    private func watchlistRow(_ watchlist: PremiaWatchlistSummaryModel) -> some View {
        NavigationLink(value: PremiaRoute.watchlist(id: watchlist.id)) {
            VStack(alignment: .leading) {
                HStack {
                    Text(watchlist.name)
                    if watchlist.isArchived {
                        Text("Archived")
                            .font(.caption2.weight(.semibold))
                            .padding(.horizontal, 6)
                            .padding(.vertical, 2)
                            .background(Color.secondary.opacity(0.15), in: Capsule())
                    }
                }
                Text("\(watchlist.instrumentCount) instruments")
                    .font(.caption)
                    .foregroundStyle(.secondary)
            }
        }
        .swipeActions(edge: .trailing, allowsFullSwipe: false) {
            Button(watchlist.isArchived ? "Restore" : "Archive") {
                Task { await archive(watchlist: watchlist, archived: !watchlist.isArchived) }
            }
            .tint(watchlist.isArchived ? .green : .orange)

            if watchlist.isArchived {
                Button("Delete", role: .destructive) {
                    Task { await delete(watchlist: watchlist) }
                }
            }
        }
    }

    @MainActor
    private func createWatchlist() async {
        do {
            _ = try await session.client.createWatchlist(name: newWatchlistName)
            newWatchlistName = ""
            isCreatingWatchlist = false
            await load()
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "WATCHLIST_CREATE_FAILED", message: caughtError.localizedDescription)
        }
    }

    @MainActor
    private func archive(watchlist: PremiaWatchlistSummaryModel, archived: Bool) async {
        do {
            _ = try await session.client.archiveWatchlist(id: watchlist.id, archived: archived)
            await load()
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "WATCHLIST_ARCHIVE_FAILED", message: caughtError.localizedDescription)
        }
    }

    @MainActor
    private func delete(watchlist: PremiaWatchlistSummaryModel) async {
        do {
            _ = try await session.client.deleteWatchlist(id: watchlist.id)
            await load()
        } catch let clientError as PremiaAPIClientError {
            error = clientError
        } catch let caughtError {
            error = PremiaAPIClientError(code: "WATCHLIST_DELETE_FAILED", message: caughtError.localizedDescription)
        }
    }
}
