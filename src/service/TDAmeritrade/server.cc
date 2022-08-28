#include <string>

#include "handler/tdameritrade_service.h"

namespace premia {
namespace tda {

using grpc::Server;
using grpc::ServerBuilder;

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  TDAmeritradeServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}

}  // namespace tda
}  // namespace premia

int main() { premia::tda::RunServer(); }