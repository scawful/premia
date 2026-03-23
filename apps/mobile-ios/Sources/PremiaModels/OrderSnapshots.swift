import Foundation

public enum PremiaAssetType: String, Codable, Sendable, Equatable {
    case equity = "EQUITY"
    case option = "OPTION"
}

public enum PremiaOrderInstruction: String, Codable, Sendable, Equatable {
    case buy = "BUY"
    case sell = "SELL"
    case buyToOpen = "BUY_TO_OPEN"
    case sellToClose = "SELL_TO_CLOSE"
    case buyToClose = "BUY_TO_CLOSE"
    case sellToOpen = "SELL_TO_OPEN"
}

public enum PremiaOrderType: String, Codable, Sendable, Equatable {
    case market = "MARKET"
    case limit = "LIMIT"
}

public struct PremiaOrderIntent: Codable, Sendable, Equatable {
    public let accountID: String?
    public let symbol: String
    public let assetType: PremiaAssetType
    public let instruction: PremiaOrderInstruction
    public let quantity: String
    public let orderType: PremiaOrderType
    public let limitPrice: String?
    public let duration: String?
    public let session: String?
    public let confirmLive: Bool

    public init(
        accountID: String? = nil,
        symbol: String,
        assetType: PremiaAssetType,
        instruction: PremiaOrderInstruction,
        quantity: String,
        orderType: PremiaOrderType,
        limitPrice: String? = nil,
        duration: String? = nil,
        session: String? = nil,
        confirmLive: Bool = false
    ) {
        self.accountID = accountID
        self.symbol = symbol
        self.assetType = assetType
        self.instruction = instruction
        self.quantity = quantity
        self.orderType = orderType
        self.limitPrice = limitPrice
        self.duration = duration
        self.session = session
        self.confirmLive = confirmLive
    }
}

public struct PremiaOrderPreview: Codable, Sendable, Equatable {
    public let previewID: String
    public let accountID: String
    public let symbol: String
    public let assetType: PremiaAssetType
    public let instruction: PremiaOrderInstruction
    public let quantity: String
    public let orderType: PremiaOrderType
    public let limitPrice: String
    public let estimatedTotal: String
    public let mode: String
    public let status: String
    public let warnings: [String]
    public let asOf: Date?

    public init(previewID: String, accountID: String, symbol: String, assetType: PremiaAssetType, instruction: PremiaOrderInstruction, quantity: String, orderType: PremiaOrderType, limitPrice: String, estimatedTotal: String, mode: String, status: String, warnings: [String], asOf: Date?) {
        self.previewID = previewID
        self.accountID = accountID
        self.symbol = symbol
        self.assetType = assetType
        self.instruction = instruction
        self.quantity = quantity
        self.orderType = orderType
        self.limitPrice = limitPrice
        self.estimatedTotal = estimatedTotal
        self.mode = mode
        self.status = status
        self.warnings = warnings
        self.asOf = asOf
    }
}

public struct PremiaOrderSubmission: Codable, Sendable, Equatable {
    public let submissionID: String
    public let accountID: String
    public let symbol: String
    public let assetType: PremiaAssetType
    public let instruction: PremiaOrderInstruction
    public let quantity: String
    public let orderType: PremiaOrderType
    public let limitPrice: String
    public let mode: String
    public let status: String
    public let submittedAt: Date?
    public let message: String
    public let asOf: Date?

    public init(submissionID: String, accountID: String, symbol: String, assetType: PremiaAssetType, instruction: PremiaOrderInstruction, quantity: String, orderType: PremiaOrderType, limitPrice: String, mode: String, status: String, submittedAt: Date?, message: String, asOf: Date?) {
        self.submissionID = submissionID
        self.accountID = accountID
        self.symbol = symbol
        self.assetType = assetType
        self.instruction = instruction
        self.quantity = quantity
        self.orderType = orderType
        self.limitPrice = limitPrice
        self.mode = mode
        self.status = status
        self.submittedAt = submittedAt
        self.message = message
        self.asOf = asOf
    }
}


public struct PremiaOrderCancellation: Codable, Sendable, Equatable {
    public let orderID: String
    public let accountID: String
    public let mode: String
    public let status: String
    public let cancelledAt: Date?
    public let message: String
    public let asOf: Date?

    public init(orderID: String, accountID: String, mode: String, status: String, cancelledAt: Date?, message: String, asOf: Date?) {
        self.orderID = orderID
        self.accountID = accountID
        self.mode = mode
        self.status = status
        self.cancelledAt = cancelledAt
        self.message = message
        self.asOf = asOf
    }
}

public struct PremiaOrderReplaceIntent: Codable, Sendable, Equatable {
    public let orderID: String
    public let replacement: PremiaOrderIntent

    public init(orderID: String, replacement: PremiaOrderIntent) {
        self.orderID = orderID
        self.replacement = replacement
    }
}

public struct PremiaOrderReplacement: Codable, Sendable, Equatable {
    public let replacementID: String
    public let replacedOrderID: String
    public let accountID: String
    public let symbol: String
    public let assetType: PremiaAssetType
    public let instruction: PremiaOrderInstruction
    public let quantity: String
    public let orderType: PremiaOrderType
    public let limitPrice: String
    public let mode: String
    public let status: String
    public let submittedAt: Date?
    public let message: String
    public let asOf: Date?

    public init(replacementID: String, replacedOrderID: String, accountID: String, symbol: String, assetType: PremiaAssetType, instruction: PremiaOrderInstruction, quantity: String, orderType: PremiaOrderType, limitPrice: String, mode: String, status: String, submittedAt: Date?, message: String, asOf: Date?) {
        self.replacementID = replacementID
        self.replacedOrderID = replacedOrderID
        self.accountID = accountID
        self.symbol = symbol
        self.assetType = assetType
        self.instruction = instruction
        self.quantity = quantity
        self.orderType = orderType
        self.limitPrice = limitPrice
        self.mode = mode
        self.status = status
        self.submittedAt = submittedAt
        self.message = message
        self.asOf = asOf
    }
}

public struct PremiaOrderRecord: Codable, Sendable, Equatable, Identifiable {
    public let id: String
    public let accountID: String
    public let symbol: String
    public let assetType: PremiaAssetType
    public let instruction: PremiaOrderInstruction
    public let quantity: String
    public let orderType: PremiaOrderType
    public let limitPrice: String
    public let mode: String
    public let status: String
    public let submittedAt: Date?
    public let updatedAt: Date?
    public let message: String

    public init(id: String, accountID: String, symbol: String, assetType: PremiaAssetType, instruction: PremiaOrderInstruction, quantity: String, orderType: PremiaOrderType, limitPrice: String, mode: String, status: String, submittedAt: Date?, updatedAt: Date?, message: String) {
        self.id = id
        self.accountID = accountID
        self.symbol = symbol
        self.assetType = assetType
        self.instruction = instruction
        self.quantity = quantity
        self.orderType = orderType
        self.limitPrice = limitPrice
        self.mode = mode
        self.status = status
        self.submittedAt = submittedAt
        self.updatedAt = updatedAt
        self.message = message
    }
}
