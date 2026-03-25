import Foundation

public struct PremiaRSUOverlaySnapshot: Sendable, Equatable {
    public struct VestEvent: Sendable, Equatable {
        public let date: String
        public let units: Int
        public let vested: Bool

        public init(date: String, units: Int, vested: Bool) {
            self.date = date
            self.units = units
            self.vested = vested
        }
    }

    public struct Grant: Sendable, Equatable, Identifiable {
        public let id: String
        public let symbol: String
        public let grantDate: String
        public let totalUnits: Int
        public let vestedUnits: Int
        public let unvestedUnits: Int
        public let currentPrice: PremiaMoney
        public let vestedValue: PremiaMoney
        public let unvestedValue: PremiaMoney
        public let nextVestDate: String?
        public let nextVestUnits: Int
        public let vestProgressPercent: Double
        public let schedule: [VestEvent]

        public init(
            id: String,
            symbol: String,
            grantDate: String,
            totalUnits: Int,
            vestedUnits: Int,
            unvestedUnits: Int,
            currentPrice: PremiaMoney,
            vestedValue: PremiaMoney,
            unvestedValue: PremiaMoney,
            nextVestDate: String?,
            nextVestUnits: Int,
            vestProgressPercent: Double,
            schedule: [VestEvent]
        ) {
            self.id = id
            self.symbol = symbol
            self.grantDate = grantDate
            self.totalUnits = totalUnits
            self.vestedUnits = vestedUnits
            self.unvestedUnits = unvestedUnits
            self.currentPrice = currentPrice
            self.vestedValue = vestedValue
            self.unvestedValue = unvestedValue
            self.nextVestDate = nextVestDate
            self.nextVestUnits = nextVestUnits
            self.vestProgressPercent = vestProgressPercent
            self.schedule = schedule
        }
    }

    public let totalVestedValue: PremiaMoney
    public let totalUnvestedValue: PremiaMoney
    public let nextVestDate: String?
    public let nextVestUnits: Int
    public let vestProgressPercent: Double
    public let grants: [Grant]
    public let asOf: Date?

    public init(
        totalVestedValue: PremiaMoney,
        totalUnvestedValue: PremiaMoney,
        nextVestDate: String?,
        nextVestUnits: Int,
        vestProgressPercent: Double,
        grants: [Grant],
        asOf: Date?
    ) {
        self.totalVestedValue = totalVestedValue
        self.totalUnvestedValue = totalUnvestedValue
        self.nextVestDate = nextVestDate
        self.nextVestUnits = nextVestUnits
        self.vestProgressPercent = vestProgressPercent
        self.grants = grants
        self.asOf = asOf
    }
}
