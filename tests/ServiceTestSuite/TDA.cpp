#include <gtest/gtest.h>
#include "Interface/TDA.hpp"

namespace ServiceTestSuite
{
    namespace TDA
    {
        TEST(TDATest, websocket_session_test)
        {
            tda::Client client;
            String consumer_key;
            String refresh_token;
            ConsoleLogger logger;
            std::ifstream keyfile("../assets/apikey.txt");
            if (keyfile.good()) {
                std::stringstream buffer;
                buffer << keyfile.rdbuf();
                keyfile.close();
                buffer >> consumer_key;
                buffer >> refresh_token;
            } else {
                std::cout << "noooo" << std::endl;
            }
            client.addAuth(consumer_key, refresh_token);
            client.fetch_access_token();
            client.start_session(logger, "QQQ");
        }
    }
}