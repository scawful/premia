import Foundation
import PremiaModels
import PremiaAPIClientGenerated

public struct PremiaAPIConfiguration: Sendable, Equatable {
    public let baseURL: URL

    public init(baseURL: URL) {
        self.baseURL = baseURL
    }
}

public final class PremiaAPIClient: @unchecked Sendable {
    public let configuration: PremiaAPIConfiguration

    public init(configuration: PremiaAPIConfiguration) {
        self.configuration = configuration
        PremiaAPIClientGeneratedAPI.basePath = configuration.baseURL.absoluteString
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
        let response = try await executeMapped { try await BootstrapAPI.getBootstrap() }
        let connections = response.data.connections.map(mapConnection)
        return PremiaBootstrapSnapshot(
            environment: response.data.session.environment,
            featureFlags: response.data.session.featureFlags,
            connections: connections,
            asOf: response.meta.asOf
        )
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func loadHome() async throws -> PremiaHomeSnapshot {
        let response = try await executeMapped { try await HomeAPI.getHomeScreen() }
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
        let response = try await executeMapped { try await AccountAPI.getAccountScreen() }
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
        let response = try await executeMapped { try await QuotesAPI.getQuoteScreen(symbol: symbol) }
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
        range: PremiaChartRange = .oneMonth,
        interval: PremiaChartInterval = .oneDay,
        extendedHours: Bool = false
    ) async throws -> PremiaChartSnapshot {
        let response = try await executeMapped {
            try await ChartsAPI.getChartScreen(
                symbol: symbol,
                range: mapChartRange(range),
                interval: mapChartInterval(interval),
                extendedHours: extendedHours
            )
        }

        return PremiaChartSnapshot(
            instrument: mapInstrument(response.data.instrument),
            range: response.data.range,
            interval: response.data.interval,
            timezone: response.data.timezone,
            seriesType: response.data.series.type.rawValue,
            candles: response.data.series.bars.map(mapCandle),
            change: response.data.stats.map { mapChange($0.change) },
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
            try await OptionsAPI.getOptionChainScreen(
                symbol: symbol,
                strikeCount: strikeCount,
                strategy: strategy,
                range: range,
                expMonth: expMonth,
                optionType: optionType
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
        let response = try await executeMapped { try await WatchlistsAPI.listWatchlists() }
        return response.data.watchlists.map(mapWatchlist)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func loadWatchlist(id: String) async throws -> PremiaWatchlistScreenSnapshot {
        let response = try await executeMapped {
            try await WatchlistsAPI.getWatchlistScreen(watchlistId: id)
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
            try await ConnectionsAPI.startSchwabOAuth(startSchwabOAuthRequest: request)
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
            try await ConnectionsAPI.completeSchwabOAuth(
                completeSchwabOAuthRequest: request
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
            try await ConnectionsAPI.createPlaidLinkToken(
                createPlaidLinkTokenRequest: request
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
            try await ConnectionsAPI.completePlaidLink(
                completePlaidLinkRequest: request
            )
        }
        return mapConnection(response.data)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func createWatchlist(name: String) async throws -> PremiaWatchlistSummaryModel {
        let response = try await executeMapped {
            try await WatchlistsAPI.createWatchlist(
                createWatchlistRequest: PremiaAPIClientGeneratedAPI.CreateWatchlistRequest(name: name)
            )
        }
        return mapWatchlist(response.data)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func renameWatchlist(id: String, name: String) async throws -> PremiaWatchlistSummaryModel {
        let response = try await executeMapped {
            try await WatchlistsAPI.updateWatchlist(
                watchlistId: id,
                updateWatchlistRequest: PremiaAPIClientGeneratedAPI.UpdateWatchlistRequest(name: name)
            )
        }
        return mapWatchlist(response.data)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func addSymbol(_ symbol: String, toWatchlist id: String) async throws -> PremiaWatchlistSummaryModel {
        let response = try await executeMapped {
            try await WatchlistsAPI.addWatchlistSymbol(
                watchlistId: id,
                addWatchlistSymbolRequest: PremiaAPIClientGeneratedAPI.AddWatchlistSymbolRequest(symbol: symbol)
            )
        }
        return mapWatchlist(response.data)
    }

    @available(macOS 10.15, iOS 13.0, *)
    public func removeSymbol(_ symbol: String, fromWatchlist id: String) async throws -> PremiaWatchlistSummaryModel {
        let response = try await executeMapped {
            try await WatchlistsAPI.deleteWatchlistSymbol(watchlistId: id, symbol: symbol)
        }
        return mapWatchlist(response.data)
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

    private func mapError(_ error: Error) -> PremiaAPIClientError {
        if case let PremiaAPIClientGeneratedAPI.ErrorResponse.error(statusCode, data, _, underlying) = error,
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

        if case let PremiaAPIClientGeneratedAPI.ErrorResponse.error(statusCode, _, _, underlying) = error {
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
            instrumentCount: watchlist.instrumentCount
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
            updatedAt: row.updatedAt
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
            bid: mapMoney(quote.bid),
            ask: mapMoney(quote.ask),
            open: mapMoney(quote.open),
            high: mapMoney(quote.high),
            low: mapMoney(quote.low),
            previousClose: mapMoney(quote.previousClose),
            volume: quote.volume,
            updatedAt: quote.updatedAt
        )
    }

    private func mapCandle(_ candle: PremiaAPIClientGeneratedAPI.Candle) -> PremiaCandle {
        PremiaCandle(
            time: candle.time,
            open: candle.open,
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
}
