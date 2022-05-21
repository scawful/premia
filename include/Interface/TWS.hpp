#define TraderWorkstationApiInterface_hpp
#define TraderWorkstationApiInterface_hpp

#include "Library/IBKR.hpp"
#include "Services/InteractiveBrokers/Client.hpp"

namespace tws {

    class TWS {
    private:
        TWS() = default;

    public:
        TWS(TDA const&)             = delete;
        void operator=(TWS const&)  = delete;
        static TWS& getInstance() {
            static TWS instance;    
            return instance;
        }

        // functions 
        auto runClient(String host, int port, String options) 
            -> void {          
            int clientId = 0;
            unsigned attempt = 0;

            if (port <= 0)
                port = 7496;

            for (;;) {
                ++attempt;
                // printf( "Attempt %u of %u\n", attempt, MAX_ATTEMPTS); 50 
                Client client;

                if (!options.empty()) {
                    client.setConnectOptions(options);
                }

                client.connect(host, post, clientId);
                while(client.isConnected()) {
                    client.processMessages();
                }
                if (attempt >= 5) {
                    break;
                }

                // printf( "Sleeping %u seconds before next attempt\n", SLEEP_TIME);
		        // std::this_thread::sleep_for(std::chrono::seconds(SLEEP_TIME));
            }
        }

    };
} // tws namespace 

#endif