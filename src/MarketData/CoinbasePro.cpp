#include "CoinbasePro.hpp"
namespace cbp
{
    void CoinbasePro::get_account_data( std::string filename )
    {
        CURL *curl;
        CURLcode res;
        FILE *fp;
        //std::fstream fp;

        curl = curl_easy_init();
        if ( curl )
        {
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            // create header structure 
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // set the access key 
            std::string access_key = "CB-ACCESS-KEY: " + CBP_KEY;
            headers = curl_slist_append( headers, access_key.c_str() );
            res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // prepare the message contents to be encoded 
            _body = "";
            _method = "GET";
            _request_path = _endpoint_url + "/accounts";
            _timestamp = std::time(0);
            _message = std::to_string( _timestamp ) + _method + _request_path + _body ;

            SDL_Log("%s", _message.c_str() );

            // base64 decode the CoinbasePro secret key
            const auto pl = 3 * CBP_SECRET.length() / 4;
            auto hmac_key = reinterpret_cast<unsigned char *>( calloc( pl + 1, 1 ) );
            const auto hmac_output_length = EVP_DecodeBlock(hmac_key, reinterpret_cast<const unsigned char *>( CBP_SECRET.c_str() ), CBP_SECRET.length() );
            if ( pl != hmac_output_length ) 
            { 
                std::cerr << "Whoops, decode predicted " << pl << " but we got " << hmac_output_length << "\n"; 
            }

            // base64 encoded HMAC sha256
            HMAC
            (
                EVP_sha256(),
                hmac_key,
                static_cast<int>( hmac_output_length ),
                reinterpret_cast<unsigned char const*>( _message.data() ),
                static_cast<int>( _message.size() ),
                _hash.data(),
                &_hash_length
            );

            free( hmac_key );

            const auto pl2 = 4 * ( ( _hash_length + 2 ) / 3);
            auto b64_encode = reinterpret_cast<char*>( calloc( pl2 + 1, 1 ) );
            const auto ol2 = EVP_EncodeBlock( reinterpret_cast<unsigned char *>(b64_encode), _hash.data(), _hash_length );
            _access_signature = boost::lexical_cast<std::string>(b64_encode, ol2);
            free( b64_encode );

            //_access_signature = std::string{ reinterpret_cast<char const*>( _hash.data()), _hash_length };

            // add the signature to the header
            std::string access_sign = "CB-ACCESS-SIGN: " + _access_signature;
            headers = curl_slist_append( headers, access_sign.c_str() );
            res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // add the timestamp to the header
            std::string timestamp = "CB-ACCESS-TIMESTAMP: " + std::to_string( _timestamp );
            headers = curl_slist_append( headers, timestamp.c_str() );
            res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // add the passphrase to the header
            std::string passphrase = "CB-ACCESS-PASSPHRASE: " + CBP_PASSPHRASE;
            headers = curl_slist_append( headers, passphrase.c_str() );
            res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // specify the user agent
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "premia-agent/1.0");

            fp = fopen( filename.c_str(), "wb" );

            curl_easy_setopt(curl, CURLOPT_URL, _request_path.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

            res = curl_easy_perform(curl);

            curl_easy_cleanup(curl);
            fclose(fp);
        }

    }

    CoinbasePro::CoinbasePro()
    {
        http_client = boost::make_shared<Client>(CBP_KEY, CBP_SECRET, CBP_PASSPHRASE, true );
        _endpoint_url = "https://api.pro.coinbase.com";
    }

    CoinbasePro::~CoinbasePro()
    {

    }

    boost::shared_ptr<Account> CoinbasePro::list_accounts()
    {
        std::time_t now = std::time(0);
        std::string output_filename = "cbp_accounts_" + std::to_string(now) + ".json";
        http_client->send_request( output_filename, "/accounts" );
        //get_account_data( output_filename );

        std::ifstream json_file( output_filename, std::ios::in | std::ios::binary );

        boost::property_tree::ptree property_tree;

        try {
            read_json(json_file, property_tree);
        }
        catch ( std::exception& json_parser_error ) {
            SDL_Log("%s", json_parser_error.what() );
        }

        json_file.close();

        boost::shared_ptr<Account> new_account_data = boost::make_shared<Account>( property_tree );
        return new_account_data;

    }

}