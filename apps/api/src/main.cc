#include <cstdlib>
#include <iostream>
#include <string>

#include "http/server.h"

namespace {

auto ReadArgument(int argc, char* argv[], const std::string& flag,
                  const std::string& fallback) -> std::string {
  for (int i = 1; i < argc - 1; ++i) {
    if (flag == argv[i]) {
      return argv[i + 1];
    }
  }
  return fallback;
}

}  // namespace

int main(int argc, char* argv[]) {
  const auto host = ReadArgument(argc, argv, "--host", "127.0.0.1");
  const auto port_text = ReadArgument(argc, argv, "--port", "8080");

  try {
    const auto port = static_cast<unsigned short>(std::stoi(port_text));
    premia::api::http::RunHttpServer(host, port);
  } catch (const std::exception& e) {
    std::cerr << "premia_api failed to start: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
