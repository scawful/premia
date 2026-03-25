import Foundation
import PremiaModels
import PremiaAPIClientGenerated

private struct BootstrapEnvelopeDTO: Decodable {
    struct Meta: Decodable {
        let requestId: String
        let asOf: Date?
    }

    struct Payload: Decodable {
        struct Session: Decodable {
            let environment: String
            let featureFlags: [String: Bool]
        }

        struct Connection: Decodable {
            let provider: String
            let status: String
            let displayName: String
            let lastSyncAt: Date?
            let reauthRequired: Bool
            let capabilities: [String: Bool]
        }

        let session: Session
        let connections: [Connection]
    }

    let data: Payload
    let meta: Meta
}

public struct PremiaAPIConfiguration: Sendable, Equatable {
    public let baseURL: URL

    public init(baseURL: URL) {
        self.baseURL = baseURL
    }
}

public final class PremiaAPIClient: @unchecked Sendable {
    public let configuration: PremiaAPIConfiguration
    private let apiConfiguration: PremiaAPIClientGeneratedAPIConfiguration

    public init(configuration: PremiaAPIConfiguration) {
        self.configuration = configuration
        self.apiConfiguration = PremiaAPIClientGeneratedAPIConfiguration.shared
        self.apiConfiguration.basePath = configuration.baseURL.absoluteString
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func performSafely<T: Sendable>(
        _ operation: @escaping @Sendable () async throws -> T
    ) async -> Result<T, PremiaAPIClientError> {
        do {
            return .success(try await operation())
        } catch let error as PremiaAPIClientError {
            return .failure(error)
        } catch {
            return .failure(mapError(error))
        }
    }

    public func bootstrapURL() -> URL {
        configuration.baseURL.appending(path: "v1/bootstrap")
    }

    public func homeURL() -> URL {
        configuration.baseURL.appending(path: "v1/screens/home")
    }

    public func quoteURL(symbol: String) -> URL {
        configuration.baseURL.appending(path: "v1/screens/quotes").appending(path: symbol)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func loadBootstrap() async throws -> PremiaBootstrapSnapshot {
        do {
            let response = try await executeMapped {
                try await PremiaAPIClientGeneratedAPI.BootstrapAPI.getBootstrap(apiConfiguration: apiConfiguration)
            }
            let connections = response.data.connections.map(mapConnection)
            return PremiaBootstrapSnapshot(
                environment: response.data.session.environment,
                featureFlags: response.data.session.featureFlags,
                connections: connections,
                asOf: response.meta.asOf
            )
        } catch {
            return try await loadBootstrapFallback()
        }
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func loadHome() async throws -> PremiaHomeSnapshot {
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.HomeAPI.getHomeScreen(apiConfiguration: apiConfiguration)
        }
        return PremiaHomeSnapshot(
            connections: response.data.connections.map(mapConnection),
            portfolio: mapPortfolio(response.data.portfolio),
            topHoldings: response.data.topHoldings.map(mapHolding),
            watchlists: response.data.watchlists.map(mapWatchlist),
            market: PremiaMarketSession(
                session: response.data.market.session.rawValue,
                nextTransitionAt: response.data.market.nextTransitionAt
            ),
            asOf: response.meta.asOf
        )
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func loadAccount() async throws -> PremiaAccountDetailSnapshot {
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.AccountAPI.getAccountScreen(apiConfiguration: apiConfiguration)
        }
        return PremiaAccountDetailSnapshot(
            accountID: response.data.accountId,
            cash: mapMoney(response.data.cash),
            netLiquidation: mapMoney(response.data.netLiquidation),
            availableFunds: mapMoney(response.data.availableFunds),
            longMarketValue: mapMoney(response.data.longMarketValue),
            shortMarketValue: mapMoney(response.data.shortMarketValue),
            buyingPower: mapMoney(response.data.buyingPower),
            equity: mapMoney(response.data.equity),
            equityPercentage: response.data.equityPercentage,
            marginBalance: mapMoney(response.data.marginBalance),
            positions: response.data.positions.map(mapAccountPosition),
            asOf: response.meta.asOf
        )
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func loadQuote(symbol: String) async throws -> PremiaQuoteScreenSnapshot {
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.QuotesAPI.getQuoteScreen(symbol: symbol, apiConfiguration: apiConfiguration)
        }
        return PremiaQuoteScreenSnapshot(
            instrument: mapInstrument(response.data.instrument),
            quote: mapQuoteSnapshot(response.data.quote),
            position: mapPosition(response.data.position),
            watchlistIds: response.data.watchlistMembership.watchlistIds,
            asOf: response.meta.asOf
        )
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func loadChart(
        symbol: String,
        accountID: String? = nil,
        range: PremiaChartRange = .oneMonth,
        interval: PremiaChartInterval = .oneDay,
        extendedHours: Bool = false
    ) async throws -> PremiaChartSnapshot {
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.ChartsAPI.getChartScreen(
                symbol: symbol,
                accountId: accountID,
                range: mapChartRange(range),
                interval: mapChartInterval(interval),
                extendedHours: extendedHours,
                apiConfiguration: apiConfiguration
            )
        }

        return PremiaChartSnapshot(
            instrument: mapInstrument(response.data.instrument),
            range: response.data.range,
            interval: response.data.interval,
            timezone: response.data.timezone,
            seriesType: response.data.series.type.rawValue,
            candles: response.data.series.bars.map(mapCandle),
            annotations: response.data.annotations.map(mapChartAnnotation),
            change: response.data.stats?.change.map(mapChange),
            asOf: response.meta.asOf
        )
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func replaceChartAnnotations(symbol: String, accountID: String? = nil, annotations: [PremiaChartAnnotation]) async throws -> PremiaChartSnapshot {
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.ChartsAPI.replaceChartAnnotations(
                symbol: symbol,
                replaceChartAnnotationsRequest: PremiaAPIClientGeneratedAPI.ReplaceChartAnnotationsRequest(
                    accountId: accountID,
                    annotations: annotations.map {
                        PremiaAPIClientGeneratedAPI.ChartAnnotation(
                            id: $0.id,
                            label: $0.label,
                            price: $0.price,
                            kind: $0.kind
                        )
                    }
                ),
                apiConfiguration: apiConfiguration
            )
        }

        return PremiaChartSnapshot(
            instrument: mapInstrument(response.data.instrument),
            range: response.data.range,
            interval: response.data.interval,
            timezone: response.data.timezone,
            seriesType: response.data.series.type.rawValue,
            candles: response.data.series.bars.map(mapCandle),
            annotations: response.data.annotations.map(mapChartAnnotation),
            change: response.data.stats?.change.map(mapChange),
            asOf: response.meta.asOf
        )
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func loadOptionChain(
        symbol: String,
        strikeCount: String = "8",
        strategy: String = "SINGLE",
        range: String = "ALL",
        expMonth: String = "ALL",
        optionType: String = "ALL"
    ) async throws -> PremiaOptionChainSnapshotModel {
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.OptionsAPI.getOptionChainScreen(
                symbol: symbol,
                strikeCount: strikeCount,
                strategy: strategy,
                range: range,
                expMonth: expMonth,
                optionType: optionType,
                apiConfiguration: apiConfiguration
            )
        }

        return PremiaOptionChainSnapshotModel(
            symbol: response.data.symbol,
            description: response.data.description,
            bid: response.data.bid,
            ask: response.data.ask,
            openPrice: response.data.openPrice,
            closePrice: response.data.closePrice,
            highPrice: response.data.highPrice,
            lowPrice: response.data.lowPrice,
            totalVolume: response.data.totalVolume,
            volatility: response.data.volatility,
            gammaExposure: response.data.gammaExposure,
            expirations: response.data.expirations.map(mapOptionExpiration),
            asOf: response.meta.asOf
        )
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func loadOptionChain(query: PremiaOptionChainQuery) async throws -> PremiaOptionChainSnapshotModel {
        try await loadOptionChain(
            symbol: query.symbol,
            strikeCount: query.strikeCount,
            strategy: query.strategy,
            range: query.range,
            expMonth: query.expMonth,
            optionType: query.optionType
        )
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func loadWatchlists() async throws -> [PremiaWatchlistSummaryModel] {
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.WatchlistsAPI.listWatchlists(apiConfiguration: apiConfiguration)
        }
        return response.data.watchlists.map(mapWatchlist)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func loadWatchlist(id: String) async throws -> PremiaWatchlistScreenSnapshot {
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.WatchlistsAPI.getWatchlistScreen(
                watchlistId: id,
                apiConfiguration: apiConfiguration
            )
        }
        return PremiaWatchlistScreenSnapshot(
            watchlist: mapWatchlist(response.data.watchlist),
            availableWatchlists: response.data.availableWatchlists.map(mapWatchlist),
            rows: response.data.rows.map(mapWatchlistRow),
            asOf: response.meta.asOf
        )
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func startSchwabOAuth(
        redirectURI: String? = nil,
        clientPlatform: String? = nil
    ) async throws -> PremiaSchwabOAuthLaunch {
        let request = PremiaAPIClientGeneratedAPI.StartSchwabOAuthRequest(
            redirectUri: redirectURI,
            clientPlatform: clientPlatform
        )
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.ConnectionsAPI.startSchwabOAuth(
                startSchwabOAuthRequest: request,
                apiConfiguration: apiConfiguration
            )
        }
        return PremiaSchwabOAuthLaunch(
            authURL: response.data.authUrl,
            state: response.data.state,
            expiresAt: response.data.expiresAt
        )
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func completeSchwabOAuth(
        callback: String,
        state: String? = nil
    ) async throws -> PremiaConnectionSummary {
        let request = PremiaAPIClientGeneratedAPI.CompleteSchwabOAuthRequest(
            callback: callback,
            state: state
        )
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.ConnectionsAPI.completeSchwabOAuth(
                completeSchwabOAuthRequest: request,
                apiConfiguration: apiConfiguration
            )
        }
        return mapConnection(response.data)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func createPlaidLinkToken(
        userID: String? = nil,
        redirectURI: String? = nil
    ) async throws -> PremiaPlaidLinkLaunch {
        let request = PremiaAPIClientGeneratedAPI.CreatePlaidLinkTokenRequest(
            userId: userID,
            redirectUri: redirectURI
        )
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.ConnectionsAPI.createPlaidLinkToken(
                createPlaidLinkTokenRequest: request,
                apiConfiguration: apiConfiguration
            )
        }
        return PremiaPlaidLinkLaunch(
            linkToken: response.data.linkToken,
            expiration: response.data.expiration
        )
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func completePlaidLink(
        publicToken: String,
        institutionID: String? = nil
    ) async throws -> PremiaConnectionSummary {
        let request = PremiaAPIClientGeneratedAPI.CompletePlaidLinkRequest(
            publicToken: publicToken,
            institutionId: institutionID
        )
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.ConnectionsAPI.completePlaidLink(
                completePlaidLinkRequest: request,
                apiConfiguration: apiConfiguration
            )
        }
        return mapConnection(response.data)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func createWatchlist(name: String) async throws -> PremiaWatchlistSummaryModel {
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.WatchlistsAPI.createWatchlist(
                createWatchlistRequest: PremiaAPIClientGeneratedAPI.CreateWatchlistRequest(name: name),
                apiConfiguration: apiConfiguration
            )
        }
        return mapWatchlist(response.data)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func renameWatchlist(id: String, name: String) async throws -> PremiaWatchlistSummaryModel {
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.WatchlistsAPI.updateWatchlist(
                watchlistId: id,
                updateWatchlistRequest: PremiaAPIClientGeneratedAPI.UpdateWatchlistRequest(name: name),
                apiConfiguration: apiConfiguration
            )
        }
        return mapWatchlist(response.data)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func addSymbol(_ symbol: String, toWatchlist id: String) async throws -> PremiaWatchlistSummaryModel {
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.WatchlistsAPI.addWatchlistSymbol(
                watchlistId: id,
                addWatchlistSymbolRequest: PremiaAPIClientGeneratedAPI.AddWatchlistSymbolRequest(symbol: symbol),
                apiConfiguration: apiConfiguration
            )
        }
        return mapWatchlist(response.data)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func removeSymbol(_ symbol: String, fromWatchlist id: String) async throws -> PremiaWatchlistSummaryModel {
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.WatchlistsAPI.deleteWatchlistSymbol(
                watchlistId: id,
                symbol: symbol,
                apiConfiguration: apiConfiguration
            )
        }
        return mapWatchlist(response.data)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func pinSymbol(_ symbol: String, inWatchlist id: String, pinned: Bool) async throws -> PremiaWatchlistSummaryModel {
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.WatchlistsAPI.pinWatchlistSymbol(
                watchlistId: id,
                symbol: symbol,
                pinWatchlistSymbolRequest: PremiaAPIClientGeneratedAPI.PinWatchlistSymbolRequest(pinned: pinned),
                apiConfiguration: apiConfiguration
            )
        }
        return mapWatchlist(response.data)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func archiveWatchlist(id: String, archived: Bool) async throws -> PremiaWatchlistSummaryModel {
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.WatchlistsAPI.archiveWatchlist(
                watchlistId: id,
                archiveWatchlistRequest: PremiaAPIClientGeneratedAPI.ArchiveWatchlistRequest(archived: archived),
                apiConfiguration: apiConfiguration
            )
        }
        return mapWatchlist(response.data)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func deleteWatchlist(id: String) async throws -> PremiaWatchlistSummaryModel {
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.WatchlistsAPI.deleteWatchlist(
                watchlistId: id,
                apiConfiguration: apiConfiguration
            )
        }
        return mapWatchlist(response.data)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func transferSymbol(_ symbol: String, fromWatchlist sourceID: String, toWatchlist destinationID: String) async throws -> PremiaWatchlistSummaryModel {
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.WatchlistsAPI.transferWatchlistSymbol(
                watchlistId: sourceID,
                transferWatchlistSymbolRequest: PremiaAPIClientGeneratedAPI.TransferWatchlistSymbolRequest(symbol: symbol, destinationWatchlistId: destinationID),
                apiConfiguration: apiConfiguration
            )
        }
        return mapWatchlist(response.data)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func previewOrder(_ intent: PremiaOrderIntent) async throws -> PremiaOrderPreview {
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.TradingAPI.previewOrder(
                orderIntentRequest: mapOrderIntent(intent),
                apiConfiguration: apiConfiguration
            )
        }
        return mapOrderPreview(response.data, asOf: response.meta.asOf)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func submitOrder(_ intent: PremiaOrderIntent) async throws -> PremiaOrderSubmission {
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.TradingAPI.submitOrder(
                orderIntentRequest: mapOrderIntent(intent),
                apiConfiguration: apiConfiguration
            )
        }
        return mapOrderSubmission(response.data, asOf: response.meta.asOf)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func cancelOrder(orderID: String, accountID: String? = nil, confirmLive: Bool = false) async throws -> PremiaOrderCancellation {
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.TradingAPI.cancelOrder(
                orderId: orderID,
                orderCancelRequest: PremiaAPIClientGeneratedAPI.OrderCancelRequest(
                    accountId: accountID,
                    confirmLive: confirmLive
                ),
                apiConfiguration: apiConfiguration
            )
        }
        return mapOrderCancellation(response.data, asOf: response.meta.asOf)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func replaceOrder(_ replacement: PremiaOrderReplaceIntent) async throws -> PremiaOrderReplacement {
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.TradingAPI.replaceOrder(
                orderId: replacement.orderID,
                orderReplaceRequest: mapOrderReplaceIntent(replacement),
                apiConfiguration: apiConfiguration
            )
        }
        return mapOrderReplacement(response.data, asOf: response.meta.asOf)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func loadOpenOrders(accountID: String? = nil) async throws -> [PremiaOrderRecord] {
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.TradingAPI.getOpenOrders(
                accountId: accountID,
                apiConfiguration: apiConfiguration
            )
        }
        return response.data.orders.map(mapOrderRecord)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func loadOrderHistory(accountID: String? = nil) async throws -> [PremiaOrderRecord] {
        let response = try await executeMapped {
            try await PremiaAPIClientGeneratedAPI.TradingAPI.getOrderHistory(
                accountId: accountID,
                apiConfiguration: apiConfiguration
            )
        }
        return response.data.orders.map(mapOrderRecord)
    }


    @available(macOS 10.15, iOS 13.0, *)
    private func executeMapped<T>(_ operation: () async throws -> T) async throws -> T {
        do {
            return try await operation()
        } catch let error as PremiaAPIClientError {
            throw error
        } catch {
            throw mapError(error)
        }
    }

    @available(macOS 10.15, iOS 13.0, *)
    private func loadBootstrapFallback() async throws -> PremiaBootstrapSnapshot {
        let (data, response) = try await URLSession.shared.data(from: bootstrapURL())
        if let http = response as? HTTPURLResponse, !(200...299).contains(http.statusCode) {
            throw PremiaAPIClientError(
                code: "HTTP_\(http.statusCode)",
                message: "Bootstrap request failed.",
                statusCode: http.statusCode,
                retryable: http.statusCode >= 500,
                action: http.statusCode == 401 ? .reauth : .retry
            )
        }

        let decoder = JSONDecoder()
        decoder.dateDecodingStrategy = .iso8601
        let envelope = try decoder.decode(BootstrapEnvelopeDTO.self, from: data)
        return PremiaBootstrapSnapshot(
            environment: envelope.data.session.environment,
            featureFlags: envelope.data.session.featureFlags,
            connections: envelope.data.connections.map { connection in
                PremiaConnectionSummary(
                    id: connection.provider,
                    provider: PremiaProvider(rawValue: connection.provider) ?? .internal,
                    status: PremiaConnectionStatus(rawValue: connection.status) ?? .notConnected,
                    displayName: connection.displayName,
                    lastSyncAt: connection.lastSyncAt,
                    reauthRequired: connection.reauthRequired,
                    capabilities: connection.capabilities
                )
            },
            asOf: envelope.meta.asOf
        )
    }

    private func mapError(_ error: Error) -> PremiaAPIClientError {
        if case let ErrorResponse.error(statusCode, data, _, _) = error,
           let data,
           let decoded = try? JSONDecoder().decode(PremiaAPIClientGeneratedAPI.ModelErrorResponse.self, from: data) {
            return PremiaAPIClientError(
                code: decoded.error.code,
                message: decoded.error.message,
                statusCode: statusCode,
                provider: decoded.error.provider.map(mapProvider),
                retryable: decoded.error.retryable,
                action: mapAction(decoded.error.action)
            )
        }

        if case let ErrorResponse.error(statusCode, _, _, underlying) = error {
            return PremiaAPIClientError(
                code: "HTTP_\(statusCode)",
                message: String(describing: underlying),
                statusCode: statusCode,
                retryable: statusCode >= 500,
                action: statusCode == 401 ? .reauth : .retry
            )
        }

        if error is CancellationError {
            return PremiaAPIClientError(
                code: "CANCELLED",
                message: "The request was cancelled.",
                retryable: true,
                action: .retry
            )
        }

        return PremiaAPIClientError(
            code: "CLIENT_ERROR",
            message: String(describing: error),
            retryable: false,
            action: .unknown
        )
    }

    private func mapConnection(_ connection: PremiaAPIClientGeneratedAPI.ConnectionSummary) -> PremiaConnectionSummary {
        PremiaConnectionSummary(
            id: connection.provider.rawValue,
            provider: mapProvider(connection.provider),
            status: mapStatus(connection.status),
            displayName: connection.displayName,
            lastSyncAt: connection.lastSyncAt,
            reauthRequired: connection.reauthRequired,
            capabilities: connection.capabilities
        )
    }

    private func mapMoney(_ money: PremiaAPIClientGeneratedAPI.Money) -> PremiaMoney {
        PremiaMoney(amount: money.amount, currency: money.currency)
    }

    private func mapChange(_ change: PremiaAPIClientGeneratedAPI.AbsolutePercentChange) -> PremiaAbsolutePercentChange {
        PremiaAbsolutePercentChange(absolute: mapMoney(change.absolute), percent: change.percent)
    }

    private func mapInstrument(_ instrument: PremiaAPIClientGeneratedAPI.Instrument) -> PremiaInstrument {
        PremiaInstrument(
            symbol: instrument.symbol,
            name: instrument.name,
            assetType: instrument.assetType.rawValue,
            primaryExchange: instrument.primaryExchange
        )
    }

    private func mapPosition(_ position: PremiaAPIClientGeneratedAPI.PositionSummary?) -> PremiaPositionSummary? {
        guard let position else {
            return nil
        }

        return PremiaPositionSummary(
            quantity: position.quantity,
            marketValue: position.marketValue.map(mapMoney),
            costBasis: position.costBasis.map(mapMoney),
            unrealizedGain: position.unrealizedGain.map(mapMoney)
        )
    }

    private func mapHolding(_ holding: PremiaAPIClientGeneratedAPI.HoldingRow) -> PremiaHoldingSummary {
        PremiaHoldingSummary(
            id: holding.id,
            symbol: holding.symbol,
            name: holding.name,
            quantity: holding.quantity,
            marketValue: mapMoney(holding.marketValue),
            dayChange: mapChange(holding.dayChange)
        )
    }

    private func mapAccountPosition(_ position: PremiaAPIClientGeneratedAPI.AccountPositionRow) -> PremiaAccountPositionModel {
        PremiaAccountPositionModel(
            symbol: position.symbol,
            name: position.name,
            dayProfitLoss: mapMoney(position.dayProfitLoss),
            dayProfitLossPercent: position.dayProfitLossPercent,
            averagePrice: mapMoney(position.averagePrice),
            marketValue: mapMoney(position.marketValue),
            quantity: position.quantity
        )
    }

    private func mapWatchlist(_ watchlist: PremiaAPIClientGeneratedAPI.WatchlistSummary) -> PremiaWatchlistSummaryModel {
        PremiaWatchlistSummaryModel(
            id: watchlist.id,
            name: watchlist.name,
            instrumentCount: watchlist.instrumentCount,
            isArchived: watchlist.isArchived
        )
    }

    private func mapWatchlistRow(_ row: PremiaAPIClientGeneratedAPI.WatchlistRow) -> PremiaWatchlistRowModel {
        PremiaWatchlistRowModel(
            id: row.id,
            symbol: row.symbol,
            name: row.name,
            lastPrice: mapMoney(row.lastPrice),
            dayChange: mapChange(row.dayChange),
            bid: row.bid.map(mapMoney),
            ask: row.ask.map(mapMoney),
            updatedAt: row.updatedAt,
            isPinned: row.isPinned
        )
    }

    private func mapChartAnnotation(_ annotation: PremiaAPIClientGeneratedAPI.ChartAnnotation) -> PremiaChartAnnotation {
        PremiaChartAnnotation(
            id: annotation.id,
            label: annotation.label,
            price: annotation.price,
            kind: annotation.kind
        )
    }

    private func mapOrderIntent(_ intent: PremiaOrderIntent) -> PremiaAPIClientGeneratedAPI.OrderIntentRequest {
        PremiaAPIClientGeneratedAPI.OrderIntentRequest(
            accountId: intent.accountID,
            symbol: intent.symbol,
            assetType: mapAssetType(intent.assetType),
            instruction: mapInstruction(intent.instruction),
            quantity: intent.quantity,
            orderType: mapOrderType(intent.orderType),
            limitPrice: intent.limitPrice,
            duration: intent.duration,
            session: intent.session,
            confirmLive: intent.confirmLive
        )
    }

    private func mapOrderReplaceIntent(_ replacement: PremiaOrderReplaceIntent) -> PremiaAPIClientGeneratedAPI.OrderReplaceRequest {
        let intent = replacement.replacement
        return PremiaAPIClientGeneratedAPI.OrderReplaceRequest(
            accountId: intent.accountID,
            symbol: intent.symbol,
            assetType: mapReplaceAssetType(intent.assetType),
            instruction: mapReplaceInstruction(intent.instruction),
            quantity: intent.quantity,
            orderType: mapReplaceOrderType(intent.orderType),
            limitPrice: intent.limitPrice,
            duration: intent.duration,
            session: intent.session,
            confirmLive: intent.confirmLive
        )
    }

    private func mapOrderPreview(_ preview: PremiaAPIClientGeneratedAPI.OrderPreviewData, asOf: Date?) -> PremiaOrderPreview {
        PremiaOrderPreview(
            previewID: preview.previewId,
            accountID: preview.accountId,
            symbol: preview.symbol,
            assetType: mapAssetTypeValue(preview.assetType),
            instruction: mapInstructionValue(preview.instruction),
            quantity: preview.quantity,
            orderType: mapOrderTypeValue(preview.orderType),
            limitPrice: preview.limitPrice,
            estimatedTotal: preview.estimatedTotal,
            mode: preview.mode,
            status: preview.status,
            warnings: preview.warnings,
            asOf: asOf
        )
    }

    private func mapOrderSubmission(_ submission: PremiaAPIClientGeneratedAPI.OrderSubmissionData, asOf: Date?) -> PremiaOrderSubmission {
        PremiaOrderSubmission(
            submissionID: submission.submissionId,
            accountID: submission.accountId,
            symbol: submission.symbol,
            assetType: mapAssetTypeValue(submission.assetType),
            instruction: mapInstructionValue(submission.instruction),
            quantity: submission.quantity,
            orderType: mapOrderTypeValue(submission.orderType),
            limitPrice: submission.limitPrice,
            mode: submission.mode,
            status: submission.status,
            submittedAt: submission.submittedAt,
            message: submission.message,
            asOf: asOf
        )
    }

    private func mapOrderCancellation(_ cancellation: PremiaAPIClientGeneratedAPI.OrderCancellationData, asOf: Date?) -> PremiaOrderCancellation {
        PremiaOrderCancellation(
            orderID: cancellation.orderId,
            accountID: cancellation.accountId,
            mode: cancellation.mode,
            status: cancellation.status,
            cancelledAt: cancellation.cancelledAt,
            message: cancellation.message,
            asOf: asOf
        )
    }

    private func mapOrderReplacement(_ replacement: PremiaAPIClientGeneratedAPI.OrderReplacementData, asOf: Date?) -> PremiaOrderReplacement {
        PremiaOrderReplacement(
            replacementID: replacement.replacementId,
            replacedOrderID: replacement.replacedOrderId,
            accountID: replacement.accountId,
            symbol: replacement.symbol,
            assetType: mapAssetTypeValue(replacement.assetType),
            instruction: mapInstructionValue(replacement.instruction),
            quantity: replacement.quantity,
            orderType: mapOrderTypeValue(replacement.orderType),
            limitPrice: replacement.limitPrice,
            mode: replacement.mode,
            status: replacement.status,
            submittedAt: replacement.submittedAt,
            message: replacement.message,
            asOf: asOf
        )
    }

    private func mapOrderRecord(_ record: PremiaAPIClientGeneratedAPI.OrderRecordData) -> PremiaOrderRecord {
        PremiaOrderRecord(
            id: record.orderId,
            accountID: record.accountId,
            symbol: record.symbol,
            assetType: mapAssetTypeValue(record.assetType),
            instruction: mapInstructionValue(record.instruction),
            quantity: record.quantity,
            orderType: mapOrderTypeValue(record.orderType),
            limitPrice: record.limitPrice,
            mode: record.mode,
            status: record.status,
            submittedAt: record.submittedAt,
            updatedAt: record.updatedAt,
            message: record.message
        )
    }

    private func mapPortfolio(_ portfolio: PremiaAPIClientGeneratedAPI.PortfolioSummary) -> PremiaPortfolioSummary {
        PremiaPortfolioSummary(
            totalValue: mapMoney(portfolio.totalValue),
            dayChange: mapChange(portfolio.dayChange),
            cash: mapMoney(portfolio.cash),
            buyingPower: mapMoney(portfolio.buyingPower),
            holdingsCount: portfolio.holdingsCount
        )
    }

    private func mapQuoteSnapshot(_ quote: PremiaAPIClientGeneratedAPI.QuoteSnapshot) -> PremiaQuoteSnapshotModel {
        PremiaQuoteSnapshotModel(
            lastPrice: mapMoney(quote.lastPrice),
            bid: quote.bid.map(mapMoney) ?? PremiaMoney(amount: "0.00", currency: quote.lastPrice.currency),
            ask: quote.ask.map(mapMoney) ?? PremiaMoney(amount: "0.00", currency: quote.lastPrice.currency),
            open: quote._open.map(mapMoney) ?? PremiaMoney(amount: "0.00", currency: quote.lastPrice.currency),
            high: quote.high.map(mapMoney) ?? PremiaMoney(amount: "0.00", currency: quote.lastPrice.currency),
            low: quote.low.map(mapMoney) ?? PremiaMoney(amount: "0.00", currency: quote.lastPrice.currency),
            previousClose: mapMoney(quote.previousClose),
            volume: quote.volume,
            updatedAt: quote.updatedAt
        )
    }

    private func mapCandle(_ candle: PremiaAPIClientGeneratedAPI.Candle) -> PremiaCandle {
        PremiaCandle(
            time: ISO8601DateFormatter().string(from: candle.time),
            open: candle._open,
            high: candle.high,
            low: candle.low,
            close: candle.close,
            volume: candle.volume
        )
    }

    private func mapOptionExpiration(_ expiration: PremiaAPIClientGeneratedAPI.OptionExpirationSnapshot) -> PremiaOptionExpirationSnapshotModel {
        PremiaOptionExpirationSnapshotModel(
            id: expiration.id,
            label: expiration.label,
            gammaAtExpiry: expiration.gammaAtExpiry,
            rows: expiration.rows.map(mapOptionRow)
        )
    }

    private func mapOptionRow(_ row: PremiaAPIClientGeneratedAPI.OptionContractPairRow) -> PremiaOptionContractPairRowModel {
        PremiaOptionContractPairRowModel(
            id: row.id,
            strike: row.strike,
            callBid: row.callBid,
            callAsk: row.callAsk,
            callLast: row.callLast,
            callChange: row.callChange,
            callDelta: row.callDelta,
            callGamma: row.callGamma,
            callTheta: row.callTheta,
            callVega: row.callVega,
            callOpenInterest: row.callOpenInterest,
            putBid: row.putBid,
            putAsk: row.putAsk,
            putLast: row.putLast,
            putChange: row.putChange,
            putDelta: row.putDelta,
            putGamma: row.putGamma,
            putTheta: row.putTheta,
            putVega: row.putVega,
            putOpenInterest: row.putOpenInterest
        )
    }

    private func mapChartRange(_ range: PremiaChartRange) -> PremiaAPIClientGeneratedAPI.ChartsAPI.ModelRange_getChartScreen {
        switch range {
        case .oneDay:
            return ._1d
        case .fiveDays:
            return ._5d
        case .oneMonth:
            return ._1m
        case .threeMonths:
            return ._3m
        case .sixMonths:
            return ._6m
        case .oneYear:
            return ._1y
        case .fiveYears:
            return ._5y
        case .max:
            return .max
        }
    }

    private func mapChartInterval(_ interval: PremiaChartInterval) -> PremiaAPIClientGeneratedAPI.ChartsAPI.Interval_getChartScreen {
        switch interval {
        case .oneMinute:
            return ._1m
        case .fiveMinutes:
            return ._5m
        case .fifteenMinutes:
            return ._15m
        case .oneHour:
            return ._1h
        case .oneDay:
            return ._1d
        case .oneWeek:
            return ._1w
        }
    }

    private func mapProvider(_ provider: PremiaAPIClientGeneratedAPI.Provider) -> PremiaProvider {
        switch provider {
        case .tda:
            return .tda
        case .schwab:
            return .schwab
        case .ibkr:
            return .ibkr
        case .plaid:
            return .plaid
        case ._internal, .unknownDefaultOpenApi:
            return .internal
        }
    }

    private func mapStatus(_ status: PremiaAPIClientGeneratedAPI.ConnectionStatus) -> PremiaConnectionStatus {
        switch status {
        case .connected:
            return .connected
        case .connecting:
            return .connecting
        case .notConnected:
            return .notConnected
        case .degraded:
            return .degraded
        case .reauthRequired, .unknownDefaultOpenApi:
            return .reauthRequired
        }
    }

    private func mapAction(_ action: PremiaAPIClientGeneratedAPI.ErrorDetail.Action?) -> PremiaAPIErrorAction {
        guard let action else {
            return .unknown
        }

        switch action {
        case ._none:
            return .none
        case .retry:
            return .retry
        case .reconnect:
            return .reconnect
        case .reauth:
            return .reauth
        case .relink:
            return .relink
        case .unknownDefaultOpenApi:
            return .unknown
        }
    }

    private func mapAssetType(_ assetType: PremiaAssetType) -> PremiaAPIClientGeneratedAPI.OrderIntentRequest.AssetType {
        switch assetType {
        case .equity:
            return .equity
        case .option:
            return .option
        }
    }

    private func mapInstruction(_ instruction: PremiaOrderInstruction) -> PremiaAPIClientGeneratedAPI.OrderIntentRequest.Instruction {
        switch instruction {
        case .buy:
            return .buy
        case .sell:
            return .sell
        case .buyToOpen:
            return .buyToOpen
        case .sellToClose:
            return .sellToClose
        case .buyToClose:
            return .buyToClose
        case .sellToOpen:
            return .sellToOpen
        }
    }

    private func mapOrderType(_ orderType: PremiaOrderType) -> PremiaAPIClientGeneratedAPI.OrderIntentRequest.OrderType {
        switch orderType {
        case .market:
            return .market
        case .limit:
            return .limit
        }
    }

    private func mapAssetTypeValue(_ value: String) -> PremiaAssetType {
        PremiaAssetType(rawValue: value) ?? .equity
    }

    private func mapReplaceAssetType(_ assetType: PremiaAssetType) -> PremiaAPIClientGeneratedAPI.OrderReplaceRequest.AssetType {
        switch assetType {
        case .equity:
            return .equity
        case .option:
            return .option
        }
    }

    private func mapInstructionValue(_ value: String) -> PremiaOrderInstruction {
        PremiaOrderInstruction(rawValue: value) ?? .buy
    }

    private func mapReplaceInstruction(_ instruction: PremiaOrderInstruction) -> PremiaAPIClientGeneratedAPI.OrderReplaceRequest.Instruction {
        switch instruction {
        case .buy:
            return .buy
        case .sell:
            return .sell
        case .buyToOpen:
            return .buyToOpen
        case .sellToClose:
            return .sellToClose
        case .buyToClose:
            return .buyToClose
        case .sellToOpen:
            return .sellToOpen
        }
    }

    private func mapOrderTypeValue(_ value: String) -> PremiaOrderType {
        PremiaOrderType(rawValue: value) ?? .limit
    }

    private func mapReplaceOrderType(_ orderType: PremiaOrderType) -> PremiaAPIClientGeneratedAPI.OrderReplaceRequest.OrderType {
        switch orderType {
        case .market:
            return .market
        case .limit:
            return .limit
        }
    }
}
