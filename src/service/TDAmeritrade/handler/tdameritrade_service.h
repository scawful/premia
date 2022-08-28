#ifndef PREMIA_SERVICE_TDAMERITRADE_HANDLER_SERVICE
#define PREMIA_SERVICE_TDAMERITRADE_HANDLER_SERVICE

#include <curl/curl.h>
#include <google/protobuf/message.h>
#include <grpc/support/log.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/ssl.h>

#include <string>

#include "absl/status/statusor.h"
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

class TDAmeritradeServiceImpl final : public ::TDAmeritrade::Service {
 public:
  Status GetUserPrincipals(grpc::ServerContext* context,
                           const UserPrincipalsRequest* request,
                           UserPrincipalsResponse* reply) override;

  // Account Services
  Status GetAccount(grpc::ServerContext* context, const AccountRequest* request,
                    AccountResponse* reply) override;

  Status GetAccounts(grpc::ServerContext* context,
                     const AccountRequest* request,
                     AccountsResponse* response) override;

  // Option Chain Services
  Status GetOptionChain(grpc::ServerContext* context,
                        const OptionChainRequest* request,
                        OptionChainResponse* reply) override;

 private:
  std::string access_token_;
};

void RunAccountServer();

}  // namespace tda
}  // namespace premia

#endif