#pragma once

#include <unordered_map>
#include <memory>
#include <mutex>
#include "thalesremotescriptwrapper.h"
#include "thalesremoteerror.h"
#include "thalesfileinterface.h"

#include "zahner.grpc.pb.h"

using namespace zahner;

namespace thalesfile {

    class ConnectionManager {
    public:
        // Create or retrieve a persistent connection
        std::shared_ptr<ThalesFileInterface> createConnection(const std::string& session_id, const std::string& host) {
            std::lock_guard<std::recursive_mutex> lock(mutex_);  // Locking for sessions_

            if (sessions_.find(session_id) == sessions_.end()) {
                try {
                    auto connection = std::make_shared<ThalesFileInterface>(host);
                    sessions_[session_id] = connection;
                }
                catch (ThalesRemoteError error) {
                    return nullptr; // Connection failed
                }
            }

            return sessions_[session_id];
        }

        // Get an existing connection
        std::shared_ptr<ThalesFileInterface> getConnection(const std::string& session_id) {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            auto it = sessions_.find(session_id);
            return (it != sessions_.end()) ? it->second : nullptr;
        }

        // Remove a session when it's no longer needed
        void removeSession(const std::string& session_id) {
            std::lock_guard<std::recursive_mutex> lock(mutex_);  // Single lock for both maps

            sessions_.erase(session_id);
            if (sessions_.find(session_id) != sessions_.end()) {
                sessions_[session_id]->close();
                sessions_.erase(session_id);
            }
        }


