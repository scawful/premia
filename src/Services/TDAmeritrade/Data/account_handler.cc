#include <google/protobuf/message.h>

#include "account_service.pb.h"

namespace premia {
namespace tda {

class AccountServiceImpl final : public Account::Service {
  Status GetAccount(ServerContext* context, const AccountRequest* request,
                    AccountResponse* reply) override {
    // Call the curl code to get the account and parse it into an object for the
    // gui to process
  }

  Status GetAccount(ServerContext* context, const AccountRequest* request,
                    AccountsResponse* response) override {}
};

}  // namespace tda
}  // namespace premia