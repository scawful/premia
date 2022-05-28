#include "Library/Fred.hpp"

#include <gtest/gtest.h>

namespace ServiceTestSuite {
namespace FredCppTests {
using namespace fredcpp;

Api api;
ApiResponse response;

TEST(STLFred, basic_test) {
  api.withLogger(fredcpp::external::SimpleLogger::getInstance())
      .withExecutor(fredcpp::external::CurlHttpClient::getInstance())
      .withParser(fredcpp::external::PugiXmlParser::getInstance());

  // set your FRED API key
  std::string yourApiKey = "<api-key>";

  api.withKey(yourApiKey);

  // 20yr constant matury yield 10 responses with desc
  api.get(
      ApiRequestBuilder::SeriesObservations("DGS20").withSort("desc").withLimit(
          "10"),
      response);

  if (!response.good()) {
    FREDCPP_LOG_ERROR(response.error);
  } else {
    FREDCPP_LOG_INFO(response);
  }
}
}  // namespace FredCppTests
}  // namespace ServiceTestSuite