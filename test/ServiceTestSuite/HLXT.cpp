#include "singletons/HLXT.hpp"

#include <gtest/gtest.h>

namespace ServiceTestSuite {
namespace HLXT {
TEST(CBPTest, basic_test) { premia::halext::Client client; }
}  // namespace HLXT
}  // namespace ServiceTestSuite