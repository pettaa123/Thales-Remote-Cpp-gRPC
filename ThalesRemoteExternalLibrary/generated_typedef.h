
/******************************************************************
 *  ____       __                        __    __   __      _ __
 * /_  / ___ _/ /  ___  ___ ___________ / /__ / /__/ /_____(_) /__
 *  / /_/ _ `/ _ \/ _ \/ -_) __/___/ -_) / -_)  '_/ __/ __/ /  '_/
 * /___/\_,_/_//_/_//_/\__/_/      \__/_/\__/_/\_\\__/_/ /_/_/\_\
 *
 * Copyright 2024 ZAHNER-elektrik I. Zahner-Schiller GmbH & Co. KG
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software
 * is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
 * THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef THALESREMOTEEXTERNALLIBRARY
#define THALESREMOTEEXTERNALLIBRARY

typedef void ZenniumConnection;
typedef void ThalesRemoteScriptWrapper;
typedef void ThalesFileInterface;

extern "C"
{
/**
 * @brief Retrieves the error message for a specific handle
 * 
 * This function retrieves the error message associated with a given handle.
 * It locks the mutex for thread-safety during access and copies the error message
 * to the provided buffer.
 *
 * @param[in]     handle    Pointer to the object whose error message is being retrieved
 * @param[out]    retval    Pointer to a buffer where the error message will be copied
 * @param[in,out] retvalLen Pointer to an integer that specifies the buffer size on input
 *                          and contains the actual length of the copied string (including 
 *                          null terminator) on output
 *
 * @return true if the error message was successfully retrieved, false otherwise
 *         When false is returned, "no_error" is copied to the buffer
 *
 * @note The function expects retvalLen to include space for the null terminator
 * @note The function ensures that the returned string is null-terminated
 */
__declspec(dllexport) bool __stdcall  getErrorMessage(void* handle, char* retval, int* retvalLen);
__declspec(dllexport) ZenniumConnection* __stdcall  createZenniumConnection();
__declspec(dllexport) void __stdcall  deleteZenniumConnection(ZenniumConnection* handle);
__declspec(dllexport) bool __stdcall  disconnectFromTerm(ZenniumConnection* handle);
__declspec(dllexport) bool __stdcall  connectToTerm(ZenniumConnection* handle, char const * address, char const* connectionName);
__declspec(dllexport) bool __stdcall  waitForBinaryTelegram(ZenniumConnection* handle, int message_type, char* retval, int* retvalLen);
__declspec(dllexport) bool __stdcall  waitForBinaryTelegramTimeout(ZenniumConnection* handle, int message_type, int timeout, char* retval, int* retvalLen);
__declspec(dllexport) bool __stdcall  setTimeout(ZenniumConnection* handle, int timeout);
__declspec(dllexport) bool __stdcall  getTimeout(ZenniumConnection* handle, int* timeout);
__declspec(dllexport) ThalesRemoteScriptWrapper* __stdcall  createThalesRemoteScriptWrapper(ZenniumConnection* handle);
__declspec(dllexport) void __stdcall deleteThalesRemoteScriptWrapper(ThalesRemoteScriptWrapper* handle);
__declspec(dllexport) ThalesFileInterface* __stdcall  createThalesFileInterface(ZenniumConnection* handle);
__declspec(dllexport) void __stdcall deleteThalesFileInterface(ThalesFileInterface* handle);
__declspec(dllexport) bool __stdcall enableAutomaticFileExchange(ThalesFileInterface* handle, char* retval, int* retvalLen, bool enable, char const* fileExtensions);
__declspec(dllexport) bool __stdcall disableAutomaticFileExchange(ThalesFileInterface* handle, char* retval, int* retvalLen);
__declspec(dllexport) bool __stdcall appendFilesToSkip(ThalesFileInterface* handle, char const* filename);
__declspec(dllexport) bool __stdcall setSavePath(ThalesFileInterface* handle, char const* path);
__declspec(dllexport) bool __stdcall enableSaveReceivedFilesToDisk(ThalesFileInterface* handle, char const* path, bool enable);
__declspec(dllexport) bool __stdcall disableSaveReceivedFilesToDisk(ThalesFileInterface* handle);

	
__declspec(dllexport) bool __stdcall forceThalesIntoRemoteScript(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall hideWindow(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall showWindow(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall getThalesVersion(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall getWorkstationHeartBeat(ThalesRemoteScriptWrapper* handle, int* retval );
	
__declspec(dllexport) bool __stdcall getCurrent(ThalesRemoteScriptWrapper* handle, double* retval );
	
__declspec(dllexport) bool __stdcall getPotential(ThalesRemoteScriptWrapper* handle, double* retval );
	
__declspec(dllexport) bool __stdcall getVoltage(ThalesRemoteScriptWrapper* handle, double* retval );
	
__declspec(dllexport) bool __stdcall setCurrent(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double current);
	
__declspec(dllexport) bool __stdcall setPotential(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double potential);
	
__declspec(dllexport) bool __stdcall setMaximumShuntIndex(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , int shunt);
	
__declspec(dllexport) bool __stdcall setMinimumShuntIndex(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , int shunt);
	
__declspec(dllexport) bool __stdcall setShuntIndex(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , int index);
	
__declspec(dllexport) bool __stdcall setVoltageRangeIndex(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , int index);
	
__declspec(dllexport) bool __stdcall selectPotentiostat(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , int device);
	
__declspec(dllexport) bool __stdcall selectPotentiostatWithoutPotentiostatStateChange(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , int device);
	
__declspec(dllexport) bool __stdcall switchToSCPIControl(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall switchToSCPIControlWithoutPotentiostatStateChange(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall getSerialNumber(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall getDeviceName(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall readSetup(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall calibrateOffsets(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall enablePotentiostat(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , bool enabled);
	
__declspec(dllexport) bool __stdcall disablePotentiostat(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall setPotentiostatMode(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , char const* potentiostatMode);
	
__declspec(dllexport) bool __stdcall enableRuleFileUsage(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , bool enabled);
	
__declspec(dllexport) bool __stdcall disableRuleFileUsage(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall setupPad4Channel(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , int card, int channel, bool enabled);
	
__declspec(dllexport) bool __stdcall setupPad4ChannelWithVoltageRange(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , int card, int channel, bool enabled, double voltageRange);
	
__declspec(dllexport) bool __stdcall setupPad4ChannelWithShuntResistor(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , int card, int channel, bool enabled, double shuntResistor);
	
__declspec(dllexport) bool __stdcall setupPad4ModeGlobal(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , char const* mode);
	
__declspec(dllexport) bool __stdcall enablePad4Global(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , bool enabled);
	
__declspec(dllexport) bool __stdcall disablePad4Global(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall readPAD4Setup(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall setFrequency(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double frequency);
	
__declspec(dllexport) bool __stdcall setAmplitude(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double amplitude);
	
__declspec(dllexport) bool __stdcall setNumberOfPeriods(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , int numberOfPeriods);
	
__declspec(dllexport) bool __stdcall setUpperFrequencyLimit(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double frequency);
	
__declspec(dllexport) bool __stdcall setLowerFrequencyLimit(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double frequency);
	
__declspec(dllexport) bool __stdcall setStartFrequency(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double frequency);
	
__declspec(dllexport) bool __stdcall setUpperStepsPerDecade(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , int steps);
	
__declspec(dllexport) bool __stdcall setLowerStepsPerDecade(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , int steps);
	
__declspec(dllexport) bool __stdcall setUpperNumberOfPeriods(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , int periods);
	
__declspec(dllexport) bool __stdcall setLowerNumberOfPeriods(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , int periods);
	
__declspec(dllexport) bool __stdcall setScanStrategy(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , char const* strategy);
	
__declspec(dllexport) bool __stdcall setScanDirection(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , char const* direction);
	
__declspec(dllexport) bool __stdcall getImpedance(ThalesRemoteScriptWrapper* handle, double* real, double* imag );
	
__declspec(dllexport) bool __stdcall getImpedancePad4(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall setEISNaming(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , char const* naming);
	
__declspec(dllexport) bool __stdcall setEISCounter(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , int number);
	
__declspec(dllexport) bool __stdcall setEISOutputPath(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , const char* path);
	
__declspec(dllexport) bool __stdcall setEISOutputFileName(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , const char* name);
	
__declspec(dllexport) bool __stdcall measureEIS(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall setCVStartPotential(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double potential);
	
__declspec(dllexport) bool __stdcall setCVUpperReversingPotential(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double potential);
	
__declspec(dllexport) bool __stdcall setCVLowerReversingPotential(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double potential);
	
__declspec(dllexport) bool __stdcall setCVEndPotential(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double potential);
	
__declspec(dllexport) bool __stdcall setCVStartHoldTime(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double time);
	
__declspec(dllexport) bool __stdcall setCVEndHoldTime(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double time);
	
__declspec(dllexport) bool __stdcall setCVScanRate(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double scanRate);
	
__declspec(dllexport) bool __stdcall setCVCycles(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double cycles);
	
__declspec(dllexport) bool __stdcall setCVSamplesPerCycle(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double samples);
	
__declspec(dllexport) bool __stdcall setCVMaximumCurrent(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double current);
	
__declspec(dllexport) bool __stdcall setCVMinimumCurrent(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double current);
	
__declspec(dllexport) bool __stdcall setCVOhmicDrop(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double ohmicDrop);
	
__declspec(dllexport) bool __stdcall enableCVAutoRestartAtCurrentOverflow(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , bool enabled);
	
__declspec(dllexport) bool __stdcall disableCVAutoRestartAtCurrentOverflow(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall enableCVAutoRestartAtCurrentUnderflow(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , bool enabled);
	
__declspec(dllexport) bool __stdcall disableCVAutoRestartAtCurrentUnderflow(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall enableCVAnalogFunctionGenerator(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , bool enabled);
	
__declspec(dllexport) bool __stdcall disableCVAnalogFunctionGenerator(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall setCVNaming(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , char const* naming);
	
__declspec(dllexport) bool __stdcall setCVCounter(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , int number);
	
__declspec(dllexport) bool __stdcall setCVOutputPath(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , const char* path);
	
__declspec(dllexport) bool __stdcall setCVOutputFileName(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , const char* name);
	
__declspec(dllexport) bool __stdcall checkCVSetup(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall readCVSetup(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall measureCV(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall setIEFirstEdgePotential(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double potential);
	
__declspec(dllexport) bool __stdcall setIESecondEdgePotential(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double potential);
	
__declspec(dllexport) bool __stdcall setIEThirdEdgePotential(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double potential);
	
__declspec(dllexport) bool __stdcall setIEFourthEdgePotential(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double potential);
	
__declspec(dllexport) bool __stdcall setIEFirstEdgePotentialRelation(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , char const* relation);
	
__declspec(dllexport) bool __stdcall setIESecondEdgePotentialRelation(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , char const* relation);
	
__declspec(dllexport) bool __stdcall setIEThirdEdgePotentialRelation(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , char const* relation);
	
__declspec(dllexport) bool __stdcall setIEFourthEdgePotentialRelation(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , char const* relation);
	
__declspec(dllexport) bool __stdcall setIEPotentialResolution(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double resolution);
	
__declspec(dllexport) bool __stdcall setIEMinimumWaitingTime(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double time);
	
__declspec(dllexport) bool __stdcall setIEMaximumWaitingTime(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double time);
	
__declspec(dllexport) bool __stdcall setIERelativeTolerance(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double tolerance);
	
__declspec(dllexport) bool __stdcall setIEAbsoluteTolerance(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double tolerance);
	
__declspec(dllexport) bool __stdcall setIEOhmicDrop(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double ohmicDrop);
	
__declspec(dllexport) bool __stdcall setIESweepMode(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , char const* sweepMode);
	
__declspec(dllexport) bool __stdcall setIEScanRate(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double scanRate);
	
__declspec(dllexport) bool __stdcall setIEMaximumCurrent(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double current);
	
__declspec(dllexport) bool __stdcall setIEMinimumCurrent(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double current);
	
__declspec(dllexport) bool __stdcall setIENaming(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , char const* naming);
	
__declspec(dllexport) bool __stdcall setIECounter(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , int number);
	
__declspec(dllexport) bool __stdcall setIEOutputPath(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , const char* path);
	
__declspec(dllexport) bool __stdcall setIEOutputFileName(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , const char* name);
	
__declspec(dllexport) bool __stdcall checkIESetup(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall readIESetup(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall measureIE(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall selectSequence(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , int number);
	
__declspec(dllexport) bool __stdcall setSequenceNaming(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , char const* naming);
	
__declspec(dllexport) bool __stdcall setSequenceCounter(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , int number);
	
__declspec(dllexport) bool __stdcall setSequenceOutputPath(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , const char* path);
	
__declspec(dllexport) bool __stdcall setSequenceOutputFileName(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , const char* name);
	
__declspec(dllexport) bool __stdcall enableSequenceAcqGlobal(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , bool state);
	
__declspec(dllexport) bool __stdcall disableSequenceAcqGlobal(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall enableSequenceAcqChannel(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , int channel, bool state);
	
__declspec(dllexport) bool __stdcall disableSequenceAcqChannel(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , int channel);
	
__declspec(dllexport) bool __stdcall readSequenceAcqSetup(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall runSequence(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall setSequenceOhmicDrop(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double value);
	
__declspec(dllexport) bool __stdcall setSequenceMaximumRuntime(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double value);
	
__declspec(dllexport) bool __stdcall setSequenceUpperPotentialLimit(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double value);
	
__declspec(dllexport) bool __stdcall setSequenceLowerPotentialLimit(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double value);
	
__declspec(dllexport) bool __stdcall setSequenceUpperCurrentLimit(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double value);
	
__declspec(dllexport) bool __stdcall setSequenceLowerCurrentLimit(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double value);
	
__declspec(dllexport) bool __stdcall setSequenceCurrentRange(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double value);
	
__declspec(dllexport) bool __stdcall setSequencePotentialLatencyWindow(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double value);
	
__declspec(dllexport) bool __stdcall setSequenceCurrentLatencyWindow(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double value);
	
__declspec(dllexport) bool __stdcall enableFraMode(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , bool enabled);
	
__declspec(dllexport) bool __stdcall setFraVoltageInputGain(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double value);
	
__declspec(dllexport) bool __stdcall setFraVoltageInputOffset(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double value);
	
__declspec(dllexport) bool __stdcall setFraVoltageOutputGain(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double value);
	
__declspec(dllexport) bool __stdcall setFraVoltageOutputOffset(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double value);
	
__declspec(dllexport) bool __stdcall setFraVoltageMinimum(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double value);
	
__declspec(dllexport) bool __stdcall setFraVoltageMaximum(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double value);
	
__declspec(dllexport) bool __stdcall setFraCurrentInputGain(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double value);
	
__declspec(dllexport) bool __stdcall setFraCurrentInputOffset(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double value);
	
__declspec(dllexport) bool __stdcall setFraCurrentOutputGain(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double value);
	
__declspec(dllexport) bool __stdcall setFraCurrentOutputOffset(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double value);
	
__declspec(dllexport) bool __stdcall setFraCurrentMinimum(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double value);
	
__declspec(dllexport) bool __stdcall setFraCurrentMaximum(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , double value);
	
__declspec(dllexport) bool __stdcall setFraPotentiostatMode(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , char const* potentiostatMode);
	
__declspec(dllexport) bool __stdcall readFraSetup(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall readAcqSetup(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall readAllAcqChannels(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );
	
__declspec(dllexport) bool __stdcall readAcqChannel(ThalesRemoteScriptWrapper* handle, double* retval , int channel);
	
__declspec(dllexport) bool __stdcall enableAcq(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen , bool enabled);
	
__declspec(dllexport) bool __stdcall disableAcq(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen );

}
#endif // THALESREMOTEEXTERNALLIBRARY
