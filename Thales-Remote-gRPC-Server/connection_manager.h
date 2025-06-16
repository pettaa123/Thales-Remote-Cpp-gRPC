#pragma once

#include <unordered_map>
#include <memory>
#include <mutex>
#include "thalesremoteconnection.h"
#include "thalesremotescriptwrapper.h"
#include "zahner.grpc.pb.h"

using namespace zahner;

class ConnectionManager {
public:
	// Create or retrieve a persistent connection
	std::shared_ptr<ZenniumConnection> createConnection(const std::string& session_id, const std::string& host, const ConnectRequest::Mode selected_mode) {
		std::lock_guard<std::recursive_mutex> lock(mutex_);  // Locking for sessions_

		if (sessions_.find(session_id) == sessions_.end()) {
			auto connection = std::make_shared<ZenniumConnection>();

			// Convert selected_mode enum to string
			std::string mode_string;
			switch (selected_mode) {
			case ConnectRequest::SCRIPT_REMOTE: mode_string = "ScriptRemote"; break;
			case ConnectRequest::WATCH: mode_string = "Watch"; break;
			case ConnectRequest::LOGGING: mode_string = "Logging"; break;
			default: mode_string = "ScriptRemote";
			}

			if (connection->connectToTerm(host, mode_string)) {
				sessions_[session_id] = connection;
			}

			else {
				return nullptr;
			}
		}

		return sessions_[session_id];
	}

	// Create or retrieve a persistent ThalesRemoteScriptWrapper instance
	std::shared_ptr<ThalesRemoteScriptWrapper> createWrapper(const std::string& session_id) {
		std::lock_guard<std::recursive_mutex> lock(mutex_);  // Locking for wrappers_

		if (wrappers_.find(session_id) == wrappers_.end()) {
			auto connection = getConnection(session_id);
			if (!connection) return nullptr; // Connection not found
			try {
				auto wrapper = std::make_shared<ThalesRemoteScriptWrapper>(connection.get());
				std::string response = wrapper->forceThalesIntoRemoteScript();
				wrappers_[session_id] = wrapper;
			}
			catch (std::exception& e) {
				std::cout << "Exception in createWrapper: " << e.what() << std::endl;
				return nullptr;
			}
		}

		return wrappers_[session_id];
	}

	// Get an existing connection
	std::shared_ptr<ZenniumConnection> getConnection(const std::string& session_id) {
		std::lock_guard<std::recursive_mutex> lock(mutex_);
		auto it = sessions_.find(session_id);
		return (it != sessions_.end()) ? it->second : nullptr;
	}

	// Get an existing ThalesRemoteScriptWrapper
	std::shared_ptr<ThalesRemoteScriptWrapper> getWrapper(const std::string& session_id) {
		std::lock_guard<std::recursive_mutex> lock(mutex_);
		auto it = wrappers_.find(session_id);
		return (it != wrappers_.end()) ? it->second : nullptr;
	}

	// Remove a session when it's no longer needed
	void removeSession(const std::string& session_id) {
		std::lock_guard<std::recursive_mutex> lock(mutex_);  // Single lock for both maps

		wrappers_.erase(session_id);
		if (sessions_.find(session_id) != sessions_.end()) {
			sessions_[session_id]->disconnectFromTerm();
			sessions_.erase(session_id);
		}
	}

private:
	std::unordered_map<std::string, std::shared_ptr<ZenniumConnection>> sessions_;
	std::unordered_map<std::string, std::shared_ptr<ThalesRemoteScriptWrapper>> wrappers_;
	std::recursive_mutex mutex_; // Allows multiple locks by the same thread
};