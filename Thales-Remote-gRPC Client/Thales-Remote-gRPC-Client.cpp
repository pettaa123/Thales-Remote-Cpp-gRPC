// Thales-Remote-gRPC Client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <grpcpp/grpcpp.h>
#include "zahner.grpc.pb.h"


using namespace zahner;

class ThalesFileClient {
public:
	ThalesFileClient(std::shared_ptr<grpc::Channel> channel)
		: stub_(ThalesFile::NewStub(channel)) {
	}

	// Sends a request to connect to the terminal
	std::string connectToTerm(const std::string& session_id, const std::string& host) {
		// Prepare request
		FileServiceConnectRequest request;
		request.set_session_id(session_id);
		request.set_host(host);

		// Container for response
		FileServiceConnectResponse response;
		grpc::ClientContext context;

		// Remote Procedure Call
		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->ConnectToTerm(&context, request, &response);

		// Returns results based on RPC status
		if (status.ok()) {
			return response.message();
		}
		else {
			std::cout << "gRPC Error: " << status.error_code() << " - " << status.error_message() << std::endl;
			return "RPC Failed";
		}
	}

	// Sends a request to connect to the terminal
	std::string disconnectFromTerm(const std::string& session_id) {
		// Prepare request
		SessionRequest request;
		request.set_session_id(session_id);

		// Container for response
		DisconnectResponse response;
		grpc::ClientContext context;

		// Remote Procedure Call
		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->DisconnectFromTerm(&context, request, &response);

		// Returns results based on RPC status
		if (status.ok()) {
			return response.message();
		}
		else {
			std::cout << "gRPC Error: " << status.error_code() << " - " << status.error_message() << std::endl;
			return "RPC Failed";
		}
	}

	zahner::FileObject acquireFile(const std::string& session_id, const std::string& filename) {
		// Prepare request
		AcquireFileRequest request;
		request.set_session_id(session_id);
		request.set_filename(filename);

		// Container for response
		zahner::FileObjectResponse response;
		grpc::ClientContext context;

		// Remote Procedure Call
		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->AcquireFile(&context, request, &response);

		if (!status.ok()) {
			std::cout << "gRPC Error: " << status.error_code() << " - " << status.error_message() << std::endl;
			return zahner::FileObject();
		}
		return response.file();
	}

	// Sends a request to enable keeping received files in the object
	std::string enableKeepReceivedFilesInObject(const std::string& session_id) {
		zahner::SessionRequest request;
		request.set_session_id(session_id);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->EnableKeepReceivedFilesInObject(&context, request, &response);

		return handleResponse(status, response);
	}

	// Sends a request to enable automatic file exchange
	std::string enableAutomaticFileExchange(const std::string& session_id, const bool enable, const std::string& file_extensions) {
		zahner::EnableAutomaticFileExchangeRequest request;
		request.set_session_id(session_id);
		request.set_enable(enable);
		request.set_file_extensions(file_extensions);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->EnableAutomaticFileExchange(&context, request, &response);

		return handleResponse(status, response);
	}

	std::string disableAutomaticFileExchange(const std::string& session_id) {
		zahner::SessionRequest request;
		request.set_session_id(session_id);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->DisableAutomaticFileExchange(&context, request, &response);

		return handleResponse(status, response);
	}

	// Sends a request to retrieve received files
	std::vector<zahner::FileObject> getReceivedFiles(const std::string& session_id) {
		// Prepare request
		zahner::SessionRequest request;
		request.set_session_id(session_id);

		// Container for response
		zahner::FileObjectsResponse response;
		grpc::ClientContext context;

		// Remote Procedure Call
		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->GetReceivedFiles(&context, request, &response);

		// Construct response struct
		std::vector<zahner::FileObject> result;

		if (!status.ok()) {
			std::cout << "gRPC Error: " << status.error_code() << " - " << status.error_message() << std::endl;
			return result;
		}

		// populate the vector with received files
		for (const auto& file : response.files()) {
			result.push_back(file);
		}


		return result;
	}

	std::string deleteReceivedFiles(const std::string& session_id) {
		zahner::SessionRequest request;
		request.set_session_id(session_id);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->DeleteReceivedFiles(&context, request, &response);

		return handleResponse(status, response);
	}


private:
	std::unique_ptr<ThalesFile::Stub> stub_;

	std::string handleResponse(grpc::Status status, const zahner::StringResponse& response) {
		if (status.ok()) {
			return response.reply();
		}
		else {
			std::cout << "gRPC Error: " << status.error_code() << " - " << status.error_message() << std::endl;
			return "Operation Failed";
		}
	}
};

class ZahnerClient {
public:
	ZahnerClient(std::shared_ptr<grpc::Channel> channel)
		: stub_(ZahnerZennium::NewStub(channel)) {
	}

	std::string setEISCounter(const std::string& session_id, const int32_t number) {
		// Prepare request
		SetEISCounterRequest request;
		request.set_session_id(session_id);
		request.set_number(number);

		// Container for response
		zahner::StringResponse response;
		grpc::ClientContext context;


		// Remote Procedure Call
		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->SetEISCounter(&context, request, &response);
		return handleResponse(status, response);
	}

	std::string setEISNaming(const std::string& session_id, const SetEISNamingRequest_NamingRule namingrule) {
		// Prepare request
		SetEISNamingRequest request;
		request.set_session_id(session_id);
		request.set_naming(namingrule);

		// Container for response
		zahner::StringResponse response;
		grpc::ClientContext context;

		// Remote Procedure Call
		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->SetEISNaming(&context, request, &response);
		return handleResponse(status, response);
	}

	std::string setEISOutputPath(const std::string& session_id, const std::string& output_path) {
		// Prepare request
		zahner::SetEISOutputPathRequest request;
		request.set_session_id(session_id);
		request.set_path(output_path);

		// Container for response
		zahner::StringResponse response;
		grpc::ClientContext context;

		// Remote Procedure Call
		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->SetEISOutputPath(&context, request, &response);
		return handleResponse(status, response);
	}

	std::string setEISOutputFileName(const std::string& session_id, const std::string& file_name) {
		// Prepare request
		zahner::SetEISOutputFileNameRequest request;
		request.set_session_id(session_id);
		request.set_name(file_name);

		// Container for response
		zahner::StringResponse response;
		grpc::ClientContext context;

		// Remote Procedure Call
		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->SetEISOutputFileName(&context, request, &response);
		return handleResponse(status, response);
	}

	std::string measureEIS(const std::string& session_id) {
		// Prepare request
		zahner::SessionRequest request;
		request.set_session_id(session_id);

		// Container for response
		zahner::StringResponse response;
		grpc::ClientContext context;

		// Remote Procedure Call
		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->MeasureEIS(&context, request, &response);
		return handleResponse(status, response);
	}


	std::string setupPad4ChannelWithVoltageRange(const std::string& session_id, int32_t card, int32_t channel, bool enabled, double voltage_range) {
		// Prepare request
		zahner::SetupPad4ChannelWithVoltageRangeRequest request;
		request.set_session_id(session_id);
		request.set_card(card);
		request.set_channel(channel);
		request.set_enabled(enabled);
		request.set_voltage_range(voltage_range);

		// Container for response
		zahner::StringResponse response;
		grpc::ClientContext context;

		// Remote Procedure Call
		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->SetupPad4ChannelWithVoltageRange(&context, request, &response);
		return handleResponse(status, response);
	}

	std::string setupPad4ModeGlobal(const std::string& session_id, zahner::SetupPad4ModeGlobalRequest::Pad4Mode mode) {
		// Prepare request
		zahner::SetupPad4ModeGlobalRequest request;
		request.set_session_id(session_id);
		request.set_mode(mode);

		// Container for response
		zahner::StringResponse response;
		grpc::ClientContext context;

		// Remote Procedure Call
		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->SetupPad4ModeGlobal(&context, request, &response);
		return handleResponse(status, response);
	}

	std::string enablePad4Global(const std::string& session_id, bool enabled) {
		// Prepare request
		zahner::EnablePad4GlobalRequest request;
		request.set_session_id(session_id);
		request.set_enabled(enabled);

		// Container for response
		zahner::StringResponse response;
		grpc::ClientContext context;

		// Remote Procedure Call
		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->EnablePad4Global(&context, request, &response);
		return handleResponse(status, response);
	}

	std::string setLowerFrequencyLimit(const std::string& session_id, double frequency) {
		zahner::SetLowerFrequencyLimitRequest request;
		request.set_session_id(session_id);
		request.set_frequency(frequency);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->SetLowerFrequencyLimit(&context, request, &response);
		return handleResponse(status, response);
	}

	std::string setUpperFrequencyLimit(const std::string& session_id, double frequency) {
		zahner::SetUpperFrequencyLimitRequest request;
		request.set_session_id(session_id);
		request.set_frequency(frequency);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->SetUpperFrequencyLimit(&context, request, &response);
		return handleResponse(status, response);
	}

	std::string setLowerNumberOfPeriods(const std::string& session_id, int32_t periods) {
		zahner::SetLowerNumberOfPeriodsRequest request;
		request.set_session_id(session_id);
		request.set_periods(periods);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->SetLowerNumberOfPeriods(&context, request, &response);
		return handleResponse(status, response);
	}

	std::string setLowerStepsPerDecade(const std::string& session_id, int32_t steps) {
		zahner::SetLowerStepsPerDecadeRequest request;
		request.set_session_id(session_id);
		request.set_steps(steps);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->SetLowerStepsPerDecade(&context, request, &response);
		return handleResponse(status, response);
	}

	std::string setUpperNumberOfPeriods(const std::string& session_id, int32_t periods) {
		zahner::SetUpperNumberOfPeriodsRequest request;
		request.set_session_id(session_id);
		request.set_periods(periods);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->SetUpperNumberOfPeriods(&context, request, &response);
		return handleResponse(status, response);
	}

	std::string setScanDirection(const std::string& session_id, zahner::SetScanDirectionRequest::ScanDirection direction) {
		zahner::SetScanDirectionRequest request;
		request.set_session_id(session_id);
		request.set_direction(direction);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->SetScanDirection(&context, request, &response);
		return handleResponse(status, response);
	}

	std::string setScanStrategy(const std::string& session_id, zahner::SetScanStrategyRequest::ScanStrategy strategy) {
		zahner::SetScanStrategyRequest request;
		request.set_session_id(session_id);
		request.set_strategy(strategy);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->SetScanStrategy(&context, request, &response);
		return handleResponse(status, response);
	}

	std::string setUpperStepsPerDecade(const std::string& session_id, int32_t steps) {
		zahner::SetUpperStepsPerDecadeRequest request;
		request.set_session_id(session_id);
		request.set_steps(steps);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->SetUpperStepsPerDecade(&context, request, &response);
		return handleResponse(status, response);
	}

	std::string setStartFrequency(const std::string& session_id, double frequency) {
		// Prepare request
		zahner::SetStartFrequencyRequest request;
		request.set_session_id(session_id);
		request.set_frequency(frequency);

		// Container for response
		zahner::StringResponse response;
		grpc::ClientContext context;

		// Remote Procedure Call
		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->SetStartFrequency(&context, request, &response);
		return handleResponse(status, response);
	}



	// Sends a request to calibrate offsets
	std::string calibrateOffsets(const std::string& session_id) {
		// Prepare request
		SessionRequest request;
		request.set_session_id(session_id);

		// Container for response
		StringResponse response;
		grpc::ClientContext context;


		// Remote Procedure Call
		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->CalibrateOffsets(&context, request, &response);
		return handleResponse(status, response);
	}

	// Sends a request to set the potentiostat mode
	std::string setPotentiostatMode(const std::string& session_id, ModeRequest::PotentiostatMode mode) {
		// Prepare request
		ModeRequest request;
		request.set_session_id(session_id);
		request.set_mode(mode);

		// Container for response
		StringResponse response;
		grpc::ClientContext context;

		// Remote Procedure Call
		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->SetPotentiostatMode(&context, request, &response);
		return handleResponse(status, response);
	}

	// Sends a request to set the potential value
	std::string setPotential(const std::string& session_id, double potential) {
		// Prepare request
		SetPotentialRequest request;
		request.set_session_id(session_id);
		request.set_potential(potential);

		// Container for response
		StringResponse response;
		grpc::ClientContext context;

		// Remote Procedure Call
		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->SetPotential(&context, request, &response);
		return handleResponse(status, response);
	}

	// Sends a request to connect to the terminal
	std::string connectToTerm(const std::string& session_id, const std::string& host, ConnectRequest::Mode selected_mode) {
		// Prepare request
		ConnectRequest request;
		request.set_session_id(session_id);
		request.set_host(host);
		request.set_selected_mode(selected_mode);  // Using the enum

		// Container for response
		ConnectResponse response;
		grpc::ClientContext context;

		// Remote Procedure Call
		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->ConnectToTerm(&context, request, &response);

		// Returns results based on RPC status
		if (status.ok()) {
			return response.message();
		}
		else {
			std::cout << "gRPC Error: " << status.error_code() << " - " << status.error_message() << std::endl;
			return "RPC Failed";
		}
	}

	// Sends a request to connect to the terminal
	std::string disconnectFromTerm(const std::string& session_id) {
		// Prepare request
		SessionRequest request;
		request.set_session_id(session_id);

		// Container for response
		DisconnectResponse response;
		grpc::ClientContext context;

		// Remote Procedure Call
		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->DisconnectFromTerm(&context, request, &response);

		// Returns results based on RPC status
		if (status.ok()) {
			return response.message();
		}
		else {
			std::cout << "gRPC Error: " << status.error_code() << " - " << status.error_message() << std::endl;
			return "RPC Failed";
		}
	}

	std::string enablePotentiostat(const std::string& session_id, bool enable) {
		EnablePotentiostatRequest request;
		request.set_session_id(session_id);
		request.set_enable(enable);

		StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->EnablePotentiostat(&context, request, &response);
		return handleResponse(status, response);
	}

	std::string disablePotentiostat(const std::string& session_id) {
		SessionRequest request;
		request.set_session_id(session_id);

		StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->DisablePotentiostat(&context, request, &response);
		return handleResponse(status, response);
	}

	std::string setFrequency(const std::string& session_id, double frequency) {
		FrequencyRequest request;
		request.set_session_id(session_id);
		request.set_frequency(frequency);

		StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->SetFrequency(&context, request, &response);
		return handleResponse(status, response);
	}

	std::string setAmplitude(const std::string& session_id, double amplitude) {
		AmplitudeRequest request;
		request.set_session_id(session_id);
		request.set_amplitude(amplitude);

		StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->SetAmplitude(&context, request, &response);
		return handleResponse(status, response);
	}

	std::string setNumberOfPeriods(const std::string& session_id, int32_t num_periods) {
		PeriodsRequest request;
		request.set_session_id(session_id);
		request.set_num_periods(num_periods);

		StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->SetNumberOfPeriods(&context, request, &response);
		return handleResponse(status, response);
	}

	ComplexNumber getImpedance(const std::string& session_id) {
		ImpedanceRequest request;
		request.set_session_id(session_id);

		ImpedanceResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->GetImpedance(&context, request, &response);
		if (status.ok()) {
			return response.impedance();
		}
		else {
			std::cout << "gRPC Error: " << status.error_code() << " - " << status.error_message() << std::endl;
			return ComplexNumber(); // Return a default object in case of failure
		}
	}

	// Get the potential value
	double getPotential(const std::string& session_id) {
		SessionRequest request;
		request.set_session_id(session_id);

		PotentialResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->GetPotential(&context, request, &response);
		if (status.ok()) {
			return response.potential();
		}
		else {
			std::cout << "gRPC Error: " << status.error_code() << " - " << status.error_message() << std::endl;
			return -1.0; // Return an error indicator
		}
	}

	// Get the current value
	double getCurrent(const std::string& session_id) {
		SessionRequest request;
		request.set_session_id(session_id);

		CurrentResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->GetCurrent(&context, request, &response);
		if (status.ok()) {
			return response.current();
		}
		else {
			std::cout << "gRPC Error: " << status.error_code() << " - " << status.error_message() << std::endl;
			return -1.0; // Return an error indicator
		}
	}

	// Sends a request to set the current value
	std::string setCurrent(const std::string& session_id, double current) {
		SetCurrentRequest request;
		request.set_session_id(session_id);
		request.set_current(current);

		StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->SetCurrent(&context, request, &response);
		return handleResponse(status, response);
	}


private:
	std::unique_ptr<ZahnerZennium::Stub> stub_;

	std::string handleResponse(grpc::Status status, const StringResponse& response) {
		if (status.ok()) {
			return response.reply();
		}
		else {
			std::cout << "gRPC Error: " << status.error_code() << " - " << status.error_message() << std::endl;
			return "Operation Failed";
		}
	}
};

void RunClient() {
	std::string target_address("localhost:50051");
	ZahnerClient client(
		grpc::CreateChannel(target_address, grpc::InsecureChannelCredentials()));

	std::string session_id = "12345";
	std::string host = "localhost";  // Example host
	ConnectRequest::Mode mode = ConnectRequest::SCRIPT_REMOTE;  // Choose mode

	std::string response = client.connectToTerm(session_id, host, mode);
	std::cout << "Connection Response: " << response << std::endl;

	// Attempt calibration
	//std::string calibrationResponse = client.calibrateOffsets(session_id);
	//std::cout << "Calibration Response: " << calibrationResponse << std::endl;


	response = client.setPotentiostatMode(session_id, ModeRequest::PotentiostatMode::ModeRequest_PotentiostatMode_POTENTIOSTATIC);
	std::cout << "setPotentiostatMode Response: " << response << std::endl;

	response = client.setPotential(session_id, 1.0);
	std::cout << "setPotential Response: " << response << std::endl;

	response = client.enablePotentiostat(session_id, true);
	std::cout << "enablePotentiostat Response: " << response << std::endl;

	for (int i = 0; i < 5; i++) {
		std::cout << "Potential: " << client.getPotential(session_id) << std::endl;
		std::cout << "Current: " << client.getCurrent(session_id) << std::endl;
	}

	response = client.disablePotentiostat(session_id);
	std::cout << "disablePotentiostat Response: " << response << std::endl;

	response = client.setPotentiostatMode(session_id, ModeRequest::GALVANOSTATIC);
	std::cout << "setPotentiostatMode Response: " << response << std::endl;

	response = client.setCurrent(session_id, 20e-9);
	std::cout << "setCurrent Response: " << response << std::endl;

	response = client.enablePotentiostat(session_id, true);
	std::cout << "enablePotentiostat Response: " << response << std::endl;

	for (int i = 0; i < 5; i++) {
		std::cout << "Potential: " << client.getPotential(session_id) << std::endl;
		std::cout << "Current: " << client.getCurrent(session_id) << std::endl;
	}

	response = client.disablePotentiostat(session_id);
	std::cout << "disablePotentiostat Response: " << response << std::endl;

	response = client.setPotentiostatMode(session_id, ModeRequest::PotentiostatMode::ModeRequest_PotentiostatMode_POTENTIOSTATIC);
	std::cout << "setPotentiostatMode Response: " << response << std::endl;

	response = client.setPotential(session_id, 1.0);
	std::cout << "setPotential Response: " << response << std::endl;

	response = client.enablePotentiostat(session_id, true);
	std::cout << "enablePotentiostat Response: " << response << std::endl;


	response = client.setFrequency(session_id, 2000);
	std::cout << "setFrequency Response: " << response << std::endl;

	response = client.setAmplitude(session_id, 10e-3);
	std::cout << "setAmplitude Response: " << response << std::endl;

	response = client.setNumberOfPeriods(session_id, 3);
	std::cout << "setNumberOfPeriods Response: " << response << std::endl;

	ComplexNumber impedance = client.getImpedance(session_id);

	std::cout << "real: " << impedance.real() << "double: " << impedance.imag() << std::endl;

	response = client.disablePotentiostat(session_id);
	std::cout << "disablePotentiostat: " << response << std::endl;

	response = client.setAmplitude(session_id, 0);
	std::cout << "setAmplitude Response: " << response << std::endl;

	response = client.disconnectFromTerm(session_id);
	std::cout << "disconnectFromTerm Response: " << response << std::endl;

	std::cout << "finish" << std::endl;

}

void RunFileExchangeExample() {
	std::string target_address("localhost:50051");
	ZahnerClient client(grpc::CreateChannel(target_address, grpc::InsecureChannelCredentials()));
	ThalesFileClient file_client(grpc::CreateChannel(target_address, grpc::InsecureChannelCredentials()));

	std::string session_id = "12345";
	std::string host = "localhost";  // Example host
	ConnectRequest::Mode mode = ConnectRequest::SCRIPT_REMOTE;  // Choose mode

	std::string response = client.connectToTerm(session_id, host, mode);
	std::cout << "Connection Response: " << response << std::endl;

	// Attempt calibration
//std::string calibrationResponse = client.calibrateOffsets(session_id);
//std::cout << "Calibration Response: " << calibrationResponse << std::endl;

	response = file_client.connectToTerm(session_id, host);
	std::cout << "Connection Response: " << response << std::endl;

	/*
	 * Measure EIS spectra with a sequential number in the file name that has been specified.
	 * Starting with number 1.
	 */

	response = client.setEISNaming(session_id, SetEISNamingRequest_NamingRule_COUNTER);
	std::cout << "SetEISNaming Response: " << response << std::endl;

	response = client.setEISCounter(session_id, 1);
	std::cout << "SetEISCounter Response: " << response << std::endl;

	response = client.setEISOutputPath(session_id, "c:\\thales\\temp\\test1");
	std::cout << "SetEISOutputPath Response: " << response << std::endl;

	response = client.setEISOutputFileName(session_id, "spectra_cells");
	std::cout << "SetEISOutputFileName Response: " << response << std::endl;

	/*
	 * Setting the parameters for the spectra.
	 * Alternatively a rule file can be used as a template.
	 */

	response = client.setPotentiostatMode(session_id, ModeRequest::PotentiostatMode::ModeRequest_PotentiostatMode_POTENTIOSTATIC);
	std::cout << "SetEISOutputFileName Response: " << response << std::endl;

	response = client.setAmplitude(session_id, 50e-3);
	std::cout << "setAmplitude Response: " << response << std::endl;

	response = client.setPotential(session_id, 0);
	std::cout << "setPotential Response: " << response << std::endl;

	response = client.setLowerFrequencyLimit(session_id, 500);
	std::cout << "setLowerFrequencyLimit Response: " << response << std::endl;

	response = client.setStartFrequency(session_id, 1000);
	std::cout << "setStartFrequency Response: " << response << std::endl;

	response = client.setUpperFrequencyLimit(session_id, 2000);
	std::cout << "setUpperFrequencyLimit Response: " << response << std::endl;

	response = client.setLowerNumberOfPeriods(session_id, 3);
	std::cout << "setLowerNumberOfPeriods Response: " << response << std::endl;

	response = client.setLowerStepsPerDecade(session_id, 5);
	std::cout << "setLowerStepsPerDecade Response: " << response << std::endl;

	response = client.setUpperNumberOfPeriods(session_id, 20);
	std::cout << "setUpperNumberOfPeriodsResponse: " << response << std::endl;

	response = client.setUpperStepsPerDecade(session_id, 5);
	std::cout << "setUpperStepsPerDecade Response: " << response << std::endl;

	response = client.setScanDirection(session_id, SetScanDirectionRequest::START_TO_MAX);
	std::cout << "setScanDirection Response: " << response << std::endl;

	response = client.setScanStrategy(session_id, SetScanStrategyRequest::SINGLE_SINE);
	std::cout << "setScanStrategy Response: " << response << std::endl;

	/*
	* Setup PAD4 Channels.
	* The PAD4 setup is encapsulated with try and catch to catch the exception if no PAD4 card is present.
	*/

	response = client.setupPad4ChannelWithVoltageRange(session_id, 1, 1, true, 4.0);
	std::cout << "setupPad4ChannelWithVoltageRange Response: " << response << std::endl;
	response = client.setupPad4ChannelWithVoltageRange(session_id, 1, 2, true, 4.0);
	std::cout << "setupPad4ChannelWithVoltageRange Response: " << response << std::endl;
	response = client.setupPad4ModeGlobal(session_id, SetupPad4ModeGlobalRequest::VOLTAGE);
	std::cout << "setupPad4ModeGlobal Response: " << response << std::endl;
	response = client.enablePad4Global(session_id, true);
	std::cout << "enablePad4Global Response: " << response << std::endl;

	/*
	* Switching on the potentiostat before the measurement,
	* so that EIS is measured at the set DC potential.
	* If the potentiostat is off before the measurement,
	* the measurement is performed at the OCP.
	*/

	response = client.enablePotentiostat(session_id, true);
	std::cout << "enablePotentiostat Response: " << response << std::endl;

	response = client.measureEIS(session_id);
	std::cout << "measureEIS Response: " << response << std::endl;

	FileObject file_obj = file_client.acquireFile(session_id, R"(C:\THALES\temp\test1\spectra_cells_0002_ser01.ism)");
	std::cout << "acquireFile Response: " << response << std::endl;

	response = file_client.enableKeepReceivedFilesInObject(session_id);
	std::cout << "enableKeepReceivedFilesInObject Response: " << response << std::endl;

	response = file_client.enableAutomaticFileExchange(session_id, true, "");
	std::cout << "enableAutomaticFileExchange Response: " << response << std::endl;

	response = client.measureEIS(session_id);
	std::cout << "measureEIS Response: " << response << std::endl;

	response = client.disablePotentiostat(session_id);
	std::cout << "disablePotentiostat Response: " << response << std::endl;

	response = client.disconnectFromTerm(session_id);
	std::cout << "disconnectFromTerm Response: " << response << std::endl;

	response = file_client.disableAutomaticFileExchange(session_id);
	std::cout << "disableAutomaticFileExchange Response: " << response << std::endl;

	std::cout << "Received Files: " << file_client.getReceivedFiles(session_id).size() << std::endl;

	response = file_client.deleteReceivedFiles(session_id);
	std::cout << "deleteReceivedFiles Response: " << response << std::endl;

	response = file_client.disconnectFromTerm(session_id);
	std::cout << "disconnectFromTerm Response: " << response << std::endl;
}

int main() {
	std::cout << "Starting Zahner Client...\n";
	RunClient();
	RunFileExchangeExample();

	std::cout << "Press any key to exit...";
	std::cin.get();  // Waits for a key press

	return 0;
}