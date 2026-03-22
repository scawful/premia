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
    targets: [
        .target(name: "PremiaModels"),
        .target(name: "PremiaAPIClient", dependencies: ["PremiaModels"]),
        .target(name: "PremiaUI", dependencies: ["PremiaModels", "PremiaAPIClient"]),
    ]
)
