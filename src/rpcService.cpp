#include "rpcService.h"
#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>
#include <memory>
#include <string>
#include <thread>
#include <chrono>

#include <grpcpp/grpcpp.h>
#include "accumulator.grpc.pb.h"
#include "accumulator.pb.h"

#define UNUSED(expr) (void)(expr)

using grpc::Server;
using grpc::ServerContext;
using grpc::Status;

std::unique_ptr<std::thread> shutdown_thread;

void
AccumulatorServiceImpl::setGrpcServer(grpc::Server* serverPtr) {
  grpcServerPtr = serverPtr;
}


/**
 * Helper function for counting words in a string.
 * Use it for implementing AccumulatorServiceImpl::AddWordCount.
 */
int
AccumulatorServiceImpl::countWords(const std::string& text) {
  std::stringstream stream(text);
  std::string oneWord;
  int wc = 0;
  while (stream >> oneWord) {
    ++wc;
  }
  return wc;
}

//////////////////////////////////////////////////////////////////
// RPC service implementations below
//////////////////////////////////////////////////////////////////

Status
AccumulatorServiceImpl::AddWordCount(ServerContext* context,
    const AddWordCountRequest* request,
    AddWordCountReply* reply) {
  UNUSED(context);

  // TODO (Milestone1): implement
  // You may use AccumulatorServiceImpl::countWords().
  // Use wcSum variable to keep track the accumulated word counts.
  int newly_added = countWords(request->text());
  this->wcSum += newly_added;
  reply->set_word_count(newly_added);
  reply->set_cummulative_count(this->wcSum);
  return Status::OK;
}

Status
AccumulatorServiceImpl::GetAllWordCount(ServerContext* context,
    const Empty* request,
    GetAllWordCountReply* reply) {
  UNUSED(context);
  UNUSED(request);

  // TODO (Milestone1): implement
  reply->set_cummulative_count(this->wcSum);
  return Status::OK;
}

// TODO (Milestone2): implement ResetCounter handler
Status
AccumulatorServiceImpl::ResetCounter(ServerContext* context,
    const Empty* request,
    StandardReply* reply) {
  UNUSED(context);
  UNUSED(request);
  
  std::string replyMsg("Reset Counter invoked.");
  reply->set_message(replyMsg);

  this->wcSum = 0;
  return Status::OK;
}

Status
AccumulatorServiceImpl::Shutdown(ServerContext* context, const Empty* request,
    StandardReply* reply) {
  UNUSED(context);
  UNUSED(request);

  std::string replyMsg("Shutdown invoked.");
  reply->set_message(replyMsg);

  // Shutdown() must be called from another thread. So, we create a new thread.
  // Don't worry about this complex threading for other RPCs.
  auto shutdownFn = [&](Server* serverPtr) {
    std::cout << "grpcServer shutdown in 3 sec." << std::endl << std::flush;
    std::this_thread::sleep_for (std::chrono::seconds(3));
    serverPtr->Shutdown(); //Wait();
    std::cout << "grpcServer shutdown." << std::endl << std::flush;
  };
  shutdown_thread = std::make_unique<std::thread>(shutdownFn, grpcServerPtr);

  return Status::OK;
}
