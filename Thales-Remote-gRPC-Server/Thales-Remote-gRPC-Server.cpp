#include <iostream>
#include <grpcpp/grpcpp.h>
#include "zahnerzenniumserviceimpl.h"
#include "thalesfileserviceimpl.h"



void RunServer(std::string &server_address) {

    ConnectionManager connectionManager;

    ZahnerZenniumServiceImpl service(connectionManager);

    thalesfile::ConnectionManager TfConnectionManager;

    ThalesFileServiceImpl fileService(TfConnectionManager);

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    builder.RegisterService(&fileService);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char* argv[])
{
    std::string server_address;
    if (argc >= 2) {
        server_address = argv[1];
    }
    else {
        server_address = "localhost:50051";
    }
    std::cout << "Starting gRPC server on " << server_address << std::endl;
    RunServer(server_address);
    return 0;
}
