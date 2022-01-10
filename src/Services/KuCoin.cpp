#include "KuCoin.hpp"

namespace kc 
{
    KuCoin::KuCoin()
    {
        this->client = Client( KUCOIN_KEY, KUCOIN_SECRET, KUCOIN_VERSION, KUCOIN_PASSPHRASE );    
    }

    Account KuCoin::list_accounts()
    {
        std::time_t now = std::time(0);
        std::string output_filename = "kc_accounts" + std::to_string(now) + ".json";
        client.send_request( output_filename, "/api/v1/accounts" );
        std::ifstream json_file( output_filename, std::ios::in | std::ios::binary );
        boost::property_tree::ptree property_tree;

        try {
            read_json( json_file, property_tree );
        }
        catch ( std::exception& json_parser_error ) {
            SDL_Log( "%s", json_parser_error.what() );
        }

        json_file.close();
        //std::remove( output_filename.c_str() );

        return Account(property_tree);
    }

    MarginAccount KuCoin::get_margin_account()
    {
        std::time_t now = std::time(0);
        std::string output_filename = "kc_accounts" + std::to_string(now) + ".json";
        client.send_request( output_filename, "/api/v1/margin/account" );
        std::ifstream json_file( output_filename, std::ios::in | std::ios::binary );
        boost::property_tree::ptree property_tree;

        try {
            read_json( json_file, property_tree );
        }
        catch ( std::exception& json_parser_error ) {
            SDL_Log( "%s", json_parser_error.what() );
        }

        json_file.close();
        //std::remove( output_filename.c_str() );

        return parser.parse_margin_account(property_tree);
    }
}