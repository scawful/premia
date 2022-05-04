#ifndef HXLT_hpp
#define HLXT_hpp

#include "../src/Services/Halext/Client.hpp"
#include "../src/Services/Halext/User.hpp"

namespace halext 
{
    class HLXT
    {
    private:
        HLXT() { }
        Client client;
        User currentUser;
        bool privateBalance = false;

    public:
        HLXT(HLXT const&)            = delete;
        void operator=(HLXT const&)  = delete;
        static HLXT& getInstance() {
            static HLXT instance;    
            return instance;
        }

        auto getSqueezeMetricsData()
            -> String const {
            return client.send_request("https://squeezemetrics.com/monitor/download/SPX.csv");
        }

        auto setPrivateBalance(bool val)
            -> void {
            privateBalance = val;
        }

        auto getPrivateBalance()
            -> bool {
            return privateBalance;
        }

    };
}

#endif