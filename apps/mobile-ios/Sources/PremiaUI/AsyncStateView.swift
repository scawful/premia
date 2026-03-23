import SwiftUI
import PremiaModels

public struct AsyncStateView<Content: View>: View {
    private let isLoading: Bool
    private let error: PremiaAPIClientError?
    private let retry: (() -> Void)?
    private let content: Content

    public init(
        isLoading: Bool,
        error: PremiaAPIClientError?,
        retry: (() -> Void)? = nil,
        @ViewBuilder content: () -> Content
    ) {
        self.isLoading = isLoading
        self.error = error
        self.retry = retry
        self.content = content()
    }

    public var body: some View {
        Group {
            if isLoading {
                ProgressView()
                    .frame(maxWidth: .infinity, maxHeight: .infinity)
            } else if let error {
                VStack(spacing: 12) {
                    Text(error.message)
                        .multilineTextAlignment(.center)
                    if let retry {
                        Button("Retry", action: retry)
                    }
                }
                .frame(maxWidth: .infinity, maxHeight: .infinity)
                .padding()
            } else {
                content
            }
        }
    }
}
