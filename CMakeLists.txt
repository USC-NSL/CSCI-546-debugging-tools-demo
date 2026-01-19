cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(Accumulator)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS} -std=c++20 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# This depends on how the setup script install the grpc
set(grpc_prefix "$ENV{HOME}/.local")

set(CMAKE_PREFIX_PATH ${grpc_prefix} CACHE PATH "libs prefix path")

# includes grpc
include(cmake/common.cmake)

# Proto file
get_filename_component(dp_proto "protos/accumulator.proto" ABSOLUTE)
get_filename_component(dp_proto_path "${dp_proto}" PATH)

# Generated sources
set(dp_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/accumulator.pb.cc")
set(dp_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/accumulator.pb.h")
set(dp_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/accumulator.grpc.pb.cc")
set(dp_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/accumulator.grpc.pb.h")
add_custom_command(
      OUTPUT "${dp_proto_srcs}" "${dp_proto_hdrs}" "${dp_grpc_srcs}" "${dp_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${dp_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${dp_proto}"
      DEPENDS "${dp_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Include DDB headers
include_directories("${CMAKE_PREFIX_PATH}/include")

# dp_grpc_proto
add_library(dp_grpc_proto
  ${dp_grpc_srcs}
  ${dp_grpc_hdrs}
  ${dp_proto_srcs}
  ${dp_proto_hdrs})
target_link_libraries(dp_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
  paho-mqtt3c
)

# Linking info for runtime.
add_executable(wcClient
  "src/clientMain.cpp" "src/rpcClient.cpp")
target_link_libraries(wcClient
  dp_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
  ${TORCH_LIBRARIES})

add_executable(wcServer
  "src/serverMain.cpp" "src/rpcService.cpp")
target_link_libraries(wcServer
  dp_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
  ${TORCH_LIBRARIES})


set_property(TARGET wcClient wcServer PROPERTY CXX_STANDARD 20)
