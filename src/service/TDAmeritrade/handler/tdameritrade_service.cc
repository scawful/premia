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

absl::StatusOr<std::string> SendRequest(const std::string& endpoint) {
  CURL* curl;
  CURLcode res;
  std::string response;

  curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, JSONWrite);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "premia-agent/1.0");
  curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
  res = curl_easy_perform(curl);

  if (res != CURLE_OK) return absl::InternalError(curl_easy_strerror(res));
  curl_easy_cleanup(curl);

  return response;
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

Status TDAmeritradeServiceImpl::PostAccessToken(
    grpc::ServerContext* context, const AccessTokenRequest* request,
    AccessTokenResponse* reply) {
  std::cout << "PostAccessToken: ";
  CURL* curl;
  CURLcode res;
  CURLHeader headers = nullptr;
  std::string response;

  curl = curl_easy_init();

  curl_easy_setopt(curl, CURLOPT_URL,
                   "https://api.tdameritrade.com/v1/oauth2/token");
  curl_easy_setopt(curl, CURLOPT_HTTPPOST, true);
  headers = curl_slist_append(
      headers, "Content-Type: application/x-www-form-urlencoded");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  CURLHeader chunk = nullptr;  // chunked request for http1.1/200 ok
  chunk = curl_slist_append(chunk, "Transfer-Encoding: chunked");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

  // specify post data, have to url encode the refresh token
  auto& refresh_token = request->refresh_token();
  auto& api_key = request->client_id();
  std::string easy_escape = curl_easy_escape(
      curl, refresh_token.c_str(), static_cast<int>(refresh_token.length()));
  std::string data_post =
      "grant_type=refresh_token&refresh_token=" + easy_escape +
      "&client_id=" + api_key;

  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data_post.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data_post.length());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, JSONWrite);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "premia-agent/1.0");
  curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

  // run the operations
  res = curl_easy_perform(curl);
  curl_easy_cleanup(curl);

  JsonParseOptions options;
  options.ignore_unknown_fields = true;
  JsonStringToMessage(response, reply, options);

  access_token_ = reply->access_token();
  std::cout << access_token_ << std::endl;
  return Status::OK;
}

Status TDAmeritradeServiceImpl::GetUserPrincipals(
    grpc::ServerContext* context, const UserPrincipalsRequest* request,
    UserPrincipalsResponse* reply) {
  std::cerr << "User Principals " << std::endl;
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

Status TDAmeritradeServiceImpl::GetPriceHistory(
    grpc::ServerContext* context, const PriceHistoryRequest* request,
    PriceHistoryResponse* reply) {
  std::string url =
      "https://api.tdameritrade.com/v1/marketdata/{ticker}/"
      "pricehistory?apikey=" +
      client_id_ +
      "&periodType={periodType}&period={period}&frequencyType={frequencyType}&"
      "frequency={frequency}&needExtendedHoursData={ext}";

  StringReplace(url, "{ticker}", request->ticker());
  StringReplace(url, "{periodType}", request->periodtype());
  StringReplace(url, "{period}", request->period());
  StringReplace(url, "{frequencyType}", request->frequencytype());
  StringReplace(url, "{frequency}", request->frequency());

  if (!request->needextendedhoursdata())
    StringReplace(url, "{ext}", "false");
  else
    StringReplace(url, "{ext}", "true");

  auto response = SendRequest(url);
  if (!response.ok()) {
    // some error
  }
  auto json = std::move(*response);
  JsonParseOptions options;
  options.ignore_unknown_fields = true;
  JsonStringToMessage(json, reply, options);
  return Status::OK;
}

Status TDAmeritradeServiceImpl::GetOptionChain(
    grpc::ServerContext* context, const OptionChainRequest* request,
    OptionChainResponse* reply) {
  std::string url =
      "https://api.tdameritrade.com/v1/marketdata/chains?apikey=" + client_id_ +
      "&symbol={ticker}&contractType={contractType}&strikeCount={strikeCount}&"
      "includeQuotes={includeQuotes}&strategy={strategy}&range={range}&"
      "expMonth={expMonth}&optionType={optionType}";

  StringReplace(url, "{ticker}", request->symbol());
  StringReplace(url, "{contractType}", request->contracttype());
  StringReplace(url, "{strikeCount}", request->strikecount());
  StringReplace(url, "{strategy}", request->strategy());
  StringReplace(url, "{range}", request->range());
  StringReplace(url, "{expMonth}", request->expmonth());
  StringReplace(url, "{optionType}", request->optiontype());

  if (!request->includequotes())
    StringReplace(url, "{includeQuotes}", "FALSE");
  else
    StringReplace(url, "{includeQuotes}", "TRUE");

  auto response = SendRequest(url);
  if (!response.ok()) {
    // some error
  }
  auto json = std::move(*response);
  JsonParseOptions options;
  options.ignore_unknown_fields = true;
  JsonStringToMessage(json, reply, options);
  return Status::OK;
}

}  // namespace tda
}  // namespace premia