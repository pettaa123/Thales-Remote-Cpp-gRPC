# CMakeList.txt : CMake project for Thales-Remote-Cpp-gRPC-cMake, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.30.3)
message("CMake version: ${CMAKE_VERSION}")

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Thales-gRPC-Server" C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)



set(ZLIB_LIBRARY "D:/vcpkg/installed/x64-windows/lib/zlib.lib")
set(ZLIB_INCLUDE_DIR "D:/vcpkg/installed/x64-windows/include")

set(OPENSSL_ROOT_DIR "D:/vcpkg/installed/x64-windows")
set(OPENSSL_INCLUDE_DIR "D:/vcpkg/installed/x64-windows/include")
find_package(OpenSSL CONFIG REQUIRED)

find_package(ZLIB REQUIRED)


find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# Define the path to your proto files
file(GLOB PROTO_FILES "../protos/*.proto")


add_library(proto-objects OBJECT "${CMAKE_CURRENT_LIST_DIR}/../protos/zahner.proto")

target_link_libraries(proto-objects PUBLIC protobuf::libprotobuf gRPC::grpc++_unsecure)

set(PROTO_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(PROTO_IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../protos")

protobuf_generate(
    TARGET proto-objects
    OUT_VAR PROTO_GENERATED_FILES
    IMPORT_DIRS ${PROTO_IMPORT_DIRS}
    PROTOC_OUT_DIR "${PROTO_BINARY_DIR}")
set_source_files_properties(${PROTO_GENERATED_FILES} PROPERTIES SKIP_UNITY_BUILD_INCLUSION on)

protobuf_generate(
    TARGET proto-objects
    OUT_VAR PROTO_GENERATED_FILES
    LANGUAGE grpc
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
    PLUGIN "protoc-gen-grpc=\$<TARGET_FILE:gRPC::grpc_cpp_plugin>"
    # PLUGIN_OPTIONS "generate_mock_code=true"
    IMPORT_DIRS ${PROTO_IMPORT_DIRS}
    PROTOC_OUT_DIR "${PROTO_BINARY_DIR}")
set_source_files_properties(${PROTO_GENERATED_FILES} PROPERTIES SKIP_UNITY_BUILD_INCLUSION on)

target_include_directories(proto-objects PUBLIC "$<BUILD_INTERFACE:${PROTO_BINARY_DIR}>")


