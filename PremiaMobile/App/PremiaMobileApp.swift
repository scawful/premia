import SwiftUI
import PremiaUI

@main
struct PremiaMobileApp: App {
    var body: some Scene {
        WindowGroup {
            PremiaRootView(baseURL: RuntimeConfiguration.baseURL)
        }
    }
}
