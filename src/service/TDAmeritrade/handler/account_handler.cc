#include <google/protobuf/message.h>

#include "Services/TDAmeritrade/account.pb.h"

namespace premia {
namespace tda {

class AccountServiceImpl final : public Account::Service {
  Status GetAccount(const AccountRequest* request,
                    AccountResponse* reply) override {
    // Call the curl code to get the account and parse it into an object for the
    // gui to process
  }

  Status GetAccount(const AccountRequest* request,
                    AccountsResponse* response) override {}
};

void test_function() { AccountServiceImpl account_service; }

}  // namespace tda
}  // namespace premia