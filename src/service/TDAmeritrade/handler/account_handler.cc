#include <google/protobuf/message.h>
#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include <string>

#include "service/TDAmeritrade/proto/account.grpc.pb.h"
#include "service/TDAmeritrade/proto/account.pb.h"

namespace premia {
namespace tda {

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;

class AccountServiceImpl final : public Account::Service {
 public:
  Status GetAccount(const AccountRequest* request,
                    AccountResponse* reply) override {
    // Call the curl code to get the account and parse it into an object for the
    // gui to process
    std::string prefix("Hello again ");

    return Status::OK;
  }

  Status GetAccounts(const AccountRequest* request,
                     AccountsResponse* response) override {
    std::string prefix("Hello again ");
    return Status::OK;
  }
};

void test_function() { AccountServiceImpl account_service; }

}  // namespace tda
}  // namespace premia