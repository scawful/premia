import Foundation

enum RuntimeConfiguration {
    static var baseURL: URL {
        if let rawValue = ProcessInfo.processInfo.environment["PREMIA_API_BASE_URL"],
           let url = URL(string: rawValue) {
            return url
        }

        if let rawValue = UserDefaults.standard.string(forKey: "PREMIA_API_BASE_URL"),
           let url = URL(string: rawValue) {
            return url
        }

        return URL(string: "http://127.0.0.1:8080")!
    }
}
