#define SDL_MAIN_HANDLED
#include <gtest/gtest.h>

namespace premiaTests {
// namespace MetatypeTests {
// TEST(TemplateTests, run_once_template) {
//   // Arrangement
//   Tuple<int> my_tuple = std::make_tuple<int>(0);
//   Tuple<int> fst_state = my_tuple, snd_state, thr_state;
//   auto f = [&my_tuple, &thr_state]() {
//     my_tuple = std::make_tuple<int>(1);
//     thr_state = my_tuple;
//   };
//   auto f_once = [&my_tuple, &snd_state, &thr_state]() {
//     my_tuple = std::make_tuple<int>(2);
//     snd_state = my_tuple;
//     thr_state = my_tuple;
//   };

//   // Action
//   runOnce<>(f, f_once);
//   runOnce<>(f, f_once);

//   // Assertion
//   ASSERT_EQ(std::get<0>(fst_state), 0);
//   ASSERT_EQ(std::get<0>(snd_state), 2);
//   ASSERT_EQ(std::get<0>(thr_state), 1);
// }

// TEST(TemplateTests, run_once_with_args_template) {
//   // Arrangement
//   String s = "";
//   String p = "f_once,";
//   auto f = [&s]() { s += "f,"; };
//   auto f_once = [&s](String e) { s += e; };

//   // Action
//   runOnceArgs<>(f, f_once, p);
//   runOnceArgs<>(f, f_once, p);

//   // Assertion
//   ASSERT_EQ("f,f_once,f,", s);
// }
// }  // namespace MetatypeTests
}  // namespace premiaTests

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}