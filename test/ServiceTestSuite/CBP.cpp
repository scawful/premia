#include "singletons/CBP.hpp"

#include <gtest/gtest.h>

namespace ServiceTestSuite {
namespace CBP {
TEST(CBPTest, basic_test) { premia::cbp::Client client; }
}  // namespace CBP
}  // namespace ServiceTestSuite