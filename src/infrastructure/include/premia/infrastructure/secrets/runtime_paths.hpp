#ifndef PREMIA_INFRASTRUCTURE_SECRETS_RUNTIME_PATHS_HPP
#define PREMIA_INFRASTRUCTURE_SECRETS_RUNTIME_PATHS_HPP

#include <filesystem>
#include <string>

namespace premia::infrastructure::secrets {

enum class ProviderKind {
  kTDA,
  kSchwab,
  kIBKR,
  kPlaid,
};

auto RuntimeRoot() -> std::filesystem::path;
auto ProviderRuntimeDir(ProviderKind provider) -> std::filesystem::path;
auto ProviderConfigPath(ProviderKind provider) -> std::filesystem::path;
auto ProviderTokenPath(ProviderKind provider) -> std::filesystem::path;

auto FileExists(const std::filesystem::path& path) -> bool;
auto EnsureProviderDir(ProviderKind provider) -> bool;
auto CopyFileToSecureStore(const std::filesystem::path& source,
                           const std::filesystem::path& destination) -> bool;
auto WriteSecureText(const std::filesystem::path& destination,
                     const std::string& contents) -> bool;

}  // namespace premia::infrastructure::secrets

#endif  // PREMIA_INFRASTRUCTURE_SECRETS_RUNTIME_PATHS_HPP
