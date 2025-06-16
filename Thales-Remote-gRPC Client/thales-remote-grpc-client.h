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
	FileServiceConnectResponse connectToTerm(const std::string& session_id, const std::string& host) {
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
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	// Sends a request to connect to the terminal
	DisconnectResponse disconnectFromTerm(const std::string& session_id) {
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
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	FileObjectResponse acquireFile(const std::string& session_id, const std::string& filename) {
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

		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	// Sends a request to enable keeping received files in the object
	StringResponse enableKeepReceivedFilesInObject(const std::string& session_id) {
		zahner::SessionRequest request;
		request.set_session_id(session_id);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->EnableKeepReceivedFilesInObject(&context, request, &response);

		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	// Sends a request to enable automatic file exchange
StringResponse enableAutomaticFileExchange(const std::string& session_id, const bool enable, const std::string& file_extensions) {
		zahner::EnableAutomaticFileExchangeRequest request;
		request.set_session_id(session_id);
		request.set_enable(enable);
		request.set_file_extensions(file_extensions);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->EnableAutomaticFileExchange(&context, request, &response);

		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	StringResponse disableAutomaticFileExchange(const std::string& session_id) {
		zahner::SessionRequest request;
		request.set_session_id(session_id);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->DisableAutomaticFileExchange(&context, request, &response);

		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	// Sends a request to retrieve received files
	FileObjectsResponse getReceivedFiles(const std::string& session_id) {
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

		// Returns results based on RPC status
		if (status.ok()) {
			// populate the vector with received files
			for (const auto& file : response.files()) {
				result.push_back(file);
			}
			return response;
		}
		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

StringResponse deleteReceivedFiles(const std::string& session_id) {
		zahner::SessionRequest request;
		request.set_session_id(session_id);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->DeleteReceivedFiles(&context, request, &response);

		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}


private:
	std::unique_ptr<ThalesFile::Stub> stub_;

};

class ZahnerClient {
public:
	ZahnerClient(std::shared_ptr<grpc::Channel> channel)
		: stub_(ZahnerZennium::NewStub(channel)) {
	}

	StringResponse setEISCounter(const std::string& session_id, const int32_t number) {
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
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	StringResponse setEISNaming(const std::string& session_id, const SetEISNamingRequest_NamingRule namingrule) {
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
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	StringResponse setEISOutputPath(const std::string& session_id, const std::string& output_path) {
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
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	StringResponse setEISOutputFileName(const std::string& session_id, const std::string& file_name) {
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
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	StringResponse measureEIS(const std::string& session_id) {
		// Prepare request
		zahner::SessionRequest request;
		request.set_session_id(session_id);

		// Container for response
		zahner::StringResponse response;
		grpc::ClientContext context;

		// Remote Procedure Call
		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->MeasureEIS(&context, request, &response);
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}


	StringResponse setupPad4ChannelWithVoltageRange(const std::string& session_id, int32_t card, int32_t channel, bool enabled, double voltage_range) {
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
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	StringResponse setupPad4ModeGlobal(const std::string& session_id, zahner::SetupPad4ModeGlobalRequest::Pad4Mode mode) {
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
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	StringResponse enablePad4Global(const std::string& session_id, bool enabled) {
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
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	StringResponse setLowerFrequencyLimit(const std::string& session_id, double frequency) {
		zahner::SetLowerFrequencyLimitRequest request;
		request.set_session_id(session_id);
		request.set_frequency(frequency);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->SetLowerFrequencyLimit(&context, request, &response);
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	StringResponse setUpperFrequencyLimit(const std::string& session_id, double frequency) {
		zahner::SetUpperFrequencyLimitRequest request;
		request.set_session_id(session_id);
		request.set_frequency(frequency);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->SetUpperFrequencyLimit(&context, request, &response);
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	StringResponse setLowerNumberOfPeriods(const std::string& session_id, int32_t periods) {
		zahner::SetLowerNumberOfPeriodsRequest request;
		request.set_session_id(session_id);
		request.set_periods(periods);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->SetLowerNumberOfPeriods(&context, request, &response);
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	StringResponse setLowerStepsPerDecade(const std::string& session_id, int32_t steps) {
		zahner::SetLowerStepsPerDecadeRequest request;
		request.set_session_id(session_id);
		request.set_steps(steps);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->SetLowerStepsPerDecade(&context, request, &response);
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	StringResponse setUpperNumberOfPeriods(const std::string& session_id, int32_t periods) {
		zahner::SetUpperNumberOfPeriodsRequest request;
		request.set_session_id(session_id);
		request.set_periods(periods);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->SetUpperNumberOfPeriods(&context, request, &response);
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	StringResponse setScanDirection(const std::string& session_id, zahner::SetScanDirectionRequest::ScanDirection direction) {
		zahner::SetScanDirectionRequest request;
		request.set_session_id(session_id);
		request.set_direction(direction);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->SetScanDirection(&context, request, &response);
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	StringResponse setScanStrategy(const SetScanStrategyRequest request) {

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->SetScanStrategy(&context, request, &response);
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	StringResponse setUpperStepsPerDecade(const std::string& session_id, int32_t steps) {
		zahner::SetUpperStepsPerDecadeRequest request;
		request.set_session_id(session_id);
		request.set_steps(steps);

		zahner::StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));
		grpc::Status status = stub_->SetUpperStepsPerDecade(&context, request, &response);
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	StringResponse setStartFrequency(const std::string& session_id, double frequency) {
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
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}



	// Sends a request to calibrate offsets
	StringResponse calibrateOffsets(const std::string& session_id) {
		// Prepare request
		SessionRequest request;
		request.set_session_id(session_id);

		// Container for response
		StringResponse response;
		grpc::ClientContext context;


		// Remote Procedure Call
		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->CalibrateOffsets(&context, request, &response);
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	// Sends a request to set the potentiostat mode
	StringResponse setPotentiostatMode(const std::string& session_id, ModeRequest::PotentiostatMode mode) {
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
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	// Sends a request to set the potential value
	StringResponse setPotential(const std::string& session_id, double potential) {
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
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	// Sends a request to connect to the terminal
	ConnectResponse connectToTerm(const std::string& session_id, const std::string& host, ConnectRequest::Mode selected_mode) {
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
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	// Sends a request to connect to the terminal
	DisconnectResponse disconnectFromTerm(const std::string& session_id) {
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
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	StringResponse enablePotentiostat(const std::string& session_id, bool enable) {
		EnablePotentiostatRequest request;
		request.set_session_id(session_id);
		request.set_enable(enable);

		StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->EnablePotentiostat(&context, request, &response);

		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	StringResponse disablePotentiostat(const std::string& session_id) {
		SessionRequest request;
		request.set_session_id(session_id);

		StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->DisablePotentiostat(&context, request, &response);
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	StringResponse setFrequency(const std::string& session_id, double frequency) {
		FrequencyRequest request;
		request.set_session_id(session_id);
		request.set_frequency(frequency);

		StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->SetFrequency(&context, request, &response);
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	StringResponse setAmplitude(const std::string& session_id, double amplitude) {
		AmplitudeRequest request;
		request.set_session_id(session_id);
		request.set_amplitude(amplitude);

		StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->SetAmplitude(&context, request, &response);
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	StringResponse setNumberOfPeriods(const std::string& session_id, int32_t num_periods) {
		PeriodsRequest request;
		request.set_session_id(session_id);
		request.set_num_periods(num_periods);

		StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->SetNumberOfPeriods(&context, request, &response);
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	ImpedanceResponse getImpedance(const std::string& session_id) {
		ImpedanceRequest request;
		request.set_session_id(session_id);

		ImpedanceResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->GetImpedance(&context, request, &response);
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	ImpedanceResponse getImpedancePad4Simple(const ImpedancePad4SimpleRequest request) {

		ImpedanceResponse response;
		grpc::ClientContext context;


		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(60000));  // 5-second timeout
		grpc::Status status = stub_->GetImpedancePad4Simple(&context, request, &response);
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	ImpedanceResponse getImpedancePad4(const ImpedancePad4Request request) {

		ImpedanceResponse response;
		grpc::ClientContext context;


		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->GetImpedancePad4(&context, request, &response);
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	// Get the potential value
	PotentialResponse getPotential(const std::string& session_id) {
		SessionRequest request;
		request.set_session_id(session_id);

		PotentialResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->GetPotential(&context, request, &response);
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	// Get the current value
	CurrentResponse getCurrent(const std::string& session_id) {
		SessionRequest request;
		request.set_session_id(session_id);

		CurrentResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->GetCurrent(&context, request, &response);
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	// Sends a request to set the current device
	StringResponse selectPotentiostat(const std::string& session_id, int device) {
		SelectPotentiostatRequest request;
		request.set_session_id(session_id);
		request.set_device(device);

		StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(20000));  // 10-second timeout
		grpc::Status status = stub_->SelectPotentiostat(&context, request, &response);
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}

	// Sends a request to set the current value
	StringResponse setCurrent(const std::string& session_id, double current) {
		SetCurrentRequest request;
		request.set_session_id(session_id);
		request.set_current(current);

		StringResponse response;
		grpc::ClientContext context;

		context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5000));  // 5-second timeout
		grpc::Status status = stub_->SetCurrent(&context, request, &response);
		// Returns results based on RPC status
		if (status.ok()) {
			return response;
		}

		response.set_status(status.error_code());
		response.set_message(status.error_message());

		return response;
	}


private:
	std::unique_ptr<ZahnerZennium::Stub> stub_;
};

