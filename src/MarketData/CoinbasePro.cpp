#include "CoinbasePro.hpp"
namespace cbp
{

    CoinbasePro::CoinbasePro()
    {
        http_client = boost::make_shared<Client>( CBP_KEY, CBP_SECRET, CBP_PASSPHRASE, true );
    }

    CoinbasePro::~CoinbasePro()
    {

    }

    boost::shared_ptr<Account> CoinbasePro::list_accounts()
    {
        std::time_t now = std::time(0);
        std::string output_filename = "cbp_accounts_" + std::to_string(now) + ".json";
        http_client->send_request( output_filename, "/accounts" );

        std::ifstream json_file( output_filename, std::ios::in | std::ios::binary );

        boost::property_tree::ptree property_tree;

        try {
            read_json( json_file, property_tree );
        }
        catch ( std::exception& json_parser_error ) {
            SDL_Log( "%s", json_parser_error.what() );
        }

        json_file.close();
        std::remove( output_filename.c_str() );

        boost::shared_ptr<Account> new_account_data = boost::make_shared<Account>( property_tree );
        return new_account_data;
    }

    boost::shared_ptr<Product> CoinbasePro::get_product_ticker( std::string symbol )
    {
        std::time_t now = std::time(0);
        std::string output_filename = "cbp_product_" + symbol + "_" + std::to_string(now) + ".json";

        std::string request = "/products/" + symbol + "-USD/ticker";
        http_client->send_request( output_filename, request );

        std::ifstream json_file( output_filename, std::ios::in | std::ios::binary );
        boost::property_tree::ptree property_tree;
        
        try {
            read_json( json_file, property_tree );
        }
        catch ( std::exception& json_parser_error ) {
            SDL_Log( "%s", json_parser_error.what() );
        }

        json_file.close();
        std::remove( output_filename.c_str() );
        boost::shared_ptr<Product> new_product_data = boost::make_shared<Product>( property_tree );
        return new_product_data;
    }

    float CoinbasePro::get_deposits()
    {
        std::time_t now = std::time(0);
        std::string output_filename = "cbp_deposits_" + std::to_string(now) + ".json";
        http_client->send_request( output_filename, "/transfers" );

        std::ifstream json_file( output_filename, std::ios::in | std::ios::binary );
        boost::property_tree::ptree property_tree;
        
        try {
            read_json( json_file, property_tree );
        }
        catch ( std::exception& json_parser_error ) {
            SDL_Log( "%s", json_parser_error.what() );
        }

        json_file.close();
        std::remove( output_filename.c_str() );

        total_deposits_usd = 0.f;
        for ( auto & deposit_it : property_tree )
        {
            for ( auto & each_deposit : deposit_it.second )
            {
                if ( each_deposit.first == "amount" )
                {
                    total_deposits_usd += boost::lexical_cast<float>(each_deposit.second.get_value<float>());
                }
            }
        }

        return total_deposits_usd;
    }

}