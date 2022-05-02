#ifndef IEXClient_hpp
#define IEXClient_hpp

#include "Metatypes.hpp"
#include "Boost.hpp"
#include "Curl.hpp"

namespace iex 
{
    class Client 
    {
    private:
        bool sandbox_mode;
        String api_key;
        String base_endpoint;
        String sandbox_endpoint;
        String token_parameter;

        String current_endpoint();
        static size_t json_write_callback(void *contents, size_t size, size_t nmemb, String *s);

    public:
        Client();

        String send_request(String endpoint);
        String send_authorized_request(String endpoint);

        String get_fund_ownership(String symbol);
        String get_insider_transactions(String symbol);
    };
}

#endif