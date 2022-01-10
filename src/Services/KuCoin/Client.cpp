#include "Client.hpp"

namespace kc
{

    // NOT MY CODE 
    // https://stackoverflow.com/questions/5288076/base64-encoding-and-decoding-with-openssl
    // just testing bc omfg base64 sucks 

    auto EncodeBase64(const std::string& to_encode) -> std::string 
    {
        /// @sa https://www.openssl.org/docs/manmaster/man3/EVP_EncodeBlock.html

        const auto predicted_len = 4 * ((to_encode.length() + 2) / 3);  // predict output size

        const auto output_buffer{std::make_unique<char[]>(predicted_len + 1)};

        const std::vector<unsigned char> vec_chars{to_encode.begin(), to_encode.end()};  // convert to_encode into uchar container

        const auto output_len = EVP_EncodeBlock(reinterpret_cast<unsigned char*>(output_buffer.get()), vec_chars.data(), static_cast<int>(vec_chars.size()));

        if (predicted_len != static_cast<unsigned long>(output_len)) {
            throw std::runtime_error("EncodeBase64 error");
        }

        return output_buffer.get();
    }

    
    auto DecodeBase64(const std::string& to_decode) -> std::string 
    {
        /// @sa https://www.openssl.org/docs/manmaster/man3/EVP_DecodeBlock.html

        const auto predicted_len = 3 * to_decode.length() / 4;  // predict output size

        const auto output_buffer{std::make_unique<char[]>(predicted_len + 1)};

        const std::vector<unsigned char> vec_chars{to_decode.begin(), to_decode.end()};  // convert to_decode into uchar container

        const auto output_len = EVP_DecodeBlock(reinterpret_cast<unsigned char*>(output_buffer.get()), vec_chars.data(), static_cast<int>(vec_chars.size()));

        if (predicted_len != static_cast<unsigned long>(output_len)) {
            throw std::runtime_error("DecodeBase64 error");
        }

        return output_buffer.get();
    }

    // END OF NOT MY CODE 

    long Client::get_server_time()
    {
        CURL *curl;
        CURLcode res;
        FILE *fp;
        long server_time;
        std::string filename = "time_request.json";

        curl = curl_easy_init();
        if ( curl )
        {
            // create header structure 
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // specify the user agent
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "premia-agent/1.0");

            std::string _request_path = _endpoint_url + "/api/v1/timestamp";
            
            fp = fopen( filename.c_str(), "wb");
            curl_easy_setopt(curl, CURLOPT_URL, _request_path.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            fclose(fp);
        }

        std::ifstream json_file( "time_request.json", std::ios::in | std::ios::binary );

        boost::property_tree::ptree property_tree;

        try {
            read_json(json_file, property_tree);
        }
        catch ( std::exception& json_parser_error ) {
            SDL_Log("%s", json_parser_error.what() );
        }

        json_file.close();

        for ( auto& time_it: property_tree )
        {
            if ( time_it.first == "data" )
            {
                server_time = time_it.second.get_value<long>();
            }
        }

        //server_time = server_time.substr(0, server_time.find('.') );

        std::remove( filename.c_str() );

        return server_time;
    }

    Client::Client()
    {
        _endpoint_url = "https://api.kucoin.com";
    }

    Client::Client( std::string api_key, std::string secret_key, std::string key_version, std::string passphrase )
    {
        _api_key = api_key;
        _secret_key = secret_key;
        _key_version = key_version;
        _passphrase = passphrase;
        _endpoint_url = "https://api.kucoin.com";
    }

    Client::~Client() = default;

    void Client::send_request( std::string filename, std::string request )
    {
        FILE *fp;
        CURL *curl;
        CURLcode res;

        curl = curl_easy_init();
        if ( curl )
        {
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            // create header structure 
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // set the api key 
            std::string access_key = "KC-API-KEY: " + _api_key;
            headers = curl_slist_append( headers, access_key.c_str() );
            res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // prepare the message contents to be encoded 
            std::string _body = "";
            std::string _method = "GET";
            std::string _request_path = _endpoint_url + request;
            std::string _timestamp = std::to_string(get_server_time());
            std::string _message = _timestamp + _method + request + _body ;

            // api_key = "api_key"
            // api_secret = "api_secret"
            // api_passphrase = "api_passphrase"
            // url = 'https://openapi-sandbox.kucoin.com/api/v1/accounts'
            // now = int(time.time() * 1000)
            // str_to_sign = str(now) + 'GET' + '/api/v1/accounts'
            // signature = base64.b64encode(
            //     hmac.new(api_secret.encode('utf-8'), str_to_sign.encode('utf-8'), hashlib.sha256).digest())
            // passphrase = base64.b64encode(hmac.new(api_secret.encode('utf-8'), api_passphrase.encode('utf-8'), hashlib.sha256).digest())
            // headers = {
            //     "KC-API-SIGN": signature,
            //     "KC-API-TIMESTAMP": str(now),
            //     "KC-API-KEY": api_key,
            //     "KC-API-PASSPHRASE": passphrase,
            //     "KC-API-KEY-VERSION": "2"
            // }

            SDL_Log("%s", _message.c_str() );

            char *hmac_key = strdup( _secret_key.c_str() );
            int key_len = strlen(hmac_key);

            const auto *data = (const unsigned char *) strdup( _message.c_str() );
            int data_len = strlen((char *) _message.c_str() );

            unsigned char *md = nullptr;
            unsigned int md_len = -1;

            // HMAC sha256
            md = HMAC
            (
                EVP_sha256(),
                hmac_key,
                static_cast<int>( key_len ),
                reinterpret_cast<unsigned char const*>( data ),
                static_cast<int>( data_len ),
                md,
                &md_len
            );

            // base64 encode the hmac signature data 
            const char *pre_encode_signature_c = strdup(reinterpret_cast<const char *>(md));
            std::string pre_encode_signature( pre_encode_signature_c );
            std::string post_encode_signature = EncodeBase64( pre_encode_signature );

            // free data related to encoding signature 
            free( hmac_key );
            free( (char *) data );
            free( (char *) pre_encode_signature_c );

            // add the signature to the header
            std::string access_sign = "KC-API-SIGN: " + post_encode_signature;
            headers = curl_slist_append( headers, access_sign.c_str() );
            res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // add the timestamp to the header
            std::string timestamp = "KC-API-TIMESTAMP: " + _timestamp;
            headers = curl_slist_append( headers, timestamp.c_str() );
            res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // add the passphrase to the header
            std::string passphrase = "KC-API-PASSPHRASE: " + _passphrase;
            headers = curl_slist_append( headers, passphrase.c_str() );
            res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            std::string key_version = "KC-API-KEY-VERSION" + _key_version;
            headers = curl_slist_append( headers, key_version.c_str() );
            res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // specify the user agent
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "Premia");

            fp = fopen( filename.c_str(), "wb" );

            curl_easy_setopt(curl, CURLOPT_URL, _request_path.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

            res = curl_easy_perform(curl);

            curl_easy_cleanup(curl);
            fclose(fp);
        }
    }

    void Client::send_request( std::string filename, std::string request, std::string method, std::string body )
    {

    }
}