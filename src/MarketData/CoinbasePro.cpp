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

}