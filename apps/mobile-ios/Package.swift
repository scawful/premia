// swift-tools-version: 5.10

import PackageDescription

let package = Package(
    name: "PremiaMobile",
    platforms: [
        .iOS(.v17),
        .macOS(.v14),
    ],
    products: [
        .library(name: "PremiaModels", targets: ["PremiaModels"]),
        .library(name: "PremiaAPIClient", targets: ["PremiaAPIClient"]),
        .library(name: "PremiaUI", targets: ["PremiaUI"]),
    ],
    dependencies: [
        .package(path: "../../clients/swift/Generated"),
    ],
    targets: [
        .target(name: "PremiaModels"),
        .target(
            name: "PremiaAPIClient",
            dependencies: [
                "PremiaModels",
                .product(name: "PremiaAPIClientGenerated", package: "Generated"),
            ]
        ),
        .target(name: "PremiaUI", dependencies: ["PremiaModels", "PremiaAPIClient"]),
        .testTarget(name: "PremiaModelsTests", dependencies: ["PremiaModels"]),
        .testTarget(name: "PremiaUITests", dependencies: ["PremiaUI", "PremiaModels", "PremiaAPIClient"]),
        .testTarget(name: "PremiaAPIClientIntegrationTests", dependencies: ["PremiaAPIClient", "PremiaModels"]),
    ]
)
