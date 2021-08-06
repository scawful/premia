#include "Client.hpp"

namespace cbp
{

    // NOT MY CODE 
    // https://stackoverflow.com/questions/5288076/base64-encoding-and-decoding-with-openssl
    // just testing bc omfg base64 sucks 
    namespace 
    {
        struct BIOFreeAll { void operator()(BIO* p) { BIO_free_all(p); } };
    }

    std::string Base64Encode(const std::vector<unsigned char>& binary)
    {
        std::unique_ptr<BIO,BIOFreeAll> b64(BIO_new(BIO_f_base64()));
        BIO_set_flags(b64.get(), BIO_FLAGS_BASE64_NO_NL);
        BIO* sink = BIO_new(BIO_s_mem());
        BIO_push(b64.get(), sink);
        BIO_write(b64.get(), binary.data(), binary.size());
        BIO_flush(b64.get());
        const char* encoded;
        const long len = BIO_get_mem_data(sink, &encoded);
        return std::string(encoded, len);
    }

    std::vector<unsigned char> Base64Decode(const char* encoded)
    {
        std::unique_ptr<BIO,BIOFreeAll> b64(BIO_new(BIO_f_base64()));
        BIO_set_flags(b64.get(), BIO_FLAGS_BASE64_NO_NL);
        BIO* source = BIO_new_mem_buf(encoded, -1); // read-only source
        BIO_push(b64.get(), source);
        const int maxlen = strlen(encoded) / 4 * 3 + 1;
        std::vector<unsigned char> decoded(maxlen);
        const int len = BIO_read(b64.get(), decoded.data(), maxlen);
        decoded.resize(len);
        return decoded;
    }
    // END OF NOT MY CODE 

    std::string Client::get_server_time()
    {
        CURL *curl;
        CURLcode res;
        FILE *fp;
        std::string server_time;

        curl = curl_easy_init();
        if ( curl )
        {
            // create header structure 
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // specify the user agent
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "premia-agent/1.0");

            std::string _request_path = _endpoint_url + "/time";
            
            fp = fopen( "time_request.json", "wb");
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
            if ( time_it.first == "epoch" )
            {
                server_time = time_it.second.get_value<std::string>();
            }
        }

        server_time = server_time.substr(0, server_time.find('.') );

        return server_time;
    }

    Client::Client( std::string api_key, std::string secret_key, std::string passphrase, bool sandbox )
    {
        _api_key = api_key;
        _secret_key = secret_key;
        _passphrase = passphrase;
        _endpoint_url = sandbox ? "https://api.pro.coinbase.com" : "https://api-public.sandbox.pro.coinbase.com";
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

            // set the access key 
            std::string access_key = "CB-ACCESS-KEY: " + _api_key;
            headers = curl_slist_append( headers, access_key.c_str() );
            res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // prepare the message contents to be encoded 
            std::string _body = "";
            std::string _method = "GET";
            std::string _request_path = _endpoint_url + request;
            std::string _timestamp = get_server_time();
            std::string _message = _timestamp + _method + request + _body ;

            SDL_Log("%s", _message.c_str() );

            // base 64 decode the secret key 
            // const std::vector<unsigned char> decoded_key = Base64Decode( _secret_key.data() );
            // char *hmac_key = _secret_key.data();
            // int key_len = strlen(hmac_key);

            // const auto *data = (const unsigned char *) strdup( _message.c_str() );
            // int data_len = strlen((char *) hmac_key);

            // base64 decode the CoinbasePro secret key
            const auto pl = 3 * _secret_key.length() / 4;
            auto hmac_key = reinterpret_cast<unsigned char *>( calloc( pl + 1, 1 ) );
            const auto hmac_output_length = EVP_DecodeBlock(hmac_key, reinterpret_cast<const unsigned char *>( _secret_key.c_str() ), _secret_key.length() );
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

            // const char *pre_encode_signature_c = strdup( reinterpret_cast<const char *>( _hash.data() ));
            // const std::vector<unsigned char> binary(pre_encode_signature_c, pre_encode_signature_c+strlen(pre_encode_signature_c));
            // std::string post_encode_signature = Base64Encode( binary );

            const auto pl2 = 4 * ( ( _hash_length + 2 ) / 3);
            auto b64_encode = reinterpret_cast<char*>( calloc( pl2 + 1, 1 ) );
            const auto ol2 = EVP_EncodeBlock( reinterpret_cast<unsigned char *>(b64_encode), _hash.data(), _hash_length );
            std::string post_encode_signature = boost::lexical_cast<std::string>(b64_encode, ol2);
            free( b64_encode );

            boost::replace_all(post_encode_signature, "\n", "");

            // add the signature to the header
            std::string access_sign = "CB-ACCESS-SIGN: " + post_encode_signature;
            headers = curl_slist_append( headers, access_sign.c_str() );
            res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // add the timestamp to the header
            std::string timestamp = "CB-ACCESS-TIMESTAMP: " + _timestamp;
            headers = curl_slist_append( headers, timestamp.c_str() );
            res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // add the passphrase to the header
            std::string passphrase = "CB-ACCESS-PASSPHRASE: " + _passphrase;
            headers = curl_slist_append( headers, passphrase.c_str() );
            res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // specify the user agent
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "CoinbaseProAPI");

            fp = fopen( filename.c_str(), "wb" );

            curl_easy_setopt(curl, CURLOPT_URL, _request_path.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

            res = curl_easy_perform(curl);

            // free( hmac_key );
            // free( (char *) data );
            // free( (char *) pre_encode_signature_c );

            curl_easy_cleanup(curl);
            fclose(fp);
        }
    }

    void Client::send_request( std::string filename, std::string request, std::string method, std::string body )
    {

    }
}