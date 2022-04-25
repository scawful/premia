#ifndef IEXClient_hpp
#define IEXClient_hpp

#include "core.hpp"
#include "apikey.hpp"

namespace iex 
{
    class Client 
    {
    private:
        bool sandbox_mode;
        std::string base_endpoint;
        std::string sandbox_endpoint;
        std::string token_parameter;

        std::string current_endpoint();
        static size_t json_write_callback(void *contents, size_t size, size_t nmemb, std::string *s);

    public:
        Client();

        std::string send_request(std::string endpoint);
        std::string send_authorized_request(std::string endpoint);

        std::string get_fund_ownership(std::string symbol);
        std::string get_insider_transactions(std::string symbol);
    };
}

#endif