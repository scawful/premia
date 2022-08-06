#ifndef CBP_hpp
#define CBP_hpp

#include "service/CoinbasePro/Account.hpp"
#include "service/CoinbasePro/Client.hpp"
#include "service/CoinbasePro/Product.hpp"

namespace premia {
namespace cbp {
class CBP {
 private:
  CBP() {}
  Account account;
  Client client;

 public:
  CBP(CBP const&) = delete;
  void operator=(CBP const&) = delete;
  static CBP& getInstance() {
    static CBP instance;
    return instance;
  }

  auto getAccounts() -> Account {
    return Account();
    // std::string response = http_client->send_request("/accounts");
    // std::istringstream json_response(response);
    // boost::property_tree::ptree property_tree;
    // try {
    //     read_json(json_response, property_tree);
    // } catch ( const boost::property_tree::ptree_error & e ) {
    //     // @todo replace with ConsoleLogger
    //     std::cout << e.what() << std::endl;
    // }

    // boost::shared_ptr<Account> new_account_data =
    // boost::make_shared<Account>(); return new_account_data;
  }

  auto getProductTicker(const std::string &symbol) -> Product {
    return Product();
    // std::string request = "/products/" + symbol + "-USD/ticker";
    // std::string response = http_client->send_request(request);
    // std::istringstream json_response(response);
    // boost::property_tree::ptree property_tree;
    // try {
    //     read_json(json_response, property_tree);
    // } catch ( const boost::property_tree::ptree_error & e ) {
    //     // @todo replace with ConsoleLogger
    //     std::cout << e.what() << std::endl;
    // }
    // boost::shared_ptr<Product> new_product_data =
    // boost::make_shared<Product>(); return new_product_data;
  }

  auto getTotalDepositsUSD() -> double {
    // std::string response = http_client->send_request("/transfers");
    // std::istringstream json_response(response);
    // boost::property_tree::ptree property_tree;
    // try {
    //     read_json(json_response, property_tree);
    // } catch ( const boost::property_tree::ptree_error & e ) {
    //     // @todo replace with ConsoleLogger
    //     std::cout << e.what() << std::endl;
    // }

    // total_deposits_usd = 0.f;
    // for ( const auto & [key,value] : property_tree ) {
    //     for ( const auto & [depositKey, depositValue] : value ) {
    //         if ( depositKey == "amount" ) {
    //             total_deposits_usd +=
    //             boost::lexical_cast<float>(depositValue.get_value<float>());
    //         }
    //     }
    // }
    // return total_deposits_usd;
    return 0.0;
  }
};
}  // namespace cbp
}  // namespace premia

#endif