#ifndef UserPrincipals_hpp
#define UserPrincipals_hpp

#include "../../../core.hpp"

namespace tda
{
    class UserPrincipals
    {
    private:
        std::unordered_map<std::string, std::string> account_data;
        std::unordered_map<std::string, std::string> streamer_info;

    public:
        UserPrincipals();
    
        void set_account_data(std::unordered_map<std::string, std::string> data);
        void set_streamer_info(std::unordered_map<std::string, std::string> data);

    };
}

#endif