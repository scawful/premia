#include "premia/infrastructure/secrets/runtime_paths.hpp"

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

#if !defined(_WIN32)
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace premia::infrastructure::secrets {

namespace fs = std::filesystem;

namespace {

auto ProviderFolderName(ProviderKind provider) -> std::string {
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

auto EnsureDirectorySecure(const fs::path& path) -> bool {
  std::error_code ec;
  fs::create_directories(path, ec);
  if (ec) {
    return false;
  }
#if !defined(_WIN32)
  fs::permissions(path,
                  fs::perms::owner_all,
                  fs::perm_options::replace,
                  ec);
  if (ec) {
    chmod(path.c_str(), S_IRWXU);
  }
#endif
  return true;
}

auto RestrictFilePermissions(const fs::path& path) -> void {
#if !defined(_WIN32)
  std::error_code ec;
  fs::permissions(path,
                  fs::perms::owner_read | fs::perms::owner_write,
                  fs::perm_options::replace,
                  ec);
  if (ec) {
    chmod(path.c_str(), S_IRUSR | S_IWUSR);
  }
#else
  (void)path;
#endif
}

}  // namespace

auto RuntimeRoot() -> fs::path {
  if (const char* override_path = std::getenv("PREMIA_RUNTIME_DIR");
      override_path != nullptr && override_path[0] != '\0') {
    return fs::path(override_path);
  }

#if defined(__APPLE__)
  if (const char* home = std::getenv("HOME"); home != nullptr && home[0] != '\0') {
    return fs::path(home) / "Library/Application Support/Premia";
  }
#elif defined(_WIN32)
  if (const char* appdata = std::getenv("APPDATA");
      appdata != nullptr && appdata[0] != '\0') {
    return fs::path(appdata) / "Premia";
  }
#else
  if (const char* xdg = std::getenv("XDG_STATE_HOME"); xdg != nullptr && xdg[0] != '\0') {
    return fs::path(xdg) / "premia";
  }
  if (const char* home = std::getenv("HOME"); home != nullptr && home[0] != '\0') {
    return fs::path(home) / ".local/state/premia";
  }
#endif

  return fs::current_path() / ".premia-runtime";
}

auto ProviderRuntimeDir(ProviderKind provider) -> fs::path {
  return RuntimeRoot() / "providers" / ProviderFolderName(provider);
}

auto ProviderConfigPath(ProviderKind provider) -> fs::path {
  return ProviderRuntimeDir(provider) / "config.json";
}

auto ProviderTokenPath(ProviderKind provider) -> fs::path {
  return ProviderRuntimeDir(provider) / "tokens.json";
}

auto FileExists(const fs::path& path) -> bool {
  std::error_code ec;
  return fs::exists(path, ec) && !ec;
}

auto EnsureProviderDir(ProviderKind provider) -> bool {
  return EnsureDirectorySecure(ProviderRuntimeDir(provider));
}

auto WriteSecureText(const fs::path& destination, const std::string& contents)
    -> bool {
  if (!EnsureDirectorySecure(destination.parent_path())) {
    return false;
  }

  const auto temp_name = destination.filename().string() + ".tmp";
  const auto temp_path = destination.parent_path() / temp_name;

  {
    std::ofstream output(temp_path, std::ios::trunc);
    if (!output.good()) {
      return false;
    }
    output << contents;
  }

  RestrictFilePermissions(temp_path);

  std::error_code ec;
  fs::rename(temp_path, destination, ec);
  if (ec) {
    fs::remove(destination, ec);
    ec.clear();
    fs::rename(temp_path, destination, ec);
  }
  if (ec) {
    fs::remove(temp_path, ec);
    return false;
  }

  RestrictFilePermissions(destination);
  return true;
}

auto CopyFileToSecureStore(const fs::path& source, const fs::path& destination)
    -> bool {
  std::ifstream input(source, std::ios::binary);
  if (!input.good()) {
    return false;
  }

  std::ostringstream buffer;
  buffer << input.rdbuf();
  return WriteSecureText(destination, buffer.str());
}

}  // namespace premia::infrastructure::secrets
