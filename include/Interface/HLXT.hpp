#ifndef HLXT_hpp
#define HLXT_hpp

#include "../src/Services/Halext/Client.hpp"
#include "../src/Services/Halext/User.hpp"

namespace Premia {
namespace halext {
enum class ChartType { LINEPLOT, CANDLESTICK, MULTIPLOT };

class HLXT {
 private:
  HLXT() = default;
  User user;
  Client client;
  ChartType chartType;
  bool privateBalance = false;

 public:
  HLXT(HLXT const&) = delete;
  void operator=(HLXT const&) = delete;
  static HLXT& getInstance() {
    static HLXT instance;
    return instance;
  }

  auto getSqueezeMetricsData() const -> String {
    return client.send_request(
        "https://squeezemetrics.com/monitor/download/SPX.csv");
  }

  auto setPrivateBalance(bool val) -> void { privateBalance = val; }

  auto getPrivateBalance() const -> bool { return privateBalance; }

  auto setSelectedChart(int i) -> void { chartType = ChartType(i); }

  auto getSelectedChart() const -> ChartType { return chartType; }
};
}  // namespace halext
}  // namespace Premia

#endif