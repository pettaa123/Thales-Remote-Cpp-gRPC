#pragma once


#include "connection_manager.h"
#include "thalesremoteerror.h"

using namespace zahner;

class ZahnerZenniumServiceImpl final : public ZahnerZennium::Service {
public:
	explicit ZahnerZenniumServiceImpl(ConnectionManager& manager) : connectionManager_(manager) {}

	grpc::Status ConnectToTerm(grpc::ServerContext* context, const ConnectRequest* request, ConnectResponse* response) override {
		try {
			auto connection = connectionManager_.createConnection(request->session_id(), request->host(), request->selected_mode());
			auto wrapper = connectionManager_.createWrapper(request->session_id());
		}
		catch (std::exception& e) {
			std::cout << e.what() << " - ThalesXT App running?" << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, e.what());
		}
		response->set_session_id(request->session_id());
		std::cout << "Client Connected" << std::endl;

		return grpc::Status::OK;
	}

	grpc::Status DisconnectFromTerm(grpc::ServerContext* context, const SessionRequest* request, DisconnectResponse* response) override {
		try {
			connectionManager_.removeSession(request->session_id());
		}
		catch (std::exception& e) {
			std::cout << "Exception in DisconnectFromTerm: " << e.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("MeasureEIS failed: ") + e.what());
		}
		std::cout << "Client Disconnected" << std::endl;

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
			std::cout << "Exception in MeasureEIS: " << e.what() << std::endl;
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
			std::cout << "Exception in SetEISOutputPath: " << e.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("SetEISOutputPath failed: ") + e.what());
		}
		return grpc::Status::OK;
	}

	grpc::Status SetEISNaming(grpc::ServerContext* context, const ::zahner::SetEISNamingRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setEISNaming((NamingRule)request->naming());

			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			std::cout << "Exception in SetEISNaming: " << e.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("SetEISNamingRule failed: ") + e.what());
		}
		return grpc::Status::OK;
	}

	grpc::Status SetEISCounter(grpc::ServerContext* context, const ::zahner::SetEISCounterRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setEISCounter(request->number());

			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			std::cout << "Exception in SetEISCounter: " << e.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("SetEISCounter failed: ") + e.what());
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
			std::cout << "Exception in SetEISOutputFilename: " << e.what() << std::endl;
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
			std::cout << "Exception in SetLowerFrequencyLimit: " << e.what() << std::endl;
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
			std::cout << "Exception in SetUpperFrequencyLimit: " << e.what() << std::endl;
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
			std::cout << "Exception in SetLowerNumberOfPeriods: " << e.what() << std::endl;
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
			std::cout << "Exception in SetLowerStepsPerDecade: " << e.what() << std::endl;
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
			std::cout << "Exception in SetUpperNumberOfPeriods: " << e.what() << std::endl;
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
			std::cout << "Exception in SetStartFrequency: " << e.what() << std::endl;
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
			std::cout << "Exception in SetScanDirection: " << e.what() << std::endl;
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
			std::cout << "Exception in SetScanStrategy: " << e.what() << std::endl;
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
			std::cout << "Exception in SetUpperStepsPerDecade: " << e.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("SetUpperStepsPerDecade failed: ") + e.what());
		}
		return grpc::Status::OK;
	}

	grpc::Status SetupPad4Channel(grpc::ServerContext* context, const ::zahner::SetupPad4ChannelRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setupPad4Channel(request->card(), request->channel(), request->enabled());
			response->set_reply(reply);
		}
		catch (const ThalesRemoteError& error) {
			std::cout << "Exception in SetupPad4Channel: " << error.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("SetupPad4Channel failed: ") + error.getMessage());
		}
		return grpc::Status::OK;
	}
	/** Setting a single channel of a PAD4 card for an EIS measurement.
	*
	*  Each channel of the Pad4 card must be configured separately and then the PAD4 must be activated with
	* ThalesRemoteScriptWrapper::enablePAD4. Each channel can be given a different voltage range or shunt. The user can
	* switch the type of PAD4 channels between voltage sense (standard configuration) and current sense (with
	* additional shunt resistor).
	*
	* \param  card The number of the card starting at 1 and up to 4.
	* \param  channel The channel of the card starting at 1 and up to 4.
	* \param  enabled True to enable the channel.
	* \param  voltageRange input voltage range, if this differs from 4 V
	*
	* \return The response string from the device.
	*/
	grpc::Status SetupPad4ChannelWithVoltageRange(grpc::ServerContext* context, const ::zahner::SetupPad4ChannelWithVoltageRangeRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setupPad4ChannelWithVoltageRange(request->card(), request->channel(), request->enabled(), request->voltage_range());
			response->set_reply(reply);
		}
		catch (const ThalesRemoteError& error) {
			std::cout << "Exception in SetupPad4ChannelWithVoltageRange: " << error.what() << std::endl;
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
		catch (const ThalesRemoteError& error) {
			std::cout << "Exception in SetupPad4ModeGlobal: " << error.what() << std::endl;
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
		catch (const ThalesRemoteError& error) {
			std::cout << "Exception in EnablePad4Global: " << error.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("EnablePad4Global failed: ") + error.getMessage());
		}
		return grpc::Status::OK;
	}

	grpc::Status GetImpedancePad4Simple(grpc::ServerContext* context, const ::zahner::ImpedancePad4SimpleRequest* request, ImpedancePad4Response* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->getImpedancePad4(request->frequency());
			std::string timestamp = getISOCurrentTimestamp();
			std::vector<std::complex<double>> impedances = pad4StringToComplex(reply);

			auto* vector_response = response->mutable_impedances();
			for (const auto& z : impedances) {
				ComplexNumber* cn = vector_response->add_values();
				cn->set_real(z.real());
				cn->set_imag(z.imag());
			}

			response->set_timestamp(timestamp);
			response->set_timestamp(timestamp);
		}
		catch (const std::exception& e) {
			std::cout << "Exception in GetImpedancePad4Simple: " << e.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("GetImpedancePad4Simple failed: ") + e.what());
		}
		return grpc::Status::OK;
	}

	grpc::Status GetImpedancePad4(grpc::ServerContext* context, const ::zahner::ImpedancePad4Request* request, ImpedancePad4Response* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->getImpedancePad4(request->frequency(), request->amplitude(), request->num_periods());
			std::string timestamp = getISOCurrentTimestamp();
			std::vector<std::complex<double>> impedances = pad4StringToComplex(reply);
			
			auto* vector_response = response->mutable_impedances();
			for (const auto& z : impedances) {
				ComplexNumber* cn = vector_response->add_values();
				cn->set_real(z.real());
				cn->set_imag(z.imag());
			}
			
			response->set_timestamp(timestamp);
		}
		catch (const std::exception& e) {
			std::cout << "Exception in GetImpedancePad4: " << e.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("GetImpedancePad4 failed: ") + e.what());
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
			std::cout << "Exception in CalibrateOffsets: " << e.what() << std::endl;
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
			std::cout << "Exception in SetPotentioStatMode: " << e.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("Setting potentiostat mode failed: ") + e.what());
		}
		return grpc::Status::OK;
	}

	grpc::Status SetVoltageRangeIndex(grpc::ServerContext* context, const VoltageRangeIndexRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setVoltageRangeIndex(request->index());
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			std::cout << "Exception in SetVoltageRangeIndex: " << e.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("Setting VoltageRangeIndex failed: ") + e.what());
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
			std::cout << "Exception in SetPotential: " << e.what() << std::endl;
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
			std::cout << "Exception in EnablePotentiostat: " << e.what() << std::endl;
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
			std::cout << "Exception in DisablePotentiostat: " << e.what() << std::endl;
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
			std::string timestamp = getISOCurrentTimestamp();
			response->set_potential(potential);
			response->set_timestamp(timestamp);
		}
		catch (const std::exception& e) {
			std::cout << "Exception in GetPotential: " << e.what() << std::endl;
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
			std::string timestamp = getISOCurrentTimestamp();
			response->set_current(current);
			response->set_timestamp(timestamp);
		}
		catch (const std::exception& e) {
			std::cout << "Exception in GetCurrent: " << e.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("Failed to retrieve current: ") + e.what());
		}
		return grpc::Status::OK;
	}

	// Set the potentiostat device
	grpc::Status SelectPotentiostat(grpc::ServerContext* context, const SelectPotentiostatRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->selectPotentiostat(request->device());
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			std::cout << "Exception in SelectPotentiostat: " << e.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("Failed to set potentiostat device: ") + e.what());
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
			std::cout << "Exception in SetFrequency: " << e.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("Failed to set frequency: ") + e.what());
		}

		return grpc::Status::OK;
	}

	// Set the amplitude
	//Ampl = 5: Set AC amplitude to 5 mV in potentiostatic mode
	//	or 5 mA in galvanostatic mode
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
			std::cout << "Exception in SetAmplitude: " << e.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("Failed to set amplitude: ") + e.what());
		}

		return grpc::Status::OK;
	}

	// Set the current
	grpc::Status SetCurrent(grpc::ServerContext* context, const SetCurrentRequest* request, StringResponse* response) override {
		auto wrapper = connectionManager_.getWrapper(request->session_id());
		if (!wrapper) {
			return grpc::Status(grpc::StatusCode::NOT_FOUND, "Failed to retrieve ThalesRemoteScriptWrapper instance.");
		}

		try {
			std::string reply = wrapper->setCurrent(request->current());
			response->set_reply(reply);
		}
		catch (const std::exception& e) {
			std::cout << "Exception in SetCurrent: " << e.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("Failed to set current: ") + e.what());
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
			std::cout << "Exception in SetNumberOfPeriods: " << e.what();
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
			std::cout << "Exception in GetImpedance: " << e.what() << std::endl;
			return grpc::Status(grpc::StatusCode::INTERNAL, std::string("Failed to retrieve impedance: ") + e.what());
		}

		return grpc::Status::OK;
	}

private:
	ConnectionManager& connectionManager_;


	//converts string in the form of "3.5-2.1i"  to complex<double>;
	std::complex<double> stringToComplex(const std::string& str) {
		double real = 0, imag = 0;
		std::string label;

		std::istringstream iss(str);

		// Extract label and values
		std::getline(iss, label, '=');
		iss >> real;
		iss.ignore(); // Ignore the comma
		iss >> imag;

		return std::complex<double>(real, imag);
	}

	//converts pad4 return string in the form of:
	// impedance= -1.640e-02, 8.926e-03;pad01= -1.745e-01,-1.380e-01;pad02= -1.004e-01,-9.176e-02;pad03=  0.000e+00, 0.000e+00;
	// pad04=  0.000e+00, 0.000e+00;pad05=  0.000e+00, 0.000e+00;pad06=  0.000e+00, 0.000e+00;pad07=  0.000e+00, 0.000e+00;
	// pad08=  0.000e+00, 0.000e+00;pad09=  0.000e+00, 0.000e+00;pad10=  0.000e+00, 0.000e+00;pad11=  0.000e+00, 0.000e+00;
	// pad12=  0.000e+00, 0.000e+00;pad13=  0.000e+00, 0.000e+00;pad14=  0.000e+00, 0.000e+00;pad15=  0.000e+00, 0.000e+00;
	// pad16=  0.000e+00, 0.000e+00

	std::vector<std::complex<double>> pad4StringToComplex(const std::string& str) {
		double real = 0, imag = 0;
		// Temporary to store the splitted string
		std::string temp;

		std::istringstream iss(str);

		std::string inputLabel;

		std::vector<std::complex<double>> parsedInputs;
		//Reads a substring from iss(which holds the entire input string) up to the next ';' character.
		while (getline(iss, temp, ';')) {
			// Split single input
			std::istringstream singleInput(temp);
			// Extract label and values
			std::getline(singleInput, inputLabel, '=');
			singleInput >> real;
			singleInput.ignore(); // Ignore the comma
			singleInput >> imag;
			parsedInputs.push_back(std::complex<double>(real, imag));
		}

		return parsedInputs;
	}

	std::string getISOCurrentTimestamp() {
		const auto now = std::chrono::system_clock::now();
		return std::format("{:%FT%TZ}", now);
	}
};
