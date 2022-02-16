#ifndef Client_hpp
#define Client_hpp

#include "../../core.hpp"

namespace tda 
{
    namespace JSONObject = boost::property_tree;
    
    class Client 
    {
    private:
        std::string api_key;
        std::string refresh_token;
        std::string access_token;

        JSONObject::ptree user_principals;

        void get_user_principals();
        void post_access_token();
        void post_account_authorization();

        size_t json_write_callback(void *contents, size_t size, size_t nmemb, std::string *s);

    public:
        Client();

        JSONObject send_request(std::string endpoint);
        JSONObject send_authorized_request(std::string endpoint);

        void start_session();
        void send_session_request();
        void send_interrupt_signal();

    };
}

#endif