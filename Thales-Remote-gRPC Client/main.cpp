#include "thales-remote-grpc-client.h"

void RunSingleFrequencyImpedanceMeasurement(){
	std::string target_address("localhost:50051");
	ZahnerClient client(
		grpc::CreateChannel(target_address, grpc::InsecureChannelCredentials()));

	std::string session_id = "12345";
	std::string host = "localhost";  // Example host
	ConnectRequest::Mode mode = ConnectRequest::SCRIPT_REMOTE;  // Choose mode

	//ConnectRequest conRequest;
	//conRequest.set_session_id(session_id);
	//conRequest.set_host(host);
	//conRequest.set_selected_mode(mode);

	ConnectResponse cResponse = client.connectToTerm(session_id,host,mode);
	if (cResponse.status()) {
		std::cout << "Connection Response Status: " << cResponse.status() << std::endl;
		std::cout << "Connection Response Message: " << cResponse.message() << std::endl;
	}

	/*
	* Measure EIS spectra with a sequential number in the file name that has been specified.
	* Starting with number 1.
	*/

	StringResponse response = client.setEISNaming(session_id, SetEISNamingRequest_NamingRule_COUNTER);
	if (response.status()) {
		std::cout << "setEISNaming Status: " << response.status() << std::endl;
		std::cout << "setEISNaming Message: " << response.message() << std::endl;
	}

	response = client.setEISCounter(session_id, 1);
	if (response.status()) {
		std::cout << "setEISCounter Status: " << response.status() << std::endl;
		std::cout << "setEISCounter Message: " << response.message() << std::endl;
	}

	response = client.setEISOutputPath(session_id, "c:\\thales\\temp\\test1");
	if (response.status()) {
		std::cout << "setEISOutputPath Status: " << response.status() << std::endl;
		std::cout << "setEISOutputPath Message: " << response.message() << std::endl;
	}


	response = client.setEISOutputFileName(session_id, "spectra_cells");
	if (response.status()) {
		std::cout << "setEISOutputFileName Status: " << response.status() << std::endl;
		std::cout << "setEISOutputFileName Message: " << response.message() << std::endl;
	}

	/*
	* Setting the parameters for the spectra.
	* Alternatively a rule file can be used as a template.
	*/

	response = client.selectPotentiostat(session_id, 1);
	if (response.status()) {
		std::cout << "selectPotentiostat Status: " << response.status() << std::endl;
		std::cout << "selectPotentiostat Message: " << response.message() << std::endl;
	}

	response = client.setPotentiostatMode(session_id, ModeRequest::PotentiostatMode::ModeRequest_PotentiostatMode_GALVANOSTATIC);
	if (response.status()) {
		std::cout << "setPotentiostatMode Status: " << response.status() << std::endl;
		std::cout << "setPotentiostatMode Message: " << response.message() << std::endl;
	}

	response = client.setAmplitude(session_id, 1.0);
	if (response.status()) {
		std::cout << "setAmplitude Status: " << response.status() << std::endl;
		std::cout << "setAmplitude Message: " << response.message() << std::endl;
	}

	response = client.setCurrent(session_id, 2.0);
	if (response.status()) {
		std::cout << "setPotential Status: " << response.status() << std::endl;
		std::cout << "setPotential Message: " << response.message() << std::endl;
	}

	SetScanStrategyRequest sssRequest;
	sssRequest.set_session_id(session_id);
	sssRequest.set_strategy(SetScanStrategyRequest_ScanStrategy_SINGLE_SINE);

	response = client.setScanStrategy(sssRequest);
	if (response.status()) {
		std::cout << "setScanStrategy Status: " << response.status() << std::endl;
		std::cout << "setScanStrategy Message: " << response.message() << std::endl;
	}


	response = client.setNumberOfPeriods(session_id, 10);
	if (response.status()) {
		std::cout << "setNumberOfPeriods Status: " << response.status() << std::endl;
		std::cout << "setNumberOfPeriods Message: " << response.message() << std::endl;
	}

	/*
	* Single frequency measurement
	*/

	response = client.enablePotentiostat(session_id, true);
	if (response.status()) {
		std::cout << "enablePotentiostat Status: " << response.status() << std::endl;
		std::cout << "enablePotentiostat Message: " << response.message() << std::endl;
	}

	PotentialResponse potResponse;
	CurrentResponse curResponse;

	for (int i = 0; i < 5; i++) {
		potResponse = client.getPotential(session_id);
		if (potResponse.status()) {
			std::cout << "getPotential Status: " << response.status() << std::endl;
			std::cout << "getPotential Message: " << response.message() << std::endl;
			break;
		}
		std::cout << potResponse.potential() << std::endl;

		curResponse = client.getCurrent(session_id);
		if (curResponse.status()) {
			std::cout << "getCurrent Status: " << curResponse.status() << std::endl;
			std::cout << "getCurrent Message: " << curResponse.message() << std::endl;
			break;
		}
		std::cout << curResponse.current() << std::endl;
	}

	ImpedanceResponse impResponse;
	ImpedancePad4SimpleRequest impRequest;
	impRequest.set_session_id(session_id);
	impRequest.set_frequency(1000);

	for (size_t i = 0; i < 10; i++)
	{
		impResponse = client.getImpedancePad4Simple(impRequest);
		if (impResponse.status()) {
			std::cout << "getImpedancePad4 Status: " << impResponse.status() << std::endl;
			std::cout << "getImpedancePad4 Message: " << impResponse.message() << std::endl;
			break;
		}
		std::cout << "getImpedancePad4 real: " << impResponse.impedance().real() << std::endl;
		std::cout << "getImpedancePad4 imag: " << impResponse.impedance().imag() << std::endl;
	}

	response = client.setAmplitude(session_id, 0.0);
	if (response.status()) {
		std::cout << "setAmplitude Status: " << response.status() << std::endl;
		std::cout << "setAmplitude Message: " << response.message() << std::endl;
	}
	response = client.disablePotentiostat(session_id);

	if (response.status()) {
		std::cout << "disablePotentiostat Status: " << response.status() << std::endl;
		std::cout << "disablePotentiostat Message: " << response.message() << std::endl;
	}

	DisconnectResponse discResponse = client.disconnectFromTerm(session_id);

	if (response.status()) {
		std::cout << "disconnectResponse Status: " << response.status() << std::endl;
		std::cout << "disconnectResponse Message: " << response.message() << std::endl;
	}
}

void RunClient() {
	std::string target_address("localhost:50051");
	ZahnerClient client(
		grpc::CreateChannel(target_address, grpc::InsecureChannelCredentials()));

	std::string session_id = "12345";
	std::string host = "localhost";  // Example host
	ConnectRequest::Mode mode = ConnectRequest::SCRIPT_REMOTE;  // Choose mode

	ConnectResponse cResponse = client.connectToTerm(session_id, host, mode);
	if (cResponse.status()) {
		std::cout << "Connection Response Status: " << cResponse.status() << std::endl;
		std::cout << "Connection Response Message: " << cResponse.message() << std::endl;
	}

	StringResponse response;

	// Attempt calibration
	//std::string calibrationResponse = client.calibrateOffsets(session_id);
	//std::cout << "Calibration Response: " << calibrationResponse << std::endl;


	response = client.setPotentiostatMode(session_id, ModeRequest::PotentiostatMode::ModeRequest_PotentiostatMode_POTENTIOSTATIC);
	if (response.status()) {
		std::cout << "setPotentiostatMode Status: " << response.status() << std::endl;
		std::cout << "setPotentiostatMode Message: " << response.message() << std::endl;
	}

	response = client.setPotential(session_id, 1.0);
	if (response.status()) {
		std::cout << "setPotential Status: " << response.status() << std::endl;
		std::cout << "setPotential Message: " << response.message() << std::endl;
	}

	response = client.enablePotentiostat(session_id, true);
	if (response.status()) {
		std::cout << "enablePotentiostat Status: " << response.status() << std::endl;
		std::cout << "enablePotentiostat Message: " << response.message() << std::endl;
	}

	for (int i = 0; i < 5; i++) {
		std::cout << "Potential: " << client.getPotential(session_id).potential() << std::endl;
		std::cout << "Current: " << client.getCurrent(session_id).current() << std::endl;
	}

	response = client.disablePotentiostat(session_id);
	if (response.status()) {
		std::cout << "disablePotentiostat Status: " << response.status() << std::endl;
		std::cout << "disablePotentiostat Message: " << response.message() << std::endl;
	}

	response = client.setPotentiostatMode(session_id, ModeRequest::GALVANOSTATIC);
	if (response.status()) {
		std::cout << "setPotentiostatMode Status: " << response.status() << std::endl;
		std::cout << "setPotentiostatMode Message: " << response.message() << std::endl;
	}

	response = client.setCurrent(session_id, 20e-9);
	if (response.status()) {
		std::cout << "setCurrent Status: " << response.status() << std::endl;
		std::cout << "setCurrent Message: " << response.message() << std::endl;
	}

	response = client.enablePotentiostat(session_id, true);
	if (response.status()) {
		std::cout << "enablePotentiostat Status: " << response.status() << std::endl;
		std::cout << "enablePotentiostat Message: " << response.message() << std::endl;
	}

	for (int i = 0; i < 5; i++) {
		std::cout << "Potential: " << client.getPotential(session_id).potential() << std::endl;
		std::cout << "Current: " << client.getCurrent(session_id).current() << std::endl;
	}

	response = client.disablePotentiostat(session_id);
	if (response.status()) {
		std::cout << "disablePotentiostat Status: " << response.status() << std::endl;
		std::cout << "disablePotentiostat Message: " << response.message() << std::endl;
	}

	response = client.setPotentiostatMode(session_id, ModeRequest::PotentiostatMode::ModeRequest_PotentiostatMode_POTENTIOSTATIC);
	if (response.status()) {
		std::cout << "setPotentiostatMode Status: " << response.status() << std::endl;
		std::cout << "setPotentiostatMode Message: " << response.message() << std::endl;
	}

	response = client.setPotential(session_id, 1.0);
	if (response.status()) {
		std::cout << "setPotential Status: " << response.status() << std::endl;
		std::cout << "setPotential Message: " << response.message() << std::endl;
	}

	response = client.enablePotentiostat(session_id, true);
	if (response.status()) {
		std::cout << "enablePotentiostat Status: " << response.status() << std::endl;
		std::cout << "enablePotentiostat Message: " << response.message() << std::endl;
	}


	response = client.setFrequency(session_id, 2000);
	if (response.status()) {
		std::cout << "setFrequency Status: " << response.status() << std::endl;
		std::cout << "setFrequency Message: " << response.message() << std::endl;
	}

	response = client.setAmplitude(session_id, 10e-3);
	if (response.status()) {
		std::cout << "setAmplitude Status: " << response.status() << std::endl;
		std::cout << "setAmplitude Message: " << response.message() << std::endl;
	}

	response = client.setNumberOfPeriods(session_id, 3);
	if (response.status()) {
		std::cout << "setNumberOfPeriods Status: " << response.status() << std::endl;
		std::cout << "setNumberOfPeriods Message: " << response.message() << std::endl;
	}

	ImpedanceResponse impedance = client.getImpedance(session_id);
	if (impedance.status()) {
		std::cout << "getImpedance Status: " << impedance.status() << std::endl;
		std::cout << "getImpedance Message: " << impedance.message() << std::endl;
	}

	std::cout << "real: " << impedance.impedance().real() << "double: " << impedance.impedance().imag() << std::endl;

	response = client.disablePotentiostat(session_id);
	if (response.status()) {
		std::cout << "disablePotentiostat Status: " << response.status() << std::endl;
		std::cout << "disablePotentiostat Message: " << response.message() << std::endl;
	}

	response = client.setAmplitude(session_id, 0);
	if (response.status()) {
		std::cout << "setAmplitude Status: " << response.status() << std::endl;
		std::cout << "setAmplitude Message: " << response.message() << std::endl;
	}

	DisconnectResponse dResponse = client.disconnectFromTerm(session_id);
	if (response.status()) {
		std::cout << "DisconnectResponse Status: " << dResponse.status() << std::endl;
		std::cout << "DisconnectResponse Message: " << dResponse.message() << std::endl;
	}

	std::cout << "finish" << std::endl;

}

void RunFileExchangeExample() {
	std::string target_address("localhost:50051");
	ZahnerClient client(grpc::CreateChannel(target_address, grpc::InsecureChannelCredentials()));
	ThalesFileClient file_client(grpc::CreateChannel(target_address, grpc::InsecureChannelCredentials()));

	std::string session_id = "12345";
	std::string host = "localhost";  // Example host
	ConnectRequest::Mode mode = ConnectRequest::SCRIPT_REMOTE;  // Choose mode

	ConnectResponse cResponse = client.connectToTerm(session_id, host, mode);
	if (cResponse.status()) {
		std::cout << "Connection Response Status: " << cResponse.status() << std::endl;
		std::cout << "Connection Response Message: " << cResponse.message() << std::endl;
	}

	// Attempt calibration
//std::string calibrationResponse = client.calibrateOffsets(session_id);
//std::cout << "Calibration Response: " << calibrationResponse << std::endl;

	FileServiceConnectResponse fResponse = file_client.connectToTerm(session_id, host);
	if (fResponse.status()) {
		std::cout << "FileServiceConnectResponse Status: " << fResponse.status() << std::endl;
		std::cout << "FileServiceConnectResponse Message: " << fResponse.message() << std::endl;
	}

	/*
	 * Measure EIS spectra with a sequential number in the file name that has been specified.
	 * Starting with number 1.
	 */

	StringResponse response = client.setEISNaming(session_id, SetEISNamingRequest_NamingRule_COUNTER);
	if (response.status()) {
		std::cout << "setEISNaming Status: " << response.status() << std::endl;
		std::cout << "setEISNaming Message: " << response.message() << std::endl;
	}

	response = client.setEISCounter(session_id, 1);
	if (response.status()) {
		std::cout << "setEISCounter Status: " << response.status() << std::endl;
		std::cout << "setEISCounter Message: " << response.message() << std::endl;
	}

	response = client.setEISOutputPath(session_id, "c:\\thales\\temp\\test1");
	if (response.status()) {
		std::cout << "setEISOutputPath Status: " << response.status() << std::endl;
		std::cout << "setEISOutputPath Message: " << response.message() << std::endl;
	}


	response = client.setEISOutputFileName(session_id, "spectra_cells");
	if (response.status()) {
		std::cout << "setEISOutputFileName Status: " << response.status() << std::endl;
		std::cout << "setEISOutputFileName Message: " << response.message() << std::endl;
	}


	/*
	 * Setting the parameters for the spectra.
	 * Alternatively a rule file can be used as a template.
	 */

	response = client.setPotentiostatMode(session_id, ModeRequest::PotentiostatMode::ModeRequest_PotentiostatMode_POTENTIOSTATIC);
	if (response.status()) {
		std::cout << "setPotentiostatMode Status: " << response.status() << std::endl;
		std::cout << "setPotentiostatMode Message: " << response.message() << std::endl;
	}


	response = client.setAmplitude(session_id, 50e-3);
	if (response.status()) {
		std::cout << "setAmplitude Status: " << response.status() << std::endl;
		std::cout << "setAmplitude Message: " << response.message() << std::endl;
	}


	response = client.setPotential(session_id, 0);
	if (response.status()) {
		std::cout << "setPotential Status: " << response.status() << std::endl;
		std::cout << "setPotential Message: " << response.message() << std::endl;
	}


	response = client.setLowerFrequencyLimit(session_id, 500);
	if (response.status()) {
		std::cout << "setLowerFrequencyLimit Status: " << response.status() << std::endl;
		std::cout << "setLowerFrequencyLimit Message: " << response.message() << std::endl;
	}


	response = client.setStartFrequency(session_id, 1000);
	if (response.status()) {
		std::cout << "setStartFrequency Status: " << response.status() << std::endl;
		std::cout << "setStartFrequency Message: " << response.message() << std::endl;
	}


	response = client.setUpperFrequencyLimit(session_id, 2000);
	if (response.status()) {
		std::cout << "setUpperFrequencyLimit Status: " << response.status() << std::endl;
		std::cout << "setUpperFrequencyLimit Message: " << response.message() << std::endl;
	}


	response = client.setLowerNumberOfPeriods(session_id, 3);
	if (response.status()) {
		std::cout << "setLowerNumberOfPeriods Status: " << response.status() << std::endl;
		std::cout << "setLowerNumberOfPeriods Message: " << response.message() << std::endl;
	}


	response = client.setLowerStepsPerDecade(session_id, 5);
	if (response.status()) {
		std::cout << "setLowerStepsPerDecade Status: " << response.status() << std::endl;
		std::cout << "setLowerStepsPerDecade Message: " << response.message() << std::endl;
	}


	response = client.setUpperNumberOfPeriods(session_id, 20);
	if (response.status()) {
		std::cout << "setUpperNumberOfPeriods Status: " << response.status() << std::endl;
		std::cout << "setUpperNumberOfPeriods Message: " << response.message() << std::endl;
	}


	response = client.setUpperStepsPerDecade(session_id, 5);
	if (response.status()) {
		std::cout << "setUpperStepsPerDecade Status: " << response.status() << std::endl;
		std::cout << "setUpperStepsPerDecade Message: " << response.message() << std::endl;
	}


	response = client.setScanDirection(session_id, SetScanDirectionRequest::START_TO_MAX);
	if (response.status()) {
		std::cout << "setScanDirection Status: " << response.status() << std::endl;
		std::cout << "setScanDirection Message: " << response.message() << std::endl;
	}

	SetScanStrategyRequest sssRequest;
	sssRequest.set_session_id(session_id);
	sssRequest.set_strategy(SetScanStrategyRequest_ScanStrategy_SINGLE_SINE);

	response = client.setScanStrategy(sssRequest );
	if (response.status()) {
		std::cout << "setScanStrategy Status: " << response.status() << std::endl;
		std::cout << "setScanStrategy Message: " << response.message() << std::endl;
	}


	/*
	* Setup PAD4 Channels.
	* The PAD4 setup is encapsulated with try and catch to catch the exception if no PAD4 card is present.
	*/

	response = client.setupPad4ChannelWithVoltageRange(session_id, 1, 1, true, 4.0);
	if (response.status()) {
		std::cout << "setupPad4ChannelWithVoltageRange Status: " << response.status() << std::endl;
		std::cout << "setupPad4ChannelWithVoltageRange Message: " << response.message() << std::endl;
	}
	response = client.setupPad4ChannelWithVoltageRange(session_id, 1, 2, true, 4.0);
	if (response.status()) {
		std::cout << "setupPad4ChannelWithVoltageRange Status: " << response.status() << std::endl;
		std::cout << "setupPad4ChannelWithVoltageRange Message: " << response.message() << std::endl;
	}
	response = client.setupPad4ModeGlobal(session_id, SetupPad4ModeGlobalRequest::VOLTAGE);
	if (response.status()) {
		std::cout << "setupPad4ModeGlobal Status: " << response.status() << std::endl;
		std::cout << "setupPad4ModeGlobal Message: " << response.message() << std::endl;
	}
	response = client.enablePad4Global(session_id, true);
	if (response.status()) {
		std::cout << "enablePad4Global Status: " << response.status() << std::endl;
		std::cout << "enablePad4Global Message: " << response.message() << std::endl;
	}

	/*
	* Switching on the potentiostat before the measurement,
	* so that EIS is measured at the set DC potential.
	* If the potentiostat is off before the measurement,
	* the measurement is performed at the OCP.
	*/

	response = client.enablePotentiostat(session_id, true);
	if (response.status()) {
		std::cout << "enablePotentiostat Status: " << response.status() << std::endl;
		std::cout << "enablePotentiostat Message: " << response.message() << std::endl;
	}

	response = client.measureEIS(session_id);
	if (response.status()) {
		std::cout << "measureEIS Status: " << response.status() << std::endl;
		std::cout << "measureEIS Message: " << response.message() << std::endl;
	}

	FileObjectResponse file_obj_response = file_client.acquireFile(session_id, R"(C:\THALES\temp\test1\spectra_cells_0002_ser01.ism)");
	if (response.status()) {
		std::cout << "acquireFile Status: " << file_obj_response.status() << std::endl;
		std::cout << "acquireFile Message: " << file_obj_response.message() << std::endl;
	}

	response = file_client.enableKeepReceivedFilesInObject(session_id);
	if (response.status()) {
		std::cout << "enableKeepReceivedFilesInObject Status: " << response.status() << std::endl;
		std::cout << "enableKeepReceivedFilesInObject Message: " << response.message() << std::endl;
	}

	response = file_client.enableAutomaticFileExchange(session_id, true, "");
	if (response.status()) {
		std::cout << "enableAutomaticFileExchange Status: " << response.status() << std::endl;
		std::cout << "enableAutomaticFileExchange Message: " << response.message() << std::endl;
	}

	response = client.measureEIS(session_id);
	if (response.status()) {
		std::cout << "measureEIS Status: " << response.status() << std::endl;
		std::cout << "measureEIS Message: " << response.message() << std::endl;
	}

	response = client.disablePotentiostat(session_id);
	if (response.status()) {
		std::cout << "disablePotentiostat Status: " << response.status() << std::endl;
		std::cout << "disablePotentiostat Message: " << response.message() << std::endl;
	}

	DisconnectResponse dResponse = client.disconnectFromTerm(session_id);
	if (response.status()) {
		std::cout << "disconnectFromTerm Status: " << dResponse.status() << std::endl;
		std::cout << "disconnectFromTerm Message: " << dResponse.message() << std::endl;
	}

	response = file_client.disableAutomaticFileExchange(session_id);
	if (response.status()) {
		std::cout << "disableAutomaticFileExchange Status: " << response.status() << std::endl;
		std::cout << "disableAutomaticFileExchange Message: " << response.message() << std::endl;
	}

	std::cout << "Received Files: " << file_client.getReceivedFiles(session_id).files().size() << std::endl;

	response = file_client.deleteReceivedFiles(session_id);
	if (response.status()) {
		std::cout << "deleteReceivedFiles Status: " << response.status() << std::endl;
		std::cout << "deleteReceivedFiles Message: " << response.message() << std::endl;
	}

	dResponse = file_client.disconnectFromTerm(session_id);
	if (response.status()) {
		std::cout << "disconnectFromTerm Status: " << dResponse.status() << std::endl;
		std::cout << "disconnectFromTerm Message: " << dResponse.message() << std::endl;
	}
}

int main() {
	std::cout << "Starting Zahner Client...\n";
	RunSingleFrequencyImpedanceMeasurement();
	//RunClient();
	//RunFileExchangeExample();

	std::cout << "Press any key to exit...";
	std::cin.get();  // Waits for a key press

	return 0;
}