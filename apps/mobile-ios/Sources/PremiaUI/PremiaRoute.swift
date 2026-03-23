import Foundation

public enum PremiaRoute: Hashable {
    case watchlist(id: String)
    case quote(symbol: String)
    case chart(symbol: String)
}
