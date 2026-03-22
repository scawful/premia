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
        let response = try await BootstrapAPI.getBootstrap()
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
        let response = try await HomeAPI.getHomeScreen()
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
    public func loadQuote(symbol: String) async throws -> PremiaQuoteScreenSnapshot {
        let response = try await QuotesAPI.getQuoteScreen(symbol: symbol)
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
        let response = try await ChartsAPI.getChartScreen(
            symbol: symbol,
            range: mapChartRange(range),
            interval: mapChartInterval(interval),
            extendedHours: extendedHours
        )

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

    private func mapWatchlist(_ watchlist: PremiaAPIClientGeneratedAPI.WatchlistSummary) -> PremiaWatchlistSummaryModel {
        PremiaWatchlistSummaryModel(
            id: watchlist.id,
            name: watchlist.name,
            instrumentCount: watchlist.instrumentCount
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
}
