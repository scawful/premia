#include <google/protobuf/message.h>
#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include "service/TDAmeritrade/proto/option_chain.grpc.pb.h"
#include "service/TDAmeritrade/proto/option_chain.pb.h"

namespace premia {
namespace tda {

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;

class OptionChainServiceImpl final : public OptionChain::Service {
  Status GetOptionChain(const OptionChainRequest* request,
                        OptionChainResponse* reply) override {
    // Call the curl code to get the OptionChain and parse it into an object for
    // the gui to process
  }

  Status GetOptionChain(const OptionChainRequest* request,
                        OptionChainsResponse* response) override {}
};

}  // namespace tda
}  // namespace premia