#include "premia/infrastructure/secrets/secret_store.hpp"

#include <cstdlib>
#include <string>

#if defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#include <Security/Security.h>
#endif

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

  auto service = CFStringCreateWithCString(nullptr, service_name.c_str(),
                                           kCFStringEncodingUTF8);
  auto account = CFStringCreateWithCString(nullptr, account_name.c_str(),
                                           kCFStringEncodingUTF8);

  const void* keys[] = {kSecClass, kSecAttrService, kSecAttrAccount,
                        kSecReturnData, kSecMatchLimit};
  const void* values[] = {kSecClassGenericPassword, service, account,
                          kCFBooleanTrue, kSecMatchLimitOne};
  auto query = CFDictionaryCreate(nullptr, keys, values, 5,
                                  &kCFTypeDictionaryKeyCallBacks,
                                  &kCFTypeDictionaryValueCallBacks);

  CFTypeRef result = nullptr;
  const auto status = SecItemCopyMatching(query, &result);

  if (query != nullptr) CFRelease(query);
  if (service != nullptr) CFRelease(service);
  if (account != nullptr) CFRelease(account);

  if (status != errSecSuccess || result == nullptr) {
    if (result != nullptr) CFRelease(result);
    return std::nullopt;
  }

  const auto data = static_cast<CFDataRef>(result);
  const auto* bytes = CFDataGetBytePtr(data);
  const auto length = CFDataGetLength(data);
  std::string value(reinterpret_cast<const char*>(bytes),
                    static_cast<std::size_t>(length));
  CFRelease(result);
  return value;
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

  auto service = CFStringCreateWithCString(nullptr, service_name.c_str(),
                                           kCFStringEncodingUTF8);
  auto account = CFStringCreateWithCString(nullptr, account_name.c_str(),
                                           kCFStringEncodingUTF8);
  auto data = CFDataCreate(nullptr,
                           reinterpret_cast<const UInt8*>(contents.data()),
                           static_cast<CFIndex>(contents.size()));

  const void* delete_keys[] = {kSecClass, kSecAttrService, kSecAttrAccount};
  const void* delete_values[] = {kSecClassGenericPassword, service, account};
  auto delete_query = CFDictionaryCreate(nullptr, delete_keys, delete_values, 3,
                                         &kCFTypeDictionaryKeyCallBacks,
                                         &kCFTypeDictionaryValueCallBacks);
  SecItemDelete(delete_query);
  if (delete_query != nullptr) CFRelease(delete_query);

  const void* add_keys[] = {kSecClass, kSecAttrService, kSecAttrAccount,
                            kSecValueData};
  const void* add_values[] = {kSecClassGenericPassword, service, account, data};
  auto add_query = CFDictionaryCreate(nullptr, add_keys, add_values, 4,
                                      &kCFTypeDictionaryKeyCallBacks,
                                      &kCFTypeDictionaryValueCallBacks);
  const auto status = SecItemAdd(add_query, nullptr);

  if (add_query != nullptr) CFRelease(add_query);
  if (data != nullptr) CFRelease(data);
  if (service != nullptr) CFRelease(service);
  if (account != nullptr) CFRelease(account);
  return status == errSecSuccess;
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

  auto service = CFStringCreateWithCString(nullptr, service_name.c_str(),
                                           kCFStringEncodingUTF8);
  auto account = CFStringCreateWithCString(nullptr, account_name.c_str(),
                                           kCFStringEncodingUTF8);
  const void* keys[] = {kSecClass, kSecAttrService, kSecAttrAccount};
  const void* values[] = {kSecClassGenericPassword, service, account};
  auto query = CFDictionaryCreate(nullptr, keys, values, 3,
                                  &kCFTypeDictionaryKeyCallBacks,
                                  &kCFTypeDictionaryValueCallBacks);
  const auto status = SecItemDelete(query);

  if (query != nullptr) CFRelease(query);
  if (service != nullptr) CFRelease(service);
  if (account != nullptr) CFRelease(account);
  return status == errSecSuccess || status == errSecItemNotFound;
#endif
}

}  // namespace premia::infrastructure::secrets
