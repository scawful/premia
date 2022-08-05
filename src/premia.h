#ifndef Premia_hpp
#define Premia_hpp

#include "metatypes.h"

namespace premia {

/**
 * @exception exception-object exception description
 */
class FatalException : public std::exception {};

class NotLoggedInException : public std::exception {};

/**
 * @exception Parser Exceptions
 */
class ParserException : public std::exception {};

class TDAParserException : public ParserException {};

/**
 * @exception Client Exceptions
 */
class ClientException : public std::exception {};

class TDAClientException : public ClientException {
 private:
  const char* msg_src;
  const char* info_ex;

 public:
  TDAClientException(const char* msg, const char* info)
      : msg_src(msg), info_ex(info) {}

  const char* what() const noexcept override { return msg_src; }
  const char* info() const { return info_ex; };
};

}  // namespace premia

#endif