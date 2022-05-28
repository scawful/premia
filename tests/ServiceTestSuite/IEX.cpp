#include "Interface/IEX.hpp"

#include <gtest/gtest.h>

namespace ServiceTestSuite {
namespace IEX {
TEST(IEXTest, basic_test) { Premia::iex::Client client; }
}  // namespace IEX
}  // namespace ServiceTestSuite