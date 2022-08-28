#include "tdameritrade_service.h"

#include <curl/curl.h>
#include <google/protobuf/message.h>
#include <google/protobuf/util/json_util.h>
#include <grpc/support/log.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/ssl.h>

#include <iostream>
#include <string>

#include "src/service/TDAmeritrade/proto/tdameritrade.grpc.pb.h"
#include "src/service/TDAmeritrade/proto/tdameritrade.pb.h"

namespace premia {
namespace tda {

using google::protobuf::util::JsonParseOptions;
using google::protobuf::util::JsonStringToMessage;
using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::Status;
using CURLHeader = struct curl_slist*;

namespace {
bool StringReplace(std::string& str, const std::string& from,
                   const std::string& to) {
  size_t start = str.find(from);
  if (start == std::string::npos) return false;

  str.replace(start, from.length(), to);
  return true;
}

size_t JSONWrite(const char* contents, size_t size, size_t nmemb,
                 std::string* s) {
  size_t new_length = size * nmemb;
  try {
    s->append(contents, new_length);
  } catch (const std::bad_alloc& e) {
    // @todo attach a logger
    return EXIT_FAILURE;
  }
  return new_length;
}

absl::StatusOr<std::string> SendAuthorizedRequest(
    const std::string& endpoint, const std::string& access_token) {
  CURL* curl;
  CURLcode res;
  CURLHeader headers = nullptr;
  std::string response;
  std::string auth_bearer = "Authorization: Bearer " + access_token;

  curl = curl_easy_init();
  headers = curl_slist_append(headers, auth_bearer.c_str());

  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, JSONWrite);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "premia-agent/1.1");
  curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

  res = curl_easy_perform(curl);
  if (res != CURLE_OK) return absl::InternalError(curl_easy_strerror(res));

  curl_easy_cleanup(curl);
  return response;
}
}  // namespace

Status TDAmeritradeServiceImpl::GetUserPrincipals(
    grpc::ServerContext* context, const UserPrincipalsRequest* request,
    UserPrincipalsResponse* reply) {
  std::cerr << "GetUserPrincipals" << std::endl;
  std::string endpoint =
      "https://api.tdameritrade.com/v1/"
      "userprincipals?fields=streamerSubscriptionKeys,streamerConnectionInfo";
  auto response = SendAuthorizedRequest(endpoint, access_token_);
  if (!response.ok()) {
    std::cerr << response.value() << std::endl;
  }
  auto json = std::move(*response);
  JsonParseOptions options;
  options.ignore_unknown_fields = true;
  JsonStringToMessage(json, reply, options);
  return Status::OK;
}

Status TDAmeritradeServiceImpl::GetAccount(grpc::ServerContext* context,
                                           const AccountRequest* request,
                                           AccountResponse* reply) {
  // TODO(scawful): Check the user principals before making a request.
  std::string account_url =
      "https://api.tdameritrade.com/v1/accounts/"
      "{accountNum}?fields=positions,orders";
  std::string account_id = request->accountid();
  StringReplace(account_url, "{accountNum}", account_id);
  auto response = SendAuthorizedRequest(account_url, access_token_);
  if (!response.ok()) {
    // some error
  }
  auto json = std::move(*response);
  JsonParseOptions options;
  options.ignore_unknown_fields = true;
  JsonStringToMessage(json, reply, options);
  return Status::OK;
}

Status TDAmeritradeServiceImpl::GetAccounts(grpc::ServerContext* context,
                                            const AccountRequest* request,
                                            AccountsResponse* response) {
  std::string prefix("Multiple Accounts");
  return Status::OK;
}

Status TDAmeritradeServiceImpl::GetOptionChain(
    grpc::ServerContext* context, const OptionChainRequest* request,
    OptionChainResponse* reply) {
  // Call the curl code to get the OptionChain and parse it into an object for
  // the gui to process
  return Status::OK;
}

}  // namespace tda
}  // namespace premia