#include "rpcClient.h"
#include <grpcpp/client_context.h>
#include <memory>
#include <string>
#include <utility>

#include <grpcpp/grpcpp.h>
#include "accumulator.grpc.pb.h"
#include "accumulator.pb.h"

std::pair<int, int>
RpcClient::AddWordCount(std::string text) {
  AddWordCountRequest request;
  request.set_text(text);
  // TODO (Milestone1): implement rest.
  grpc::ClientContext context;
  AddWordCountReply reply;
  stub_->AddWordCount(&context, request, &reply);
  return {reply.word_count(), reply.cummulative_count()};
}

int
RpcClient::GetAllWordCount() {
  // TODO (Milestone1): implement
  Empty request;
  grpc::ClientContext context;
  GetAllWordCountReply reply;
  stub_->GetAllWordCount(&context, request, &reply);
  return reply.cummulative_count();
}

std::string
RpcClient::ResetCounter() {
  // TODO: implement.
  Empty request;
  grpc::ClientContext context;
  StandardReply reply;
  stub_->ResetCounter(&context, request, &reply);
  return reply.message();
}

std::string
RpcClient::Shutdown() {
  Empty request;
  grpc::ClientContext context;
  StandardReply reply;
  Status status = stub_->Shutdown(&context, request, &reply);
  if (status.ok()) {
    return reply.message();
  } else {
    std::cerr << "Failed to request shutdown. code: "<< status.error_code()
              <<  " msg: %s." << status.error_message().c_str();
    return "Failed to request shutdown.";
  }
}
