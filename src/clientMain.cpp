#include <iostream>
#include <memory>
#include <string>
#include <cstring>
#include <getopt.h>
#include "rpcClient.h"
#include "ddb/integration.hpp"

char* ipAndPort;           // includes port number.
char* inputText;
bool resetCounter = false;
bool shutdownServer = false;

// DDB: options
bool ddb = false;
char* ddb_host_ip = (char*)"127.0.0.1";
char* ddb_proc_alias = (char*)"wc_client";

void parse_args(int argc, char** argv) {
  static struct option long_options[] = {
      {"dest", required_argument, NULL, 'i'},
      {"reset", no_argument, NULL, 'r'},
      {"shutdown", no_argument, NULL, 's'},
      {"text", required_argument, NULL, 't'},
      // DDB: options
      {"ddb", no_argument, NULL, 0},
      {"ddb_host_ip", required_argument, NULL, 0},
      {"ddb_proc_alias", required_argument, NULL, 0},
      {NULL, 0, NULL, 0}
  };

  // loop over all of the options
  signed char ch;
  int option_index = 0;
  while ((ch = getopt_long(argc, argv, "t:a:", long_options, &option_index)) != -1) {
    switch (ch) {
      case 0:
        // DDB: options
        // Long-only options
        if (strcmp(long_options[option_index].name, "ddb") == 0) {
          ddb = true;
        } else if (strcmp(long_options[option_index].name, "ddb_host_ip") == 0) {
          ddb_host_ip = optarg;
        } else if (strcmp(long_options[option_index].name, "ddb_proc_alias") == 0) {
          ddb_proc_alias = optarg;
        }
        break;
      case 'i':
        ipAndPort = optarg;
        break;
      case 'r':
        resetCounter = true;
        break;
      case 's':
        shutdownServer = true;
        break;
      case 't':
        inputText = optarg;
        break;
      default:
        printf("?? getopt returned character code 0%o ??\n", ch);
    }
  }
}

int main(int argc, char** argv) {
  parse_args(argc, argv);

  // DDB: initialization
  if (ddb) {
    auto cfg = DDB::Config::get_default(ddb_host_ip)
                   .with_alias(ddb_proc_alias)
                   .with_hash(ddb_proc_alias);
    auto connector = DDB::DDBConnector(cfg);
    connector.init();
  }

  auto channel = grpc::CreateChannel(ipAndPort, grpc::InsecureChannelCredentials());
  RpcClient client(channel);

  if (resetCounter) {
   client.ResetCounter();
    std::cout << "Reset invoked." << std::endl;
  } else if (shutdownServer) {
    client.Shutdown();
    std::cout << "Shutdown requested." << std::endl;
  } else {
    // TODO (Milestone3): modify below to use just one RPC.
    /*
    int wc = client.AddWordCount(inputText);
    int wcSum = client.GetAllWordCount();
    */
    auto [wc, wcSum] = client.AddWordCount(inputText);
    std::cout << "Word count: " << wc << std::endl
              << "Sum of all word counts: " << wcSum << std::endl;
  }

  return 0;
}
