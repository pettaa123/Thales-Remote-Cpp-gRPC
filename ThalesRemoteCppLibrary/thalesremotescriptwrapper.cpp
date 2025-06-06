﻿/******************************************************************
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

#include "thalesremotescriptwrapper.h"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include "termconnectionerror.h"
#include "thalesremoteerror.h"

template <typename T>
std::string to_string_with_precision(const T value, const int n = 6) {
    std::stringstream out;
    out << std::scientific << std::setprecision(n) << value;
    return out.str();
}

std::vector<int> tokenize(const std::string& str, char delim) {
    std::vector<int> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        tokens.push_back(std::stoi(token));
    }
    return tokens;
}

int compareVersions(const std::string& version1, const std::string& version2) {
    std::vector<int> v1 = tokenize(version1, '.');
    std::vector<int> v2 = tokenize(version2, '.');
    size_t i;
    for (i = 0; i < v1.size() && i < v2.size(); ++i) {
        if (v1[i] < v2[i]) {
            return -1;
        } else if (v1[i] > v2[i]) {
            return 1;
        }
    }
    if (v1.size() < v2.size()) {
        return -1;
    } else if (v1.size() > v2.size()) {
        return 1;
    } else {
        return 0;
    }
}

const std::string MINIMUM_THALES_VERSION = "5.9.2";

ThalesRemoteScriptWrapper::ThalesRemoteScriptWrapper(ZenniumConnection* const remoteConnection) :
    remoteConnection(remoteConnection) {
    bool versionOk = true;

    try {
        const auto versionReply = getThalesVersion();
        if (versionReply.find("devel") == std::string::npos) {
            std::regex replyStringPattern("(\\d+.\\d+.\\d+)");
            std::smatch match;

            if (std::regex_search(versionReply, match, replyStringPattern)) {
                const std::string versionString = match[0];
                const auto compResult           = compareVersions(versionString, MINIMUM_THALES_VERSION);
                if (compResult == -1) {
                    versionOk = false;
                }
            } else {
                versionOk = false;
            }
        }
    } catch (TermConnectionError e) {
        versionOk = false;
    }

    if (versionOk == false) {
        throw ThalesRemoteError("Please update your Thales version.");
    }
}

std::string ThalesRemoteScriptWrapper::executeRemoteCommand(std::string command) {
    return remoteConnection->sendStringAndWaitForReplyString("1:" + command + ":", 2);
}

std::string ThalesRemoteScriptWrapper::forceThalesIntoRemoteScript() {
    remoteConnection->sendStringAndWaitForReplyString(
        "3," + this->remoteConnection->getConnectionName() + ",0,OFF", 128
    );
    return remoteConnection->sendStringAndWaitForReplyString("2," + this->remoteConnection->getConnectionName(), 128);
}

std::string ThalesRemoteScriptWrapper::hideWindow() {
    return remoteConnection->sendStringAndWaitForReplyString(
        "3," + this->remoteConnection->getConnectionName() + ",5,off", 128
    );
}

std::string ThalesRemoteScriptWrapper::showWindow() {
    return remoteConnection->sendStringAndWaitForReplyString(
        "3," + this->remoteConnection->getConnectionName() + ",5,on", 128
    );
}

std::string ThalesRemoteScriptWrapper::getThalesVersion() {
    auto reply = remoteConnection->sendStringAndWaitForReplyString(
        "3," + this->remoteConnection->getConnectionName() + ",7", 128
    );

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    std::regex replyStringPattern(".*?,.*?,(.*)");
    std::smatch match;

    std::regex_search(reply, match, replyStringPattern);

    if (match.size() < 1) {
        throw ThalesRemoteError("Error with the serial number.");
    }

    return match.str(1);
}

int ThalesRemoteScriptWrapper::getWorkstationHeartBeat() {
    auto reply =
        remoteConnection->sendStringAndWaitForReplyString("1," + this->remoteConnection->getConnectionName(), 128);
    int result = -1;

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    std::regex replyStringPattern("^\\S*,\\S*,([\\d]*)$");
    std::smatch match;

    std::regex_search(reply, match, replyStringPattern);

    if (match.size() > 1) {
        result = this->stringToInt(match.str(1));
    }

    return result;
}

double ThalesRemoteScriptWrapper::getCurrent() {
    return this->requestValueAndParseUsingRegexp("CURRENT", std::regex("current=\\s*(.*?)A"));
}

double ThalesRemoteScriptWrapper::getPotential() {
    return this->requestValueAndParseUsingRegexp("POTENTIAL", std::regex("potential=\\s*(.*?)V"));
}

double ThalesRemoteScriptWrapper::getVoltage() {
    return this->getPotential();
}

std::string ThalesRemoteScriptWrapper::setCurrent(double current) {
    return this->setValue("Cset", current);
}

std::string ThalesRemoteScriptWrapper::setPotential(double potential) {
    return this->setValue("Pset", potential);
}

std::string ThalesRemoteScriptWrapper::setMaximumShuntIndex(int shunt) {
    return this->setValue("Rmax", shunt);
}

std::string ThalesRemoteScriptWrapper::setMinimumShuntIndex(int shunt) {
    return this->setValue("Rmin", shunt);
}

std::string ThalesRemoteScriptWrapper::setShuntIndex(int index) {
    this->setMinimumShuntIndex(index);
    return this->setMaximumShuntIndex(index);
}

std::string ThalesRemoteScriptWrapper::setVoltageRangeIndex(int index) {
    return this->setValue("Potrange", index);
}

std::string ThalesRemoteScriptWrapper::selectPotentiostat(int device) {
    return this->setValue("DEV%", device);
}

std::string ThalesRemoteScriptWrapper::selectPotentiostatWithoutPotentiostatStateChange(int device) {
    return this->setValue("DEVHOT%", device);
}

std::string ThalesRemoteScriptWrapper::switchToSCPIControl() {
    return this->executeRemoteCommand("SETUSB");
}

std::string ThalesRemoteScriptWrapper::switchToSCPIControlWithoutPotentiostatStateChange() {
    return this->executeRemoteCommand("HOT2USB");
}

std::string ThalesRemoteScriptWrapper::getSerialNumber() {
    std::string retval = "";
    std::string reply  = this->executeRemoteCommand("ALLNUM");

    std::regex replyStringPattern("(.*);(.*);([a-zA-Z]*)");
    std::smatch match;

    std::regex_search(reply, match, replyStringPattern);

    if (match.size() > 3) {
        retval = match.str(2);
    }
    return retval;
}

std::string ThalesRemoteScriptWrapper::getDeviceName() {
    std::string retval = "";
    std::string reply  = this->executeRemoteCommand("ALLNUM");

    std::regex replyStringPattern("(.*);(.*);([a-zA-Z]*)");
    std::smatch match;

    std::regex_search(reply, match, replyStringPattern);

    if (match.size() > 3) {
        retval = match.str(3);
    }
    return retval;
}

std::string ThalesRemoteScriptWrapper::readSetup() {
    return this->executeRemoteCommand("SENDSETUP");
}

std::string ThalesRemoteScriptWrapper::calibrateOffsets() {
    return this->executeRemoteCommand("CALOFFSETS");
}

std::string ThalesRemoteScriptWrapper::enablePotentiostat(bool enabled) {
    if (enabled == true) {
        return this->executeRemoteCommand("Pot=-1");
    } else {
        return this->executeRemoteCommand("Pot=0");
    }
}

std::string ThalesRemoteScriptWrapper::disablePotentiostat() {
    return this->enablePotentiostat(false);
}

std::string ThalesRemoteScriptWrapper::setPotentiostatMode(PotentiostatMode potentiostatMode) {
    std::string command;

    switch (potentiostatMode) {
        case PotentiostatMode::POTENTIOSTATIC:
            command = "Gal=0:GAL=0";
            break;
        case PotentiostatMode::GALVANOSTATIC:
            command = "Gal=-1:GAL=1";
            break;
        case PotentiostatMode::PSEUDOGALVANOSTATIC:
            command = "Gal=0:GAL=-1";
            break;
        default:
            return "error";
            break;
    }

    return this->executeRemoteCommand(command);
}

std::string ThalesRemoteScriptWrapper::enableRuleFileUsage(bool enabled) {
    return this->setValue("UseRuleFile", enabled);
}

std::string ThalesRemoteScriptWrapper::disableRuleFileUsage() {
    return this->enableRuleFileUsage(false);
}

std::string ThalesRemoteScriptWrapper::setupPad4Channel(int card, int channel, bool enabled) {
    int enable          = (enabled == true) ? 1 : 0;
    std::string command = "PAD4=" + std::to_string(card) + ";" + std::to_string(channel) + ";" + std::to_string(enable);
    auto reply          = this->executeRemoteCommand(command);

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    return reply;
}

std::string ThalesRemoteScriptWrapper::setupPad4ChannelWithVoltageRange(
    int card, int channel, bool enabled, double voltageRange
) {
    setupPad4Channel(card, channel, enabled);
    std::string command =
        "PAD4_PRANGE=" + std::to_string(card) + ";" + std::to_string(channel) + ";" + std::to_string(voltageRange);
    auto reply = this->executeRemoteCommand(command);

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    return reply;
}

std::string ThalesRemoteScriptWrapper::setupPad4ChannelWithShuntResistor(
    int card, int channel, bool enabled, double shuntResistor
) {
    setupPad4Channel(card, channel, enabled);
    std::string command =
        "PAD4_RSHUNT=" + std::to_string(card) + ";" + std::to_string(channel) + ";" + std::to_string(shuntResistor);
    auto reply = this->executeRemoteCommand(command);

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    return reply;
}

std::string ThalesRemoteScriptWrapper::setupPad4ModeGlobal(Pad4Mode mode) {
    int intmode;
    switch (mode) {
        default:
        case Pad4Mode::VOLTAGE:
            intmode = 0;
            break;
        case Pad4Mode::CURRENT:
            intmode = 1;
            break;
    }
    return this->setValue("PAD4MOD", intmode);
}

std::string ThalesRemoteScriptWrapper::enablePad4Global(bool enabled) {
    return this->setValue("PAD4ENA", enabled);
}

std::string ThalesRemoteScriptWrapper::disablePad4Global() {
    return this->enablePad4Global(false);
}

std::string ThalesRemoteScriptWrapper::readPAD4Setup() {
    auto reply = this->executeRemoteCommand("SENDPAD4SETUP");

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    return reply;
}

std::string ThalesRemoteScriptWrapper::setFrequency(double frequency) {
    return this->setValue("Frq", frequency);
}

std::string ThalesRemoteScriptWrapper::setAmplitude(double amplitude) {
    return this->setValue("Ampl", amplitude * 1e3);
}

std::string ThalesRemoteScriptWrapper::setNumberOfPeriods(int numberOfPeriods) {
    // little bits of stability

    if (numberOfPeriods > 100) {
        numberOfPeriods = 100;
    }

    if (numberOfPeriods < 1) {
        numberOfPeriods = 1;
    }

    return this->setValue("Nw", numberOfPeriods);
}

std::string ThalesRemoteScriptWrapper::setUpperFrequencyLimit(double frequency) {
    return this->setValue("Fmax", frequency);
}

std::string ThalesRemoteScriptWrapper::setLowerFrequencyLimit(double frequency) {
    return this->setValue("Fmin", frequency);
}

std::string ThalesRemoteScriptWrapper::setStartFrequency(double frequency) {
    return this->setValue("Fstart", frequency);
}

std::string ThalesRemoteScriptWrapper::setUpperStepsPerDecade(int steps) {
    return this->setValue("dfm", steps);
}

std::string ThalesRemoteScriptWrapper::setLowerStepsPerDecade(int steps) {
    return this->setValue("dfl", steps);
}

std::string ThalesRemoteScriptWrapper::setUpperNumberOfPeriods(int periods) {
    return this->setValue("Nws", periods);
}

std::string ThalesRemoteScriptWrapper::setLowerNumberOfPeriods(int periods) {
    return this->setValue("Nwl", periods);
}

std::string ThalesRemoteScriptWrapper::setScanStrategy(ScanStrategy strategy) {
    int strategyInt;
    switch (strategy) {
        default:
        case ScanStrategy::SINGLE_SINE:
            strategyInt = 0;
            break;
        case ScanStrategy::MULTI_SINE:
            strategyInt = 1;
            break;
        case ScanStrategy::TABLE:
            strategyInt = 2;
            break;
    }
    return this->setValue("ScanStrategy", strategyInt);
}

std::string ThalesRemoteScriptWrapper::setScanDirection(ScanDirection direction) {
    int directionInt;
    switch (direction) {
        default:
        case ScanDirection::START_TO_MAX:
            directionInt = 0;
            break;
        case ScanDirection::START_TO_MIN:
            directionInt = 1;
            break;
    }
    return this->setValue("ScanDirection", directionInt);
}

std::complex<double> ThalesRemoteScriptWrapper::getImpedance() {
    std::complex<double> result(std::nan("1"), std::nan("1"));

    std::string reply = this->executeRemoteCommand("IMPEDANCE");

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    std::regex replyStringPattern("impedance=\\s*(.*?),\\s*(.*?)\\\r");
    std::smatch match;
    std::regex_search(reply, match, replyStringPattern);

    if (match.size() > 2) {
        result = std::complex<double>(this->stringToDobule(match.str(1)), this->stringToDobule(match.str(2)));
    }

    return result;
}

std::complex<double> ThalesRemoteScriptWrapper::getImpedance(double frequency) {
    this->setFrequency(frequency);

    return this->getImpedance();
}

std::complex<double> ThalesRemoteScriptWrapper::getImpedance(double frequency, double amplitude, int numberOfPeriods) {
    this->setFrequency(frequency);
    this->setAmplitude(amplitude);
    this->setNumberOfPeriods(numberOfPeriods);

    return this->getImpedance();
}

std::string ThalesRemoteScriptWrapper::getImpedancePad4() {
    std::string reply = this->executeRemoteCommand("PAD4IMP");

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    return reply;
}

std::string ThalesRemoteScriptWrapper::getImpedancePad4(double frequency) {
    this->setFrequency(frequency);

    return this->getImpedancePad4();
}

std::string ThalesRemoteScriptWrapper::getImpedancePad4(double frequency, double amplitude, int numberOfPeriods) {
    this->setFrequency(frequency);
    this->setAmplitude(amplitude);
    this->setNumberOfPeriods(numberOfPeriods);

    return this->getImpedancePad4();
}

std::string ThalesRemoteScriptWrapper::setEISNaming(NamingRule naming) {
    int namingInt;
    switch (naming) {
        default:
        case NamingRule::DATETIME:
            namingInt = 0;
            break;
        case NamingRule::COUNTER:
            namingInt = 1;
            break;
        case NamingRule::INDIVIDUAL:
            namingInt = 2;
            break;
    }
    return this->setValue("EIS_MOD", namingInt);
}

std::string ThalesRemoteScriptWrapper::setEISCounter(int number) {
    return this->setValue("EIS_NUM", number);
}

std::string ThalesRemoteScriptWrapper::setEISOutputPath(std::string path) {
    transform(path.begin(), path.end(), path.begin(), ::tolower);
    return this->setValue("EIS_PATH", path);
}

std::string ThalesRemoteScriptWrapper::setEISOutputFileName(std::string name) {
    return this->setValue("EIS_ROOT", name);
}

std::string ThalesRemoteScriptWrapper::measureEIS() {
    auto reply = this->executeRemoteCommand("EIS");

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    return reply;
}

std::string ThalesRemoteScriptWrapper::setCVStartPotential(double potential) {
    return this->setValue("CV_Pstart", potential);
}

std::string ThalesRemoteScriptWrapper::setCVUpperReversingPotential(double potential) {
    return this->setValue("CV_Pupper", potential);
}

std::string ThalesRemoteScriptWrapper::setCVLowerReversingPotential(double potential) {
    return this->setValue("CV_Plower", potential);
}

std::string ThalesRemoteScriptWrapper::setCVEndPotential(double potential) {
    return this->setValue("CV_Pend", potential);
}

std::string ThalesRemoteScriptWrapper::setCVStartHoldTime(double time) {
    return this->setValue("CV_Tstart", time);
}

std::string ThalesRemoteScriptWrapper::setCVEndHoldTime(double time) {
    return this->setValue("CV_Tend", time);
}

std::string ThalesRemoteScriptWrapper::setCVScanRate(double scanRate) {
    return this->setValue("CV_Srate", scanRate);
}

std::string ThalesRemoteScriptWrapper::setCVCycles(double cycles) {
    return this->setValue("CV_Periods", cycles);
}

std::string ThalesRemoteScriptWrapper::setCVSamplesPerCycle(double samples) {
    return this->setValue("CV_PpPer", samples);
}

std::string ThalesRemoteScriptWrapper::setCVMaximumCurrent(double current) {
    return this->setValue("CV_Ima", current);
}

std::string ThalesRemoteScriptWrapper::setCVMinimumCurrent(double current) {
    return this->setValue("CV_Imi", current);
}

std::string ThalesRemoteScriptWrapper::setCVOhmicDrop(double ohmicDrop) {
    return this->setValue("CV_Odrop", ohmicDrop);
}

std::string ThalesRemoteScriptWrapper::enableCVAutoRestartAtCurrentOverflow(bool enabled) {
    return this->setValue("CV_AutoReStart", enabled);
}

std::string ThalesRemoteScriptWrapper::disableCVAutoRestartAtCurrentOverflow() {
    return this->enableCVAutoRestartAtCurrentOverflow(false);
}

std::string ThalesRemoteScriptWrapper::enableCVAutoRestartAtCurrentUnderflow(bool enabled) {
    return this->setValue("CV_AutoScale", enabled);
}

std::string ThalesRemoteScriptWrapper::disableCVAutoRestartAtCurrentUnderflow() {
    return this->enableCVAutoRestartAtCurrentUnderflow(false);
}

std::string ThalesRemoteScriptWrapper::enableCVAnalogFunctionGenerator(bool enabled) {
    return this->setValue("CV_AFGena", enabled);
}

std::string ThalesRemoteScriptWrapper::disableCVAnalogFunctionGenerator() {
    return this->enableCVAnalogFunctionGenerator(false);
}

std::string ThalesRemoteScriptWrapper::setCVNaming(NamingRule naming) {
    int namingInt;
    switch (naming) {
        default:
        case NamingRule::DATETIME:
            namingInt = 0;
            break;
        case NamingRule::COUNTER:
            namingInt = 1;
            break;
        case NamingRule::INDIVIDUAL:
            namingInt = 2;
            break;
    }
    return this->setValue("CV_MOD", namingInt);
}

std::string ThalesRemoteScriptWrapper::setCVCounter(int number) {
    return this->setValue("CV_NUM", number);
}

std::string ThalesRemoteScriptWrapper::setCVOutputPath(std::string path) {
    transform(path.begin(), path.end(), path.begin(), ::tolower);
    return this->setValue("CV_PATH", path);
}

std::string ThalesRemoteScriptWrapper::setCVOutputFileName(std::string name) {
    return this->setValue("CV_ROOT", name);
}

std::string ThalesRemoteScriptWrapper::checkCVSetup() {
    auto reply = this->executeRemoteCommand("CHECKCV");

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    return reply;
}

std::string ThalesRemoteScriptWrapper::readCVSetup() {
    auto reply = this->executeRemoteCommand("SENDCVSETUP");

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    return reply;
}

std::string ThalesRemoteScriptWrapper::measureCV() {
    auto reply = this->executeRemoteCommand("CV");

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    return reply;
}

std::string ThalesRemoteScriptWrapper::setIEFirstEdgePotential(double potential) {
    return this->setValue("IE_EckPot1", potential);
}

std::string ThalesRemoteScriptWrapper::setIESecondEdgePotential(double potential) {
    return this->setValue("IE_EckPot2", potential);
}

std::string ThalesRemoteScriptWrapper::setIEThirdEdgePotential(double potential) {
    return this->setValue("IE_EckPot3", potential);
}

std::string ThalesRemoteScriptWrapper::setIEFourthEdgePotential(double potential) {
    return this->setValue("IE_EckPot4", potential);
}

std::string ThalesRemoteScriptWrapper::setIEFirstEdgePotentialRelation(PotentialRelation relation) {
    return this->setValue("IE_EckPot1rel", relation);
}

std::string ThalesRemoteScriptWrapper::setIESecondEdgePotentialRelation(PotentialRelation relation) {
    return this->setValue("IE_EckPot2rel", relation);
}

std::string ThalesRemoteScriptWrapper::setIEThirdEdgePotentialRelation(PotentialRelation relation) {
    return this->setValue("IE_EckPot3rel", relation);
}

std::string ThalesRemoteScriptWrapper::setIEFourthEdgePotentialRelation(PotentialRelation relation) {
    return this->setValue("IE_EckPot4rel", relation);
}

std::string ThalesRemoteScriptWrapper::setIEPotentialResolution(double resolution) {
    return this->setValue("IE_Resolution", resolution);
}

std::string ThalesRemoteScriptWrapper::setIEMinimumWaitingTime(double time) {
    return this->setValue("IE_WZmin", time);
}

std::string ThalesRemoteScriptWrapper::setIEMaximumWaitingTime(double time) {
    return this->setValue("IE_WZmax", time);
}

std::string ThalesRemoteScriptWrapper::setIERelativeTolerance(double tolerance) {
    return this->setValue("IE_Torel", tolerance);
}

std::string ThalesRemoteScriptWrapper::setIEAbsoluteTolerance(double tolerance) {
    return this->setValue("IE_Toabs", tolerance);
}

std::string ThalesRemoteScriptWrapper::setIEOhmicDrop(double ohmicDrop) {
    return this->setValue("IE_Odrop", ohmicDrop);
}

std::string ThalesRemoteScriptWrapper::setIESweepMode(IESweepMode sweepMode) {
    int sweepModeInt;
    switch (sweepMode) {
        default:
        case IESweepMode::DYNAMICSCAN:
            sweepModeInt = 2;
            break;
        case IESweepMode::FIXEDSAMPLING:
            sweepModeInt = 1;
            break;
        case IESweepMode::STEADYSTATE:
            sweepModeInt = 0;
            break;
    }
    return this->setValue("IE_SweepMode", sweepModeInt);
}

std::string ThalesRemoteScriptWrapper::setIEScanRate(double scanRate) {
    return this->setValue("IE_Srate", scanRate);
}

std::string ThalesRemoteScriptWrapper::setIEMaximumCurrent(double current) {
    return this->setValue("IE_Ima", current);
}

std::string ThalesRemoteScriptWrapper::setIEMinimumCurrent(double current) {
    return this->setValue("IE_Imi", current);
}

std::string ThalesRemoteScriptWrapper::setIENaming(NamingRule naming) {
    int namingInt;
    switch (naming) {
        default:
        case NamingRule::DATETIME:
            namingInt = 0;
            break;
        case NamingRule::COUNTER:
            namingInt = 1;
            break;
        case NamingRule::INDIVIDUAL:
            namingInt = 2;
            break;
    }
    return this->setValue("IE_MOD", namingInt);
}

std::string ThalesRemoteScriptWrapper::setIECounter(int number) {
    return this->setValue("IE_NUM", number);
}

std::string ThalesRemoteScriptWrapper::setIEOutputPath(std::string path) {
    transform(path.begin(), path.end(), path.begin(), ::tolower);
    return this->setValue("IE_PATH", path);
}

std::string ThalesRemoteScriptWrapper::setIEOutputFileName(std::string name) {
    return this->setValue("IE_ROOT", name);
}

std::string ThalesRemoteScriptWrapper::checkIESetup() {
    auto reply = this->executeRemoteCommand("CHECKIE");

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    return reply;
}

std::string ThalesRemoteScriptWrapper::readIESetup() {
    auto reply = this->executeRemoteCommand("SENDIESETUP");

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    return reply;
}

std::string ThalesRemoteScriptWrapper::measureIE() {
    auto reply = this->executeRemoteCommand("IE");

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    return reply;
}

std::string ThalesRemoteScriptWrapper::selectSequence(int number) {
    auto reply = this->executeRemoteCommand("SELSEQ=" + std::to_string(number));

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    return reply;
}

std::string ThalesRemoteScriptWrapper::setSequenceNaming(NamingRule naming) {
    int namingInt;
    switch (naming) {
        default:
        case NamingRule::DATETIME:
            namingInt = 0;
            break;
        case NamingRule::COUNTER:
            namingInt = 1;
            break;
        case NamingRule::INDIVIDUAL:
            namingInt = 2;
            break;
    }
    return this->setValue("SEQ_MOD", namingInt);
}

std::string ThalesRemoteScriptWrapper::setSequenceCounter(int number) {
    return this->setValue("SEQ_NUM", number);
}

std::string ThalesRemoteScriptWrapper::setSequenceOutputPath(std::string path) {
    transform(path.begin(), path.end(), path.begin(), ::tolower);
    return this->setValue("SEQ_PATH", path);
}

std::string ThalesRemoteScriptWrapper::setSequenceOutputFileName(std::string name) {
    return this->setValue("SEQ_ROOT", name);
}

std::string ThalesRemoteScriptWrapper::enableSequenceAcqGlobal(bool state) {
    return this->setValue("SEQ_ACQENA", state == true ? -1 : 0);
}

std::string ThalesRemoteScriptWrapper::disableSequenceAcqGlobal() {
    return this->enableSequenceAcqGlobal(false);
}

std::string ThalesRemoteScriptWrapper::enableSequenceAcqChannel(int channel, bool state) {
    std::string command = "SEQ_ACQ=" + std::to_string(channel) + ";" + std::to_string(state == true ? 1 : 0);
    auto reply          = this->executeRemoteCommand(command);

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    return reply;
}

std::string ThalesRemoteScriptWrapper::disableSequenceAcqChannel(int channel) {
    return this->enableSequenceAcqChannel(channel, false);
}

std::string ThalesRemoteScriptWrapper::readSequenceAcqSetup() {
    return this->executeRemoteCommand("SENDSEQACQSETUP");
}

std::string ThalesRemoteScriptWrapper::runSequence() {
    auto reply = this->executeRemoteCommand("DOSEQ");

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    return reply;
}

std::string ThalesRemoteScriptWrapper::setSequenceOhmicDrop(double value) {
    return this->setValue("SEQ_RODROP", value);
}

std::string ThalesRemoteScriptWrapper::setSequenceMaximumRuntime(double value) {
    return this->setValue("SEQ_MAXTIME", value);
}

std::string ThalesRemoteScriptWrapper::setSequenceUpperPotentialLimit(double value) {
    return this->setValue("SEQ_EUPPER", value);
}

std::string ThalesRemoteScriptWrapper::setSequenceLowerPotentialLimit(double value) {
    return this->setValue("SEQ_ELOWER", value);
}

std::string ThalesRemoteScriptWrapper::setSequenceUpperCurrentLimit(double value) {
    return this->setValue("SEQ_IUPPER", value);
}

std::string ThalesRemoteScriptWrapper::setSequenceLowerCurrentLimit(double value) {
    return this->setValue("SEQ_ILOWER", value);
}

std::string ThalesRemoteScriptWrapper::setSequenceCurrentRange(double value) {
    return this->setValue("SEQ_IRANGE", value);
}

std::string ThalesRemoteScriptWrapper::setSequencePotentialLatencyWindow(double value) {
    return this->setValue("SEQ_POTOFLO", value);
}

std::string ThalesRemoteScriptWrapper::setSequenceCurrentLatencyWindow(double value) {
    return this->setValue("SEQ_CUROFLO", value);
}

std::string ThalesRemoteScriptWrapper::enableFraMode(bool enabled) {
    return this->setValue("FRA", enabled);
}

std::string ThalesRemoteScriptWrapper::disableFraMode() {
    return this->enableFraMode(false);
}

std::string ThalesRemoteScriptWrapper::setFraVoltageInputGain(double value) {
    return this->setValue("FRA_POT_IN", value);
}

std::string ThalesRemoteScriptWrapper::setFraVoltageInputOffset(double value) {
    return this->setValue("FRA_POT_IN_OFF", value);
}

std::string ThalesRemoteScriptWrapper::setFraVoltageOutputGain(double value) {
    return this->setValue("FRA_POT_OUT", value);
}

std::string ThalesRemoteScriptWrapper::setFraVoltageOutputOffset(double value) {
    return this->setValue("FRA_POT_OUT_OFF", value);
}

std::string ThalesRemoteScriptWrapper::setFraVoltageMinimum(double value) {
    return this->setValue("FRA_POT_MIN", value);
}

std::string ThalesRemoteScriptWrapper::setFraVoltageMaximum(double value) {
    return this->setValue("FRA_POT_MAX", value);
}

std::string ThalesRemoteScriptWrapper::setFraCurrentInputGain(double value) {
    return this->setValue("FRA_CUR_IN", value);
}

std::string ThalesRemoteScriptWrapper::setFraCurrentInputOffset(double value) {
    return this->setValue("FRA_CUR_IN_OFF", value);
}

std::string ThalesRemoteScriptWrapper::setFraCurrentOutputGain(double value) {
    return this->setValue("FRA_CUR_OUT", value);
}

std::string ThalesRemoteScriptWrapper::setFraCurrentOutputOffset(double value) {
    return this->setValue("FRA_CUR_OUT_OFF", value);
}

std::string ThalesRemoteScriptWrapper::setFraCurrentMinimum(double value) {
    return this->setValue("FRA_CUR_MIN", value);
}

std::string ThalesRemoteScriptWrapper::setFraCurrentMaximum(double value) {
    return this->setValue("FRA_CUR_MAX", value);
}

std::string ThalesRemoteScriptWrapper::setFraPotentiostatMode(PotentiostatMode potentiostatMode) {
    std::string command;

    switch (potentiostatMode) {
        case PotentiostatMode::POTENTIOSTATIC:
            command = "FRAGAL=0";
            break;
        case PotentiostatMode::GALVANOSTATIC:
            command = "FRAGAL=1";
            break;
        default:
            return "error";
            break;
    }

    return this->executeRemoteCommand(command);
}

std::string ThalesRemoteScriptWrapper::readFraSetup() {
    return this->executeRemoteCommand("SENDFRASETUP");
}

std::string ThalesRemoteScriptWrapper::readAcqSetup() {
    return this->executeRemoteCommand("SENDACQSETUP");
}

std::string ThalesRemoteScriptWrapper::readAllAcqChannels() {
    std::string reply = this->executeRemoteCommand("ANALOGALL");

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    return reply;
}

double ThalesRemoteScriptWrapper::readAcqChannel(int channel) {
    this->setValue("CHANNEL", channel);
    return this->requestValueAndParseUsingRegexp("ANALOGIN", std::regex("=[\\s]*(.*)"));
}


std::string ThalesRemoteScriptWrapper::enableAcq(bool enabled) {
    if (enabled == true) {
        return this->executeRemoteCommand("ACQENA=1");
    } else {
        return this->executeRemoteCommand("ACQENA=0");
    }
}

std::string ThalesRemoteScriptWrapper::disableAcq() {
    return this->enableAcq(false);
}

/*
 * protected/internal Methods
 */


std::string ThalesRemoteScriptWrapper::setValue(std::string name, PotentialRelation relation) {
    int relationInt;

    if (relation == PotentialRelation::RELATIVE_RELATED) {
        relationInt = -1;
    } else {
        relationInt = 0;
    }

    return this->setValue(name, relationInt);
}

std::string ThalesRemoteScriptWrapper::setValue(std::string name, bool value) {
    int enable        = (value == true) ? 1 : 0;
    std::string reply = this->executeRemoteCommand(name + "=" + std::to_string(enable));

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    return reply;
}

std::string ThalesRemoteScriptWrapper::setValue(std::string name, double value) {
    std::string reply = this->executeRemoteCommand(name + "=" + to_string_with_precision(value, 10));

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    return reply;
}

std::string ThalesRemoteScriptWrapper::setValue(std::string name, int value) {
    std::string reply = this->executeRemoteCommand(name + "=" + std::to_string(value));

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    return reply;
}

std::string ThalesRemoteScriptWrapper::setValue(std::string name, std::string value) {
    std::string reply = this->executeRemoteCommand(name + "=" + value);

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    return reply;
}

double ThalesRemoteScriptWrapper::requestValueAndParseUsingRegexp(std::string command, std::regex pattern) {
    double result = std::nan("1");

    std::string reply = this->executeRemoteCommand(command);

    if (reply.find("ERROR") != std::string::npos) {
        throw ThalesRemoteError(reply);
    }

    std::regex replyStringPattern(pattern);
    std::smatch match;

    std::regex_search(reply, match, replyStringPattern);

    if (match.size() > 1) {
        result = this->stringToDobule(match.str(1));
    }

    return result;
}

double ThalesRemoteScriptWrapper::stringToDobule(std::string string) {
    std::stringstream stream(string);
    double number;

    stream >> number;

    return number;
}

int ThalesRemoteScriptWrapper::stringToInt(std::string string) {
    std::stringstream stream(string);
    int number;

    stream >> number;

    return number;
}
