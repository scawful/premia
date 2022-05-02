#define SDL_MAIN_HANDLED
#include <gtest/gtest.h>
#include "Metatypes.hpp"

namespace PremiaTests
{
    namespace MetatypeTests
    {
        TEST(TemplateTests, run_once_template)
        {
            // Arrangement
            Tuple<int> my_tuple = std::make_tuple<int>(0);
            Tuple<int> fst_state = my_tuple, snd_state, thr_state;
            auto f = [&my_tuple, &thr_state] () { 
                my_tuple = std::make_tuple<int>(1); 
                thr_state = my_tuple;
            };
            auto f_once = [&my_tuple, &snd_state, &thr_state] () { 
                my_tuple = std::make_tuple<int>(2); 
                snd_state = my_tuple;
                thr_state = my_tuple;
            };
            
            // Action
            runOnce<>(f, f_once);
            runOnce<>(f, f_once);

            // Assertion
            ASSERT_EQ(std::get<0>(fst_state), 0);
            ASSERT_EQ(std::get<0>(snd_state), 2);
            ASSERT_EQ(std::get<0>(thr_state), 1);
        }
    }
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}