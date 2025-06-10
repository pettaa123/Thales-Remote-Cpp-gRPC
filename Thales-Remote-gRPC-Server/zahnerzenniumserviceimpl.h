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
			return grpc::Status(grpc::StatusCode::INTERNAL, "Failed to establish connection.");
		}

		auto wrapper = connectionManager_.createWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::INTERNAL, "Wrapper initialization failed.");
		}
		response->set_session_id(request->session_id());

		return grpc::Status::OK;
	}

	grpc::Status DisconnectFromTerm(grpc::ServerContext* context, const SessionRequest* request, DisconnectResponse* response) override {
		connectionManager_.removeSession(request->session_id());
		return grpc::Status::OK;
	}

	grpc::Status MeasureEIS(grpc::ServerContext* context, const ::zahner::SessionRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->measureEIS();
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("MeasureEIS failed: ") + e.what());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetEISOutputPath(grpc::ServerContext* context, const ::zahner::SetEISOutputPathRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setEISOutputPath(request->path());
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("SetEISOutputPath failed: ") + e.what());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetEISOutputFileName(grpc::ServerContext* context, const ::zahner::SetEISOutputFileNameRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setEISOutputFileName(request->name());
			
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("SetEISOutputFileName failed: ") + e.what());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetLowerFrequencyLimit(grpc::ServerContext* context, const ::zahner::SetLowerFrequencyLimitRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setLowerFrequencyLimit(request->frequency());
			
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("SetLowerFrequencyLimit failed: ") + e.what());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetUpperFrequencyLimit(grpc::ServerContext* context, const ::zahner::SetUpperFrequencyLimitRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setUpperFrequencyLimit(request->frequency());
			
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("SetUpperFrequencyLimit failed: ") + e.what());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetLowerNumberOfPeriods(grpc::ServerContext* context, const ::zahner::SetLowerNumberOfPeriodsRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setLowerNumberOfPeriods(request->periods());
			
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("SetLowerNumberOfPeriods failed: ") + e.what());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetLowerStepsPerDecade(grpc::ServerContext* context, const ::zahner::SetLowerStepsPerDecadeRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setLowerStepsPerDecade(request->steps());
			
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("SetLowerStepsPerDecade failed: ") + e.what());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetUpperNumberOfPeriods(grpc::ServerContext* context, const ::zahner::SetUpperNumberOfPeriodsRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setUpperNumberOfPeriods(request->periods());
			
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("SetUpperNumberOfPeriods failed: ") + e.what());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetStartFrequency(grpc::ServerContext* context, const ::zahner::SetStartFrequencyRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setStartFrequency(request->frequency());
			
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("SetStartFrequencyRequest failed: ") + e.what());
		}
		return grpc::Status::OK;
	}

	grpc::Status SetScanDirection(grpc::ServerContext* context, const ::zahner::SetScanDirectionRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setScanDirection((ScanDirection)request->direction());
			
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("SetScanDirection failed: ") + e.what());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetScanStrategy(grpc::ServerContext* context, const ::zahner::SetScanStrategyRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setScanStrategy((ScanStrategy)request->strategy());
			
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("SetScanStrategy failed: ") + e.what());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetUpperStepsPerDecade(grpc::ServerContext* context, const ::zahner::SetUpperStepsPerDecadeRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setUpperStepsPerDecade(request->steps());
			
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("SetUpperStepsPerDecade failed: ") + e.what());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetupPad4ChannelWithVoltageRange(grpc::ServerContext* context, const ::zahner::SetupPad4ChannelWithVoltageRangeRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setupPad4ChannelWithVoltageRange(request->card(),request->channel(),request->enabled(),request->voltage_range());
			
			response->set_reply(reply);
		}
		catch (const ThalesRemoteError error) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("SetupPad4ChannelWithVoltageRange failed: ") + error.getMessage());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetupPad4ModeGlobal(grpc::ServerContext* context, const ::zahner::SetupPad4ModeGlobalRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setupPad4ModeGlobal((Pad4Mode)request->mode());
			
			response->set_reply(reply);
		}
		catch (const ThalesRemoteError error) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("SetupPad4ModeGlobal failed: ") + error.getMessage());
		}

		return grpc::Status::OK;
	}

	grpc::Status EnablePad4Global(grpc::ServerContext* context, const ::zahner::EnablePad4GlobalRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->enablePad4Global(request->enabled());			
			response->set_reply(reply);
		}
		catch (const ThalesRemoteError error) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("EnablePad4Global failed: ") + error.getMessage());
		}

		return grpc::Status::OK;
	}

	grpc::Status GetImpedancePad4Simple(grpc::ServerContext* context, const ::zahner::ImpedancePad4SimpleRequest* request, ImpedanceResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->getImpedancePad4(request->frequency());
			std::string timestamp = getISOCurrentTimestamp();
			std::complex<double> impedance; stringToComplex(reply);
			auto response_impedance = std::make_unique<ComplexNumber>();
			response_impedance->set_real(impedance.real());
			response_impedance->set_imag(impedance.imag());
			response->set_allocated_impedance(response_impedance.release());
			response->set_timestamp(timestamp);
		}
		catch (const ThalesRemoteError error) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("GetImpedancePad4Simple failed: ") + error.getMessage());
		}

		return grpc::Status::OK;
	}

	grpc::Status GetImpedancePad4(grpc::ServerContext* context, const ::zahner::ImpedancePad4Request* request, ImpedanceResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->getImpedancePad4(request->frequency(),request->amplitude(), request->num_periods());
			std::string timestamp = getISOCurrentTimestamp();
			std::complex<double> impedance; stringToComplex(reply);
			auto response_impedance = std::make_unique<ComplexNumber>();
			response_impedance->set_real(impedance.real());
			response_impedance->set_imag(impedance.imag());
			response->set_allocated_impedance(response_impedance.release());
			response->set_timestamp(timestamp);
		}
		catch (const ThalesRemoteError error) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("GetImpedancePad4 failed: ") + error.getMessage());
		}

		return grpc::Status::OK;
	}

	grpc::Status CalibrateOffsets(grpc::ServerContext* context, const ::zahner::SessionRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->calibrateOffsets();
			
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("Calibration failed: ") + e.what());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetPotentiostatMode(grpc::ServerContext* context, const ModeRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setPotentiostatMode(static_cast<PotentiostatMode>(request->mode()));
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("Setting potentiostat mode failed: ") + e.what());
		}

		return grpc::Status::OK;
	}

	grpc::Status SetPotential(grpc::ServerContext* context, const SetPotentialRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setPotential(request->potential());
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("Setting potential failed: ") + e.what());
		}

		return grpc::Status::OK;
	}

	// Enable or disable the potentiostat
	grpc::Status EnablePotentiostat(grpc::ServerContext* context, const EnablePotentiostatRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->enablePotentiostat(request->enable());
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("Failed to enable potentiostat: ") + e.what());
		}

		return grpc::Status::OK;
	}

	// Enable or disable the potentiostat
	grpc::Status DisablePotentiostat(grpc::ServerContext* context, const SessionRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->disablePotentiostat();
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("Failed to disable potentiostat: ") + e.what());
		}

		return grpc::Status::OK;
	}

	// Get the current potential value
	grpc::Status GetPotential(grpc::ServerContext* context, const SessionRequest* request, PotentialResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			double potential = wrapper->getPotential();
			response->set_potential(potential);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("Failed to retrieve potential: ") + e.what());
		}

		return grpc::Status::OK;
	}

	// Get the current value
	grpc::Status GetCurrent(grpc::ServerContext* context, const SessionRequest* request, CurrentResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			double current = wrapper->getCurrent();
			response->set_current(current);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("Failed to retrieve current: ") + e.what());
		}

		return grpc::Status::OK;
	}

	// Set the frequency
	grpc::Status SetFrequency(grpc::ServerContext* context, const FrequencyRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setFrequency(request->frequency());
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("Failed to set frequency: ") + e.what());
		}

		return grpc::Status::OK;
	}

	// Set the amplitude
	grpc::Status SetAmplitude(grpc::ServerContext* context, const AmplitudeRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setAmplitude(request->amplitude());
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("Failed to set amplitude: ") + e.what());
		}

		return grpc::Status::OK;
	}

	// Set the number of periods
	grpc::Status SetNumberOfPeriods(grpc::ServerContext* context, const PeriodsRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setNumberOfPeriods(request->num_periods());
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("Failed to set number of periods: ") + e.what());
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
			 std::string timestamp = getISOCurrentTimestamp();
			 auto response_impedance = std::make_unique<ComplexNumber>();
			 response_impedance->set_real(impedance.real());
			 response_impedance->set_imag(impedance.imag());
			 response->set_allocated_impedance(response_impedance.release());
			 response->set_timestamp(timestamp);
		}
		catch (const std::exception& e) {
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("Failed to retrieve impedance: ") + e.what());
		}

		return grpc::Status::OK;
	}

private:
	ConnectionManager& connectionManager_;


	//converts string in the form of "3.5-2.1i"  to complex<double>;
	std::complex<double> stringToComplex(const std::string& str) {
		double real = 0, imag = 0;
		char op = '+', suffix = 'i';

		std::istringstream iss(str);
		iss >> real >> op >> imag >> suffix;

		if (op == '-') imag = -imag; // Handle negative imaginary part

		return std::complex<double>(real, imag);
	}

	std::string getISOCurrentTimestamp() {
		const auto now = std::chrono::system_clock::now();
		return std::format("{:%FT%TZ}", now);
	}
};
