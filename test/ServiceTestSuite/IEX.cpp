#include "singletons/IEX.hpp"

#include <gtest/gtest.h>

namespace ServiceTestSuite {
namespace IEX {
TEST(IEXTest, basic_test) { premia::iex::Client client; }
}  // namespace IEX
}  // namespace ServiceTestSuite