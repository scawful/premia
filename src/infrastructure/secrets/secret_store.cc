#include "premia/infrastructure/secrets/secret_store.hpp"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <string>

namespace premia::infrastructure::secrets {

namespace {

auto ProviderName(ProviderKind provider) -> std::string {
  switch (provider) {
    case ProviderKind::kTDA:
      return "tda";
    case ProviderKind::kSchwab:
      return "schwab";
    case ProviderKind::kPlaid:
      return "plaid";
  }
  return "provider";
}

auto SecretName(SecretKind kind) -> std::string {
  switch (kind) {
    case SecretKind::kConfig:
      return "config";
    case SecretKind::kTokens:
      return "tokens";
  }
  return "secret";
}

auto SecretAccount(ProviderKind provider, SecretKind kind) -> std::string {
  return ProviderName(provider) + "." + SecretName(kind);
}

auto ShellEscape(const std::string& value) -> std::string {
  std::string escaped = "'";
  for (const char ch : value) {
    if (ch == '\'') {
      escaped += "'\\''";
    } else {
      escaped.push_back(ch);
    }
  }
  escaped += "'";
  return escaped;
}

auto HexEncode(const std::string& value) -> std::string {
  static constexpr char kHex[] = "0123456789abcdef";
  std::string encoded;
  encoded.reserve(value.size() * 2);
  for (const unsigned char ch : value) {
    encoded.push_back(kHex[(ch >> 4) & 0x0F]);
    encoded.push_back(kHex[ch & 0x0F]);
  }
  return encoded;
}

auto RunCommandCapture(const std::string& command) -> std::optional<std::string> {
  std::array<char, 256> buffer{};
  std::string output;
  FILE* pipe = popen(command.c_str(), "r");
  if (pipe == nullptr) {
    return std::nullopt;
  }
  while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
    output += buffer.data();
  }
  const auto status = pclose(pipe);
  if (status != 0) {
    return std::nullopt;
  }
  if (!output.empty() && output.back() == '\n') {
    output.pop_back();
  }
  return output;
}

auto RunCommandStatus(const std::string& command) -> bool {
  return std::system(command.c_str()) == 0;
}

}  // namespace

auto KeychainEnabled() -> bool {
#if defined(__APPLE__)
  if (const char* disabled = std::getenv("PREMIA_DISABLE_KEYCHAIN");
      disabled != nullptr) {
    const std::string value(disabled);
    return value != "1" && value != "true" && value != "TRUE";
  }
  return true;
#else
  return false;
#endif
}

auto LoadSecret(ProviderKind provider, SecretKind kind)
    -> std::optional<std::string> {
#if !defined(__APPLE__)
  (void)provider;
  (void)kind;
  return std::nullopt;
#else
  if (!KeychainEnabled()) {
    return std::nullopt;
  }

  const auto service_name = std::string("com.scawful.premia");
  const auto account_name = SecretAccount(provider, kind);
  const auto command = std::string("/usr/bin/security find-generic-password -s ") +
                       ShellEscape(service_name) + " -a " +
                       ShellEscape(account_name) + " -w 2>/dev/null";
  return RunCommandCapture(command);
#endif
}

auto SaveSecret(ProviderKind provider, SecretKind kind,
                const std::string& contents) -> bool {
#if !defined(__APPLE__)
  (void)provider;
  (void)kind;
  (void)contents;
  return false;
#else
  if (!KeychainEnabled()) {
    return false;
  }

  const auto service_name = std::string("com.scawful.premia");
  const auto account_name = SecretAccount(provider, kind);
  const auto command = std::string("/usr/bin/security add-generic-password -U -s ") +
                       ShellEscape(service_name) + " -a " +
                       ShellEscape(account_name) + " -X " +
                       ShellEscape(HexEncode(contents)) + " >/dev/null 2>&1";
  return RunCommandStatus(command);
#endif
}

auto DeleteSecret(ProviderKind provider, SecretKind kind) -> bool {
#if !defined(__APPLE__)
  (void)provider;
  (void)kind;
  return false;
#else
  if (!KeychainEnabled()) {
    return false;
  }

  const auto service_name = std::string("com.scawful.premia");
  const auto account_name = SecretAccount(provider, kind);
  const auto command = std::string("/usr/bin/security delete-generic-password -s ") +
                       ShellEscape(service_name) + " -a " +
                       ShellEscape(account_name) + " >/dev/null 2>&1";
  return RunCommandStatus(command);
#endif
}

}  // namespace premia::infrastructure::secrets