    private:
        std::unordered_map<std::string, std::shared_ptr<ThalesFileInterface>> sessions_;
        std::unordered_map<std::string, std::shared_ptr<ThalesRemoteScriptWrapper>> wrappers_;
        std::recursive_mutex mutex_; // Allows multiple locks by the same thread
    };

}

    class ThalesFileServiceImpl final : public ThalesFile::Service {
    public:
        explicit ThalesFileServiceImpl(thalesfile::ConnectionManager& manager) : connectionManager_(manager) {}

        grpc::Status ConnectToTerm(grpc::ServerContext* context, const FileServiceConnectRequest* request, FileServiceConnectResponse* response) override {
            auto connection = connectionManager_.createConnection(request->session_id(), request->host());
            if (!connection) {
                response->set_success(false);
                response->set_message("Failed to connect.");
                return grpc::Status(grpc::StatusCode::INTERNAL, "Failed to establish connection.");
            }

            response->set_success(true);
            response->set_message("Connected successfully.");
            response->set_session_id(request->session_id());

            return grpc::Status::OK;
        }

        grpc::Status DisconnectFromTerm(grpc::ServerContext* context, const SessionRequest* request, DisconnectResponse* response) override {
            connectionManager_.removeSession(request->session_id());
            response->set_success(true);
            response->set_message("Disconnected successfully.");
            return grpc::Status::OK;
        }

        grpc::Status AcquireFile(grpc::ServerContext* context, const AcquireFileRequest* request, FileObjectResponse* response) override {
            auto connection = connectionManager_.getConnection(request->session_id());
            if (!connection) {
                response->set_success(false);
                response->set_message("Failed to retrieve ThalesFileInterface instance.");
                return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
            }

            try {
                ThalesFileInterface::FileObject reply = connection->acquireFile(request->filename());
                response->set_success(true);

                // Dynamically allocate FileObject
                zahner::FileObject* z_FileObject = new zahner::FileObject();
                z_FileObject->set_name(reply.name);
                z_FileObject->set_path(reply.path);
                z_FileObject->set_binary_data(reply.binary_data);

                // Pass ownership to response
                response->set_allocated_file(z_FileObject);
                response->set_message("AcquireFile completed successfully.");
            }
            catch (const std::exception& e) {
                response->set_success(false);
                response->set_message(std::string("AcquireFile failed: ") + e.what());
                return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
            }

            return grpc::Status::OK;
        }

        grpc::Status EnableSaveReceivedFilesToDisk(grpc::ServerContext* context, const EnableSaveReceivedFilesToDiskRequest* request, StringResponse* response) override {
            auto connection = connectionManager_.getConnection(request->session_id());
            if (!connection) {
                response->set_success(false);
                response->set_message("Failed to retrieve ThalesFileInterface instance.");
                return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
            }

            try {
                connection->enableSaveReceivedFilesToDisk(request->path(),request->enable());
                response->set_success(true);
                response->set_message("EnableSaveReceivedFilesToDisk completed successfully.");
            }
            catch (const std::exception& e) {
                response->set_success(false);
                response->set_message(std::string("EnableSaveReceivedFilesToDisk failed: ") + e.what());
                return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
            }

            return grpc::Status::OK;
        }

        grpc::Status EnableKeepReceivedFilesInObject(grpc::ServerContext* context, const SessionRequest* request, StringResponse* response) override {
            auto connection = connectionManager_.getConnection(request->session_id());
            if (!connection) {
                response->set_success(false);
                response->set_message("Failed to retrieve ThalesFileInterface instance.");
                return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
            }

            try {
                connection->enableKeepReceivedFilesInObject();
                response->set_success(true);
                response->set_message("EnableKeepReceivedFilesInObject completed successfully.");
            }
            catch (const std::exception& e) {
                response->set_success(false);
                response->set_message(std::string("EnableKeepReceivedFilesInObject failed: ") + e.what());
                return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
            }

            return grpc::Status::OK;
        }

        grpc::Status EnableAutomaticFileExchange(grpc::ServerContext* context, const EnableAutomaticFileExchangeRequest* request, StringResponse* response) override {
            auto connection = connectionManager_.getConnection(request->session_id());
            if (!connection) {
                response->set_success(false);
                response->set_message("Failed to retrieve ThalesFileInterface instance.");
                return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
            }

            try {
                connection->enableAutomaticFileExchange();
                response->set_success(true);
                response->set_message("EnableAutomaticFileExchange completed successfully.");
            }
            catch (const std::exception& e) {
                response->set_success(false);
                response->set_message(std::string("enableAutomaticFileExchange failed: ") + e.what());
                return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
            }

            return grpc::Status::OK;
        }

        grpc::Status DisableAutomaticFileExchange(grpc::ServerContext* context, const SessionRequest* request, StringResponse* response) override {
            auto connection = connectionManager_.getConnection(request->session_id());
            if (!connection) {
                response->set_success(false);
                response->set_message("Failed to retrieve ThalesFileInterface instance.");
                return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
            }

            try {
                connection->disableAutomaticFileExchange();
                response->set_success(true);
                response->set_message("DisableAutomaticFileExchange completed successfully.");
            }
            catch (const std::exception& e) {
                response->set_success(false);
                response->set_message(std::string("DisableAutomaticFileExchange failed: ") + e.what());
                return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
            }

            return grpc::Status::OK;
        }

        grpc::Status DeleteReceivedFiles(grpc::ServerContext* context, const SessionRequest* request, StringResponse* response) override {
            auto connection = connectionManager_.getConnection(request->session_id());
            if (!connection) {
                response->set_success(false);
                response->set_message("Failed to retrieve ThalesFileInterface instance.");
                return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
            }

            try {
                connection->deleteReceivedFiles();
                response->set_success(true);
                response->set_message("DeleteReceivedFiles completed successfully.");
            }
            catch (const std::exception& e) {
                response->set_success(false);
                response->set_message(std::string("DeleteReceivedFiles failed: ") + e.what());
                return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
            }

            return grpc::Status::OK;
        }

        grpc::Status GetReceivedFiles(grpc::ServerContext* context, const SessionRequest* request, FileObjectsResponse* response) override {
            auto connection = connectionManager_.getConnection(request->session_id());
            if (!connection) {
                response->set_success(false);
                response->set_message("Failed to retrieve ThalesFileInterface instance.");
                return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
            }

            try {
                const std::vector<ThalesFileInterface::FileObject>& reply = connection->getReceivedFiles();

                response->set_success(true);
                response->set_message("GetReceivedFiles completed successfully.");
                // Populate response->files with received files
                for (const auto& file : reply) {
                    zahner::FileObject* z_FileObject = response->add_files();
                    z_FileObject->set_name(file.name);
                    z_FileObject->set_path(file.path);
                    z_FileObject->set_binary_data(file.binary_data);
                }
            }
            catch (const std::exception& e) {
                response->set_success(false);
                response->set_message(std::string("GetReceivedFiles failed: ") + e.what());
                return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
            }

            return grpc::Status::OK;
        }


    private:
        thalesfile::ConnectionManager& connectionManager_;
    };
