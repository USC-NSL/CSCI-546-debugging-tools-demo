#ifndef RPC_SERVICE_H
#define RPC_SERVICE_H

#include <memory>
#include <string>
#include <thread>

#include <grpcpp/grpcpp.h>
#include "accumulator.grpc.pb.h"

using grpc::Server;
using grpc::ServerContext;
using grpc::Status;

extern std::unique_ptr<std::thread> shutdown_thread;

/**
 * GRPC service implementation for runtime.
 */
class AccumulatorServiceImpl final : public Accumulator::Service {
 public:
  AccumulatorServiceImpl()
    : Accumulator::Service() {}

  void setGrpcServer(grpc::Server* serverPtr);
 
 private:
  Status AddWordCount(ServerContext* context,
                      const AddWordCountRequest* request,
                      AddWordCountReply* reply) override;
  Status GetAllWordCount(ServerContext* context,
                      const Empty* request,
                      GetAllWordCountReply* reply) override;
  Status ResetCounter(ServerContext* context,
                      const Empty* request,
                      StandardReply* reply) override;
  Status Shutdown(ServerContext* context, const Empty* request,
                  StandardReply* reply) override;
  //TODO (Milestone2): define ResetCounter.

  int countWords(const std::string& text);

  // Pointer to grpc server instance. Used for shutdown.
  grpc::Server* grpcServerPtr;

  // Counter for accumulating all word counts.
  int wcSum = 0;
};

#endif // RPC_SERVICE_H