#pragma once


#include "connection_manager.h"
#include "thalesremoteerror.h"

using namespace zahner;

class ZahnerZenniumServiceImpl final : public ZahnerZennium::Service {
public:
	explicit ZahnerZenniumServiceImpl(ConnectionManager& manager) : connectionManager_(manager) {}

	grpc::Status ConnectToTerm(grpc::ServerContext* context, const ConnectRequest* request, ConnectResponse* response) override {
		auto connection = connectionManager_.createConnection(request->session_id(), request->host(), request->selected_mode());
		if (!connection) {
			response->set_success(false);
			response->set_message("Failed to connect.");
			return grpc::Status(grpc::StatusCode::INTERNAL, "Failed to establish connection.");
		}

		auto wrapper = connectionManager_.createWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to create ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::INTERNAL, "Wrapper initialization failed.");
		}

		response->set_success(true);
		response->set_message("Connected successfully and wrapper initialized.");
		response->set_session_id(request->session_id());

		return grpc::Status::OK;
	}

	grpc::Status DisconnectFromTerm(grpc::ServerContext* context, const SessionRequest* request, DisconnectResponse* response) override {
		connectionManager_.removeSession(request->session_id());
		response->set_success(true);
		response->set_message("Disconnected successfully and wrapper removed.");
		return grpc::Status::OK;
	}

	grpc::Status MeasureEIS(grpc::ServerContext* context, const ::zahner::SessionRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			std::string reply = wrapper->measureEIS();
			response->set_success(true);
			response->set_reply(reply);
			response->set_message("MeasureEIS completed successfully.");
		}
		catch (const std::exception& e) {
			response->set_success(false);
			response->set_message(std::string("MeasureEIS failed: ") + e.what());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetEISOutputPath(grpc::ServerContext* context, const ::zahner::SetEISOutputPathRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			std::string reply = wrapper->setEISOutputPath(request->path());
			response->set_success(true);
			response->set_reply(reply);
			response->set_message("SetEISOutputPath completed successfully.");
		}
		catch (const std::exception& e) {
			response->set_success(false);
			response->set_message(std::string("SetEISOutputPath failed: ") + e.what());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetEISOutputFileName(grpc::ServerContext* context, const ::zahner::SetEISOutputFileNameRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			std::string reply = wrapper->setEISOutputFileName(request->name());
			response->set_success(true);
			response->set_reply(reply);
			response->set_message("SetEISOutputFileName completed successfully.");
		}
		catch (const std::exception& e) {
			response->set_success(false);
			response->set_message(std::string("SetEISOutputFileName failed: ") + e.what());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetLowerFrequencyLimit(grpc::ServerContext* context, const ::zahner::SetLowerFrequencyLimitRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			std::string reply = wrapper->setLowerFrequencyLimit(request->frequency());
			response->set_success(true);
			response->set_reply(reply);
			response->set_message("SetLowerFrequencyLimit completed successfully.");
		}
		catch (const std::exception& e) {
			response->set_success(false);
			response->set_message(std::string("SetLowerFrequencyLimit failed: ") + e.what());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetUpperFrequencyLimit(grpc::ServerContext* context, const ::zahner::SetUpperFrequencyLimitRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			std::string reply = wrapper->setUpperFrequencyLimit(request->frequency());
			response->set_success(true);
			response->set_reply(reply);
			response->set_message("SetUpperFrequencyLimit completed successfully.");
		}
		catch (const std::exception& e) {
			response->set_success(false);
			response->set_message(std::string("SetUpperFrequencyLimit failed: ") + e.what());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetLowerNumberOfPeriods(grpc::ServerContext* context, const ::zahner::SetLowerNumberOfPeriodsRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			std::string reply = wrapper->setLowerNumberOfPeriods(request->periods());
			response->set_success(true);
			response->set_reply(reply);
			response->set_message("SetLowerNumberOfPeriods completed successfully.");
		}
		catch (const std::exception& e) {
			response->set_success(false);
			response->set_message(std::string("SetLowerNumberOfPeriods failed: ") + e.what());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetLowerStepsPerDecade(grpc::ServerContext* context, const ::zahner::SetLowerStepsPerDecadeRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			std::string reply = wrapper->setLowerStepsPerDecade(request->steps());
			response->set_success(true);
			response->set_reply(reply);
			response->set_message("SetLowerStepsPerDecade completed successfully.");
		}
		catch (const std::exception& e) {
			response->set_success(false);
			response->set_message(std::string("SetLowerStepsPerDecade failed: ") + e.what());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetUpperNumberOfPeriods(grpc::ServerContext* context, const ::zahner::SetUpperNumberOfPeriodsRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			std::string reply = wrapper->setUpperNumberOfPeriods(request->periods());
			response->set_success(true);
			response->set_reply(reply);
			response->set_message("SetUpperNumberOfPeriods completed successfully.");
		}
		catch (const std::exception& e) {
			response->set_success(false);
			response->set_message(std::string("SetUpperNumberOfPeriods failed: ") + e.what());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetStartFrequency(grpc::ServerContext* context, const ::zahner::SetStartFrequencyRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			std::string reply = wrapper->setStartFrequency(request->frequency());
			response->set_success(true);
			response->set_reply(reply);
			response->set_message("SetStartFrequencyRequest completed successfully.");
		}
		catch (const std::exception& e) {
			response->set_success(false);
			response->set_message(std::string("SetStartFrequencyRequest failed: ") + e.what());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetScanDirection(grpc::ServerContext* context, const ::zahner::SetScanDirectionRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}


		try {
			std::string reply = wrapper->setScanDirection((ScanDirection)request->direction());
			response->set_success(true);
			response->set_reply(reply);
			response->set_message("SetScanDirection completed successfully.");
		}
		catch (const std::exception& e) {
			response->set_success(false);
			response->set_message(std::string("SetScanDirection failed: ") + e.what());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetScanStrategy(grpc::ServerContext* context, const ::zahner::SetScanStrategyRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			std::string reply = wrapper->setScanStrategy((ScanStrategy)request->strategy());
			response->set_success(true);
			response->set_reply(reply);
			response->set_message("SetScanStrategy completed successfully.");
		}
		catch (const std::exception& e) {
			response->set_success(false);
			response->set_message(std::string("SetScanStrategy failed: ") + e.what());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetUpperStepsPerDecade(grpc::ServerContext* context, const ::zahner::SetUpperStepsPerDecadeRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			std::string reply = wrapper->setUpperStepsPerDecade(request->steps());
			response->set_success(true);
			response->set_reply(reply);
			response->set_message("SetUpperStepsPerDecade completed successfully.");
		}
		catch (const std::exception& e) {
			response->set_success(false);
			response->set_message(std::string("SetUpperStepsPerDecade failed: ") + e.what());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetupPad4ChannelWithVoltageRange(grpc::ServerContext* context, const ::zahner::SetupPad4ChannelWithVoltageRangeRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			std::string reply = wrapper->setupPad4ChannelWithVoltageRange(request->card(),request->channel(),request->enabled(),request->voltage_range());
			response->set_success(true);
			response->set_reply(reply);
			response->set_message("SetupPad4ChannelWithVoltageRange completed successfully.");
		}
		catch (const ThalesRemoteError error) {
			response->set_success(false);
			response->set_message(std::string("SetupPad4ChannelWithVoltageRange failed: ") + error.getMessage());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetupPad4ModeGlobal(grpc::ServerContext* context, const ::zahner::SetupPad4ModeGlobalRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			std::string reply = wrapper->setupPad4ModeGlobal((Pad4Mode)request->mode());
			response->set_success(true);
			response->set_reply(reply);
			response->set_message("SetupPad4ModeGlobal completed successfully.");
		}
		catch (const ThalesRemoteError error) {
			response->set_success(false);
			response->set_message(std::string("SetupPad4ModeGlobal failed: ") + error.getMessage());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	grpc::Status EnablePad4Global(grpc::ServerContext* context, const ::zahner::EnablePad4GlobalRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			std::string reply = wrapper->enablePad4Global(request->enabled());
			response->set_success(true);
			response->set_reply(reply);
			response->set_message("EnablePad4Global completed successfully.");
		}
		catch (const ThalesRemoteError error) {
			response->set_success(false);
			response->set_message(std::string("EnablePad4Globalfailed: ") + error.getMessage());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}


	grpc::Status CalibrateOffsets(grpc::ServerContext* context, const ::zahner::SessionRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			std::string reply = wrapper->calibrateOffsets();
			response->set_success(true);
			response->set_reply(reply);
			response->set_message("Calibration completed successfully.");
		}
		catch (const std::exception& e) {
			response->set_success(false);
			response->set_message(std::string("Calibration failed: ") + e.what());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetPotentiostatMode(grpc::ServerContext* context, const ModeRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			std::string reply = wrapper->setPotentiostatMode(static_cast<PotentiostatMode>(request->mode()));
			response->set_success(true);
			response->set_message("Potentiostat mode set successfully.");
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			response->set_success(false);
			response->set_message(std::string("Setting potentiostat mode failed: ") + e.what());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetPotential(grpc::ServerContext* context, const SetPotentialRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			std::string reply = wrapper->setPotential(request->potential());
			response->set_success(true);
			response->set_message("Potential set successfully.");
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			response->set_success(false);
			response->set_message(std::string("Setting potential failed: ") + e.what());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	// Enable or disable the potentiostat
	grpc::Status EnablePotentiostat(grpc::ServerContext* context, const EnablePotentiostatRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			std::string reply = wrapper->enablePotentiostat(request->enable());
			response->set_success(true);
			response->set_message(request->enable() ? "Potentiostat enabled." : "Potentiostat disabled.");
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			response->set_success(false);
			response->set_message(std::string("Failed to enable potentiostat: ") + e.what());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	// Enable or disable the potentiostat
	grpc::Status DisablePotentiostat(grpc::ServerContext* context, const SessionRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			std::string reply = wrapper->disablePotentiostat();
			response->set_success(true);
			response->set_message("Disabled potentiostat successfully");
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			response->set_success(false);
			response->set_message(std::string("Failed to disable potentiostat: ") + e.what());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	// Get the current potential value
	grpc::Status GetPotential(grpc::ServerContext* context, const SessionRequest* request, PotentialResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			double potential = wrapper->getPotential();
			response->set_success(true);
			response->set_message("Retrieved potential successfully.");
			response->set_potential(potential);
		}
		catch (const std::exception& e) {
			response->set_success(false);
			response->set_message(std::string("Failed to retrieve potential: ") + e.what());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	// Get the current value
	grpc::Status GetCurrent(grpc::ServerContext* context, const SessionRequest* request, CurrentResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			double current = wrapper->getCurrent();
			response->set_success(true);
			response->set_message("Retrieved current successfully.");
			response->set_current(current);
		}
		catch (const std::exception& e) {
			response->set_success(false);
			response->set_message(std::string("Failed to retrieve current: ") + e.what());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	// Set the frequency
	grpc::Status SetFrequency(grpc::ServerContext* context, const FrequencyRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			std::string reply = wrapper->setFrequency(request->frequency());
			response->set_success(true);
			response->set_message("Frequency set successfully.");
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			response->set_success(false);
			response->set_message(std::string("Failed to set frequency: ") + e.what());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	// Set the amplitude
	grpc::Status SetAmplitude(grpc::ServerContext* context, const AmplitudeRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			std::string reply = wrapper->setAmplitude(request->amplitude());
			response->set_success(true);
			response->set_message("Amplitude set successfully.");
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			response->set_success(false);
			response->set_message(std::string("Failed to set amplitude: ") + e.what());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	// Set the number of periods
	grpc::Status SetNumberOfPeriods(grpc::ServerContext* context, const PeriodsRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			response->set_success(false);
			response->set_message("Failed to retrieve ThalesRemoteScriptWrapper instance.");
			return grpc::Status(grpc::StatusCode::NOT_FOUND, response->message());
		}

		try {
			std::string reply = wrapper->setNumberOfPeriods(request->num_periods());
			response->set_success(true);
			response->set_message("Number of periods set successfully.");
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			response->set_success(false);
			response->set_message(std::string("Failed to set number of periods: ") + e.what());
			return grpc::Status(grpc::StatusCode::INTERNAL, response->message());
		}

		return grpc::Status::OK;
	}

	// Get impedance value
	grpc::Status GetImpedance(grpc::ServerContext* context, const ImpedanceRequest* request, ImpedanceResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			 std::complex<double> impedance = wrapper->getImpedance();
			 auto response_impedance = std::make_unique<ComplexNumber>();
			 response_impedance->set_real(impedance.real());
			 response_impedance->set_imag(impedance.imag());
			 response->set_allocated_impedance(response_impedance.release());
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("Failed to retrieve impedance: ") + e.what());
		}

		return grpc::Status::OK;
	}

private:
	ConnectionManager& connectionManager_;
};
