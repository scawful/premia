#ifndef Client_hpp
#define Client_hpp

#include "../../core.hpp"
#include "../../apikey.hpp"
#include "Parser.hpp"
#include "Session.hpp"
#include "Data/UserPrincipals.hpp"

namespace JSONObject = boost::property_tree;

namespace tda 
{

    class Client 
    {
    private:
        // API Strings 
        std::string api_key;
        std::string refresh_token;
        std::string access_token;

        // API Data 
        Parser parser;
        UserPrincipals user_principals;

        // WebSocket session variables 
        boost::asio::io_context ioc;
        std::shared_ptr<tda::Session> websocket_session;
        std::shared_ptr<std::vector<std::string>> websocket_buffer;

        // API Functions 
        void get_user_principals();
        std::string post_access_token();

        bool string_replace(std::string& str, const std::string from, const std::string to);
        static size_t json_write_callback(void *contents, size_t size, size_t nmemb, std::string *s);

    public:
        Client();

        std::string send_request(std::string endpoint);
        std::string send_authorized_request(std::string endpoint);

        std::string get_price_history(std::string endpoint);
        std::string get_quote(std::string symbol);
        std::string get_account(std::string account_id);


        void start_session();
        void send_session_request(std::string request);
        void send_interrupt_signal();
        void fetch_access_token();
    };
}

#endif