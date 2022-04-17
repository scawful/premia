#include "CoinbasePro.hpp"
namespace cbp
{

    CoinbasePro::CoinbasePro() {
        http_client = boost::make_shared<Client>( CBP_KEY, CBP_SECRET, CBP_PASSPHRASE, true );
    }

    CoinbasePro::~CoinbasePro()=default;

    boost::shared_ptr<Account> CoinbasePro::list_accounts()
    {
        std::string response = http_client->send_request("/accounts");
        std::istringstream json_response(response);
        boost::property_tree::ptree property_tree;
        try {
            read_json( json_response, property_tree );
        } catch ( const boost::property_tree::ptree_error & json_parser_error ) {
            SDL_Log( "%s", json_parser_error.what() );
        }

        boost::shared_ptr<Account> new_account_data = boost::make_shared<Account>( property_tree );
        return new_account_data;
    }

    boost::shared_ptr<Product> CoinbasePro::get_product_ticker( const std::string & symbol )
    {
        std::string request = "/products/" + symbol + "-USD/ticker";
        std::string response = http_client->send_request(request);
        std::istringstream json_response(response);
        boost::property_tree::ptree property_tree;
        try {
            read_json( json_response, property_tree );
        } catch ( const boost::property_tree::ptree_error & e ) {
            SDL_Log( "%s", e.what() );
        }
        boost::shared_ptr<Product> new_product_data = boost::make_shared<Product>( property_tree );
        return new_product_data;
    }

    float CoinbasePro::get_deposits()
    {
        std::string response = http_client->send_request("/transfers");
        std::istringstream json_response(response);
        boost::property_tree::ptree property_tree;
        try {
            read_json( json_response, property_tree );
        } catch ( const boost::property_tree::ptree_error & e ) {
            SDL_Log( "%s", e.what() );
        }

        total_deposits_usd = 0.f;
        for ( const auto & [key,value] : property_tree ) {
            for ( const auto & [depositKey, depositValue] : value ) {
                if ( depositKey == "amount" ) {
                    total_deposits_usd += boost::lexical_cast<float>(depositValue.get_value<float>());
                }
            }
        }
        return total_deposits_usd;
    }

}