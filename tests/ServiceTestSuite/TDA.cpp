#include <gtest/gtest.h>
#include "Services/TDA.hpp"

namespace ServiceTestSuite
{
    namespace TDA
    {
        TEST(TDATest, client_has_auth_token)
        {
            tda::Client client;

            EXPECT_EQ(false, client.is_session_logged_in());
        }
    }
}