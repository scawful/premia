#ifndef PREMIA_API_HTTP_SERVER_H
#define PREMIA_API_HTTP_SERVER_H

#include <string>

namespace premia::api::http {

auto RunHttpServer(const std::string& host, unsigned short port) -> void;

}  // namespace premia::api::http

#endif  // PREMIA_API_HTTP_SERVER_H
