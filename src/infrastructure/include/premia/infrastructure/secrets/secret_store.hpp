#ifndef PREMIA_INFRASTRUCTURE_SECRETS_SECRET_STORE_HPP
#define PREMIA_INFRASTRUCTURE_SECRETS_SECRET_STORE_HPP

#include <optional>
#include <string>

#include "premia/infrastructure/secrets/runtime_paths.hpp"

namespace premia::infrastructure::secrets {

enum class SecretKind {
  kConfig,
  kTokens,
};

auto KeychainEnabled() -> bool;
auto LoadSecret(ProviderKind provider, SecretKind kind)
    -> std::optional<std::string>;
auto SaveSecret(ProviderKind provider, SecretKind kind,
                const std::string& contents) -> bool;
auto DeleteSecret(ProviderKind provider, SecretKind kind) -> bool;

}  // namespace premia::infrastructure::secrets

#endif  // PREMIA_INFRASTRUCTURE_SECRETS_SECRET_STORE_HPP
