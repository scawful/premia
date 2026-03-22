#ifndef PREMIA_CORE_DOMAIN_VALUE_TYPES_HPP
#define PREMIA_CORE_DOMAIN_VALUE_TYPES_HPP

#include <string>

namespace premia::core::domain {

enum class Provider {
  kTDA,
  kSchwab,
  kIBKR,
  kPlaid,
  kInternal,
};

enum class ConnectionStatus {
  kConnected,
  kConnecting,
  kNotConnected,
  kDegraded,
  kReauthRequired,
};

struct Money {
  std::string amount;
  std::string currency;
};

struct AbsolutePercentChange {
  Money absolute;
  std::string percent;
};

inline auto ProviderToString(Provider provider) -> std::string {
  switch (provider) {
    case Provider::kTDA:
      return "tda";
    case Provider::kSchwab:
      return "schwab";
    case Provider::kIBKR:
      return "ibkr";
    case Provider::kPlaid:
      return "plaid";
    case Provider::kInternal:
      return "internal";
  }
  return "internal";
}

inline auto ProviderFromString(const std::string& provider) -> Provider {
  if (provider == "tda") {
    return Provider::kTDA;
  }
  if (provider == "schwab") {
    return Provider::kSchwab;
  }
  if (provider == "ibkr") {
    return Provider::kIBKR;
  }
  if (provider == "plaid") {
    return Provider::kPlaid;
  }
  return Provider::kInternal;
}

inline auto ConnectionStatusToString(ConnectionStatus status) -> std::string {
  switch (status) {
    case ConnectionStatus::kConnected:
      return "connected";
    case ConnectionStatus::kConnecting:
      return "connecting";
    case ConnectionStatus::kNotConnected:
      return "not_connected";
    case ConnectionStatus::kDegraded:
      return "degraded";
    case ConnectionStatus::kReauthRequired:
      return "reauth_required";
  }
  return "not_connected";
}

}  // namespace premia::core::domain

#endif  // PREMIA_CORE_DOMAIN_VALUE_TYPES_HPP
