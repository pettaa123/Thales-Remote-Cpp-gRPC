#pragma once

#include <unordered_map>
#include <memory>
#include <mutex>
#include <fstream>
#include "thalesremotescriptwrapper.h"
#include "thalesremoteerror.h"
#include "thalesfileinterface.h"
#include "zahner.grpc.pb.h"
#include "zahnerzenniumserviceimpl.h"
#include "ism_txt_parser.h"

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
				catch (const std::exception& e) {
					std::cout << "Exception in ThalesFileInterface::createConnection " << e.what() << std::endl;
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
			std::cout << "Failed to establish connection. ThalesXT App running?" << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, "Failed to establish connection.");
		}

		response->set_session_id(request->session_id());
		std::cout << "FileInterface Client Connected" << std::endl;

		return grpc::Status::OK;
	}

	grpc::Status DisconnectFromTerm(grpc::ServerContext* context, const SessionRequest* request, DisconnectResponse* response) override {
		try {
			connectionManager_.removeSession(request->session_id());
		}
		catch (std::exception& e) {
			std::cout << "Exception in DisconnectFromTerm: " << e.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, "Failed to disonnect.");

		}
		return grpc::Status::OK;
	}

	grpc::Status ListFiles(grpc::ServerContext* context, const DirectoryRequest* request, DirectoryResponse* response) override {
		std::string path = request->path();
		if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
			return grpc::Status(grpc::INVALID_ARGUMENT, "Invalid directory path");
		}

		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			response->add_files(entry.path().string());
		}

		return grpc::Status::OK;
	}

	grpc::Status ParseFiles(grpc::ServerContext* context, const ParseRequest* request, ParseResponse* response) override {
		std::string directoryPath = request->directory_path();
		std::vector<std::string> csvFiles;
		if (!fs::exists(directoryPath)) {
			return grpc::Status(grpc::INVALID_ARGUMENT, "Directory does not exist");
		}

		for (const auto& entry : fs::directory_iterator(directoryPath)) {
			if (entry.is_regular_file() && entry.path().extension() == ".txt") {
				std::vector<DataEntry> fileData = parseFile(entry.path().string());
				std::string csvFilename = writeToCSV(directoryPath, entry.path().stem().string(), fileData);
				csvFiles.push_back(csvFilename);
			}
		}

		for (const auto& filename : csvFiles) {
			response->add_csv_filenames(filename);
		}

		return grpc::Status::OK;
	}

	grpc::Status DownloadFile(grpc::ServerContext* context, const FileRequest* request, grpc::ServerWriter<FileChunk>* writer) override {
		std::ifstream input_file(request->file_path(), std::ios::binary);
		if (!input_file) {
			return grpc::Status(grpc::NOT_FOUND, "File not found.");
		}

		FileChunk chunk;
		char buffer[4096];
		while (input_file.read(buffer, sizeof(buffer)) || input_file.gcount() > 0) {
			chunk.set_data(std::string(buffer, input_file.gcount()));
			writer->Write(chunk);
		}

		input_file.close();
		return grpc::Status::OK;
	}

	//grpc::Status AcquireFile(grpc::ServerContext* context, const AcquireFileRequest* request, FileObjectResponse* response) override {
	//	auto connection = connectionManager_.getConnection(request->session_id());
	//	if (!connection) {
	//		return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesFileInterface instance.");
	//	}
	//
	//	try {
	//		ThalesFileInterface::FileObject reply = connection->acquireFile(request->filename());
	//
	//
	//		// Dynamically allocate FileObject
	//		zahner::FileObject* z_FileObject = new zahner::FileObject();
	//		z_FileObject->set_name(reply.name);
	//		z_FileObject->set_path(reply.path);
	//		//z_FileObject->set_binary_data(
	//		//	reply.binary_data.data(),
	//		//	static_cast<int>(reply.binary_data.size())
	//		//);
	//
	//		// Transfer ownership of the allocated FileObject to the response
	//		response->set_allocated_file(z_FileObject);
	//	}
	//	catch (const std::exception& e) {
	//		std::cout << "Exception in AcquireFile: " << e.what() << std::endl;
	//		return grpc::Status(grpc::StatusCode::INTERNAL, std::string("AcquireFile failed: ") + e.what());
	//	}
	//
	//	return grpc::Status::OK;
	//}

	grpc::Status EnableSaveReceivedFilesToDisk(grpc::ServerContext* context, const EnableSaveReceivedFilesToDiskRequest* request, StringResponse* response) override {
		auto connection = connectionManager_.getConnection(request->session_id());
		if (!connection) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesFileInterface instance.");
		}

		try {
			connection->enableSaveReceivedFilesToDisk(request->path(), request->enable());
		}
		catch (const std::exception& e) {
			std::cout << "Exception in EnableSaveReceiveFilesToDisk: " << e.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("EnableSaveReceivedFilesToDisk failed: ") + e.what());
		}

		return grpc::Status::OK;
	}

	grpc::Status EnableKeepReceivedFilesInObject(grpc::ServerContext* context, const SessionRequest* request, StringResponse* response) override {
		auto connection = connectionManager_.getConnection(request->session_id());
		if (!connection) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesFileInterface instance.");
		}

		try {
			connection->enableKeepReceivedFilesInObject();
			response->set_message("EnableKeepReceivedFilesInObject completed successfully.");
		}
		catch (const std::exception& e) {
			std::cout << "Exception in EnableKeepReceivedFilsInObject: " << e.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("EnableKeepReceivedFilesInObject failed: ") + e.what());
		}

		return grpc::Status::OK;
	}

	grpc::Status EnableAutomaticFileExchange(grpc::ServerContext* context, const EnableAutomaticFileExchangeRequest* request, StringResponse* response) override {
		auto connection = connectionManager_.getConnection(request->session_id());
		if (!connection) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesFileInterface instance.");
		}

		try {
			response->set_reply(connection->enableAutomaticFileExchange());
		}
		catch (const std::exception& e) {
			std::cout << "Exception in EnableAutomaticFileExchange: " << e.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("enableAutomaticFileExchange failed: ") + e.what());
		}

		return grpc::Status::OK;
	}

	grpc::Status DisableAutomaticFileExchange(grpc::ServerContext* context, const SessionRequest* request, StringResponse* response) override {
		auto connection = connectionManager_.getConnection(request->session_id());
		if (!connection) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesFileInterface instance.");
		}

		try {
			connection->disableAutomaticFileExchange();
			response->set_reply(connection->disableAutomaticFileExchange());
		}
		catch (const std::exception& e) {
			std::cout << "Exception in DisableAutomaticFileExchange: " << e.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("DisableAutomaticFileExchange failed: ") + e.what());
		}

		return grpc::Status::OK;
	}

	grpc::Status DeleteReceivedFiles(grpc::ServerContext* context, const SessionRequest* request, StringResponse* response) override {
		auto connection = connectionManager_.getConnection(request->session_id());
		if (!connection) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesFileInterface instance.");
		}

		try {
			connection->deleteReceivedFiles();
		}
		catch (const std::exception& e) {
			std::cout << "Exception in DeleteReceivedFiles: " << e.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("DeleteReceivedFiles failed: ") + e.what());
		}

		return grpc::Status::OK;
	}

	//grpc::Status GetReceivedFiles(grpc::ServerContext* context, const SessionRequest* request, FileObjectsResponse* response) override {
	//	auto connection = connectionManager_.getConnection(request->session_id());
	//	if (!connection) {
	//		return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesFileInterface instance.");
	//	}
	//
	//	try {
	//		const std::vector<ThalesFileInterface::FileObject>& reply = connection->getReceivedFiles();
	//
	//		// Populate response->files with received files
	//		for (const auto& file : reply) {
	//			zahner::FileObject* z_FileObject = response->add_files();
	//			z_FileObject->set_name(file.name);
	//			z_FileObject->set_path(file.path);
	//			z_FileObject->set_binary_data(file.binary_data);
	//		}
	//	}
	//	catch (const std::exception& e) {
	//		std::cout << "Exception in GetReceivedFiles: " << e.what() << std::endl;
	//		return grpc::Status(grpc::StatusCode::INTERNAL, std::string("GetReceivedFiles failed: ") + e.what());
	//	}
	//
	//	return grpc::Status::OK;
	//}


private:
	thalesfile::ConnectionManager& connectionManager_;
};
