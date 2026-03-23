import Foundation
import XCTest

final class PremiaAPITestServer {
    let baseURL: URL

    private let workspaceURL: URL
    private let process: Process

    private init(baseURL: URL, workspaceURL: URL, process: Process) {
        self.baseURL = baseURL
        self.workspaceURL = workspaceURL
        self.process = process
    }

    static func start(assetOverrides: [String: String] = [:]) async throws -> PremiaAPITestServer {
        let repoRoot = repositoryRoot()
        let binary = resolveBinaryPath(repoRoot: repoRoot)
        guard FileManager.default.fileExists(atPath: binary.path) else {
            throw XCTSkip("premia_api binary not found at \(binary.path)")
        }

        let workspaceURL = try makeWorkspace(repoRoot: repoRoot, assetOverrides: assetOverrides)
        let port = 8500 + Int.random(in: 0..<300)
        let baseURL = URL(string: "http://127.0.0.1:\(port)")!

        let process = Process()
        process.executableURL = binary
        process.currentDirectoryURL = workspaceURL
        process.arguments = ["--host", "127.0.0.1", "--port", "\(port)"]
        process.standardOutput = Pipe()
        process.standardError = Pipe()
        try process.run()

        for _ in 0..<40 {
            try await Task.sleep(nanoseconds: 100_000_000)
            do {
                let (_, response) = try await URLSession.shared.data(from: baseURL.appending(path: "health"))
                if let http = response as? HTTPURLResponse, http.statusCode == 200 {
                    return PremiaAPITestServer(baseURL: baseURL, workspaceURL: workspaceURL, process: process)
                }
            } catch {
            }
        }

        process.terminate()
        throw XCTSkip("premia_api failed to start for integration tests")
    }

    func stop() {
        if process.isRunning {
            process.terminate()
            process.waitUntilExit()
        }
        try? FileManager.default.removeItem(at: workspaceURL)
    }

    private static func repositoryRoot() -> URL {
        var url = URL(fileURLWithPath: #filePath)
        for _ in 0..<6 {
            url.deleteLastPathComponent()
        }
        return url
    }

    private static func resolveBinaryPath(repoRoot: URL) -> URL {
        if let env = ProcessInfo.processInfo.environment["PREMIA_API_BIN"] {
            return URL(fileURLWithPath: env)
        }
        return repoRoot.appendingPathComponent("build-next-providers/bin/premia_api")
    }

    private static func makeWorkspace(repoRoot: URL, assetOverrides: [String: String]) throws -> URL {
        let fm = FileManager.default
        let root = fm.temporaryDirectory.appendingPathComponent("premia-swift-integration-\(UUID().uuidString)")
        try fm.createDirectory(at: root.appendingPathComponent("assets"), withIntermediateDirectories: true)
        for asset in ["account.json", "orders.json", "options.json", "portfolio.json", "plaid.json", "schwab.json", "tda.json", "watchlists.json"] {
            try fm.copyItem(at: repoRoot.appendingPathComponent("assets/\(asset)"),
                            to: root.appendingPathComponent("assets/\(asset)"))
        }
        for (name, contents) in assetOverrides {
            try contents.write(to: root.appendingPathComponent("assets/\(name)"), atomically: true, encoding: .utf8)
        }
        return root
    }
}
