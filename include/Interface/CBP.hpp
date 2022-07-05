#ifndef CBP_hpp
#define CBP_hpp

#include "../src/Services/CoinbasePro/Account.hpp"
#include "../src/Services/CoinbasePro/Client.hpp"
#include "../src/Services/CoinbasePro/Product.hpp"

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
    // String response = http_client->send_request("/accounts");
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

  auto getProductTicker(CRString symbol) -> Product {
    return Product();
    // String request = "/products/" + symbol + "-USD/ticker";
    // String response = http_client->send_request(request);
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
    // String response = http_client->send_request("/transfers");
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