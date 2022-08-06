#ifndef TraderWorkstationApiInterface_hpp
#define TraderWorkstationApiInterface_hpp

#include "service/InteractiveBrokers/Client.hpp"

namespace premia {
namespace tws {

class TWS {
 private:
  TWS() = default;

 public:
  TWS(TWS const&) = delete;
  void operator=(TWS const&) = delete;
  static TWS& getInstance() {
    static TWS instance;
    return instance;
  }

  // functions
  auto runClient(std::string host, int port, int clientId) -> void {
    std::string options = "";
    constexpr auto MAX_ATTEMPTS = 10;
    constexpr auto SLEEP_TIME = 5;
    unsigned attempt = 0;

    if (port <= 0) port = 7496;

    for (;;) {
      ++attempt;
      printf("Attempt %u of %u\n", attempt, MAX_ATTEMPTS);
      Client client;

      if (!options.empty()) {
        client.setConnectOptions(options);
      }

      client.connect(host.c_str(), port, clientId);
      while (client.isConnected()) {
        client.processMessages();
      }

      if (attempt >= MAX_ATTEMPTS) {
        break;
      }
    }
  }
};
}  // namespace tws
}  // namespace premia

#endif