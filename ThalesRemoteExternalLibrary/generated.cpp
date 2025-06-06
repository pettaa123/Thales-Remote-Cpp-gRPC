
/******************************************************************
 *  ____       __                        __    __   __      _ __
 * /_  / ___ _/ /  ___  ___ ___________ / /__ / /__/ /_____(_) /__
 *  / /_/ _ `/ _ \/ _ \/ -_) __/___/ -_) / -_)  '_/ __/ __/ /  '_/
 * /___/\_,_/_//_/_//_/\__/_/      \__/_/\__/_/\_\\__/_/ /_/_/\_\
 *
 * Copyright 2023 ZAHNER-elektrik I. Zahner-Schiller GmbH & Co. KG
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

#include "generated.h"
#include <map>
#include <string>
#include "thalesremoteerror.h"

std::mutex zenniumConnectionsMutex;
std::mutex scriptWrappersMutex;
std::mutex fileInterfacesMutex;
std::mutex errorMessagesMutex;

std::map<ZenniumConnection*, std::shared_ptr<ZenniumConnection>> zenniumConnections;
std::map<ThalesRemoteScriptWrapper*, std::shared_ptr<ThalesRemoteScriptWrapper>> scriptWrappers;
std::map<ThalesFileInterface*, std::shared_ptr<ThalesFileInterface>> fileInterfaces;
std::map<void*, std::string> errorMessages;

std::map<ZenniumConnection*, std::mutex> zenniumMutexes;
std::map<ThalesRemoteScriptWrapper*, std::mutex> scriptWrapperMutexes;
std::map<ThalesFileInterface*, std::mutex> fileInterfaceMutexes;
std::map<void*, std::mutex> errorMutexes;

bool setNoErrorErrorMessage(void* handle) {
    std::lock_guard<std::mutex> objectLock(errorMutexes.at(handle));
    errorMessages.at(handle) = "no error";
    return false;
}

bool setErrorMessage(void* handle, const std::string& message) {
    std::lock_guard<std::mutex> objectLock(errorMutexes.at(handle));
    errorMessages.at(handle) = message;
    return false;
}

extern "C" {
__declspec(dllexport) bool __stdcall getErrorMessage(void* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(errorMutexes.at(handle));
        *retvalLen =
            errorMessages.at(handle).copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;
        return true;
    } catch (...) {
        const char no_error[] = "no error";
        int max_length        = *retvalLen - 1;  // Reserve space for null terminator

        // Copy at most max_length characters
        strncpy(retval, no_error, max_length);

        // Ensure null termination (important with strncpy!)
        retval[max_length] = '\0';

        // Set the actual length copied (including null terminator)
        *retvalLen = std::min(static_cast<int>(strlen(no_error)), max_length) + 1;
        return false;
    }
}

__declspec(dllexport) ZenniumConnection* __stdcall createZenniumConnection() {
    auto connection = std::make_shared<ZenniumConnection>();
    auto rawPtr     = connection.get();

    {
        std::lock_guard<std::mutex> connectionsLock(zenniumConnectionsMutex);
        zenniumConnections.insert({rawPtr, connection});
        zenniumMutexes.try_emplace(rawPtr);
    }

    {
        std::lock_guard<std::mutex> errorsLock(errorMessagesMutex);
        errorMessages.insert({rawPtr, ""});
        errorMutexes.try_emplace(rawPtr);
    }

    return rawPtr;
}
__declspec(dllexport) void __stdcall deleteZenniumConnection(ZenniumConnection* handle) {
    std::lock_guard<std::mutex> connectionsLock(zenniumConnectionsMutex);
    std::lock_guard<std::mutex> errorsLock(errorMessagesMutex);

    {
        std::lock_guard<std::mutex> objectLock(zenniumMutexes.at(handle));
        zenniumConnections.erase(handle);
    }
    zenniumMutexes.erase(handle);

    {
        std::lock_guard<std::mutex> objectLock(errorMutexes.at(handle));
        errorMessages.erase(handle);
    }
    errorMutexes.erase(handle);
}

__declspec(dllexport) bool __stdcall disconnectFromTerm(ZenniumConnection* handle) {
    try {
        std::lock_guard<std::mutex> objectLock(zenniumMutexes.at(handle));
        zenniumConnections.at(handle)->disconnectFromTerm();

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall connectToTerm(ZenniumConnection* handle, char const* address, char const* connectionName) {
    try {
        std::lock_guard<std::mutex> objectLock(zenniumMutexes.at(handle));
        setNoErrorErrorMessage(handle);
        return zenniumConnections.at(handle)->connectToTerm(std::string(address), std::string(connectionName));
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool waitForBinaryTelegram(
    ZenniumConnection* handle, int message_type, char* retval, int* retvalLen
) {
    try {
        std::lock_guard<std::mutex> objectLock(zenniumMutexes.at(handle));
        auto returned = zenniumConnections.at(handle)->waitForBinaryTelegram(message_type);

        if (retval != nullptr && *retvalLen > 0) {
            size_t copySize = std::min(static_cast<size_t>(*retvalLen), returned.size());
            std::copy(returned.begin(), returned.begin() + copySize, retval);
            *retvalLen = copySize;
        }

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool waitForBinaryTelegramTimeout(
    ZenniumConnection* handle, int message_type, int timeout, char* retval, int* retvalLen
) {
    try {
        std::lock_guard<std::mutex> objectLock(zenniumMutexes.at(handle));
        auto returned = zenniumConnections.at(handle)->waitForBinaryTelegram(
            message_type, std::chrono::duration<int, std::milli>(timeout)
        );

        if (retval != nullptr && *retvalLen > 0) {
            size_t copySize = std::min(static_cast<size_t>(*retvalLen), returned.size());
            std::copy(returned.begin(), returned.begin() + copySize, retval);
            *retvalLen = copySize;
        }

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setTimeout(ZenniumConnection* handle, int timeout) {
    std::lock_guard<std::mutex> objectLock(zenniumMutexes.at(handle));
    zenniumConnections.at(handle)->setTimeout(std::chrono::duration<int, std::milli>(timeout));
    return true;
}

__declspec(dllexport) bool __stdcall getTimeout(ZenniumConnection* handle, int* timeout) {
    std::lock_guard<std::mutex> objectLock(zenniumMutexes.at(handle));
    auto retval = zenniumConnections.at(handle)->getTimeout();
    *timeout    = static_cast<int>(std::chrono::milliseconds(retval).count());
    return true;
}

__declspec(dllexport) ThalesRemoteScriptWrapper* __stdcall createThalesRemoteScriptWrapper(ZenniumConnection* handle) {
    std::lock_guard<std::mutex> objectLock(zenniumMutexes.at(handle));
    auto wrapper = std::make_shared<ThalesRemoteScriptWrapper>(handle);
    auto rawPtr  = wrapper.get();

    {
        std::lock_guard<std::mutex> wrappersLock(scriptWrappersMutex);
        scriptWrappers.insert({rawPtr, wrapper});
        scriptWrapperMutexes.try_emplace(rawPtr);
    }

    {
        std::lock_guard<std::mutex> errorsLock(errorMessagesMutex);
        errorMessages.insert({rawPtr, "no error"});
        errorMutexes.try_emplace(rawPtr);
    }

    return rawPtr;
}

__declspec(dllexport) void __stdcall deleteThalesRemoteScriptWrapper(ThalesRemoteScriptWrapper* handle) {
    std::lock_guard<std::mutex> wrappersLock(scriptWrappersMutex);
    std::lock_guard<std::mutex> errorsLock(errorMessagesMutex);

    {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        scriptWrappers.erase(handle);
    }
    scriptWrapperMutexes.erase(handle);

    {
        std::lock_guard<std::mutex> objectLock(errorMutexes.at(handle));
        errorMessages.erase(handle);
    }
    errorMutexes.erase(handle);
}

__declspec(dllexport) ThalesFileInterface* __stdcall createThalesFileInterface(ZenniumConnection* handle) {
    std::lock_guard<std::mutex> objectLock(zenniumMutexes.at(handle));
    auto wrapper = std::make_shared<ThalesFileInterface>(handle);
    auto rawPtr  = wrapper.get();

    {
        std::lock_guard<std::mutex> wrappersLock(fileInterfacesMutex);
        fileInterfaces.insert({rawPtr, wrapper});
        fileInterfaceMutexes.try_emplace(rawPtr);  // Konstruiert den Mutex in-place
    }

    {
        std::lock_guard<std::mutex> errorsLock(errorMessagesMutex);
        errorMessages.insert({rawPtr, ""});
        errorMutexes.try_emplace(rawPtr);  // Konstruiert den Mutex in-place
    }

    return rawPtr;
}

__declspec(dllexport) void __stdcall deleteThalesFileInterface(ThalesFileInterface* handle) {
    std::lock_guard<std::mutex> wrappersLock(fileInterfacesMutex);
    std::lock_guard<std::mutex> errorsLock(errorMessagesMutex);

    {
        std::lock_guard<std::mutex> objectLock(fileInterfaceMutexes.at(handle));
        fileInterfaces.erase(handle);
    }
    fileInterfaceMutexes.erase(handle);

    {
        std::lock_guard<std::mutex> objectLock(errorMutexes.at(handle));
        errorMessages.erase(handle);
    }
    errorMutexes.erase(handle);
}

__declspec(dllexport) bool __stdcall enableAutomaticFileExchange(
    ThalesFileInterface* handle, char* retval, int* retvalLen, bool enable, char const* fileExtensions
) {
    try {
        std::lock_guard<std::mutex> objectLock(fileInterfaceMutexes.at(handle));
        auto returned = fileInterfaces.at(handle)->enableAutomaticFileExchange(enable, std::string(fileExtensions));

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall disableAutomaticFileExchange(ThalesFileInterface* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(fileInterfaceMutexes.at(handle));
        auto returned = fileInterfaces.at(handle)->disableAutomaticFileExchange();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall appendFilesToSkip(ThalesFileInterface* handle, char const* filename) {
    try {
        std::lock_guard<std::mutex> objectLock(fileInterfaceMutexes.at(handle));
        fileInterfaces.at(handle)->appendFilesToSkip(std::string(filename));

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setSavePath(ThalesFileInterface* handle, char const* path) {
    try {
        std::lock_guard<std::mutex> objectLock(fileInterfaceMutexes.at(handle));
        fileInterfaces.at(handle)->setSavePath(std::string(path));

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall enableSaveReceivedFilesToDisk(ThalesFileInterface* handle, char const* path, bool enable) {
    try {
        std::lock_guard<std::mutex> objectLock(fileInterfaceMutexes.at(handle));
        fileInterfaces.at(handle)->enableSaveReceivedFilesToDisk(std::string(path), enable);

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall disableSaveReceivedFilesToDisk(ThalesFileInterface* handle) {
    try {
        std::lock_guard<std::mutex> objectLock(fileInterfaceMutexes.at(handle));
        fileInterfaces.at(handle)->disableSaveReceivedFilesToDisk();

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}


PotentiostatMode stringToPotentiostatMode(std::string string) {
    if (string == "") {
        throw ThalesRemoteError("enum member does not exist.");
    }

    else if (string == "POTENTIOSTATIC") {
        return PotentiostatMode::POTENTIOSTATIC;
    }

    else if (string == "GALVANOSTATIC") {
        return PotentiostatMode::GALVANOSTATIC;
    }

    else if (string == "PSEUDOGALVANOSTATIC") {
        return PotentiostatMode::PSEUDOGALVANOSTATIC;
    }

    else {
        throw ThalesRemoteError("enum member does not exist.");
    }
}

ScanStrategy stringToScanStrategy(std::string string) {
    if (string == "") {
        throw ThalesRemoteError("enum member does not exist.");
    }

    else if (string == "SINGLE_SINE") {
        return ScanStrategy::SINGLE_SINE;
    }

    else if (string == "MULTI_SINE") {
        return ScanStrategy::MULTI_SINE;
    }

    else if (string == "TABLE") {
        return ScanStrategy::TABLE;
    }

    else {
        throw ThalesRemoteError("enum member does not exist.");
    }
}

ScanDirection stringToScanDirection(std::string string) {
    if (string == "") {
        throw ThalesRemoteError("enum member does not exist.");
    }

    else if (string == "START_TO_MIN") {
        return ScanDirection::START_TO_MIN;
    }

    else if (string == "START_TO_MAX") {
        return ScanDirection::START_TO_MAX;
    }

    else {
        throw ThalesRemoteError("enum member does not exist.");
    }
}

NamingRule stringToNamingRule(std::string string) {
    if (string == "") {
        throw ThalesRemoteError("enum member does not exist.");
    }

    else if (string == "DATETIME") {
        return NamingRule::DATETIME;
    }

    else if (string == "COUNTER") {
        return NamingRule::COUNTER;
    }

    else if (string == "INDIVIDUAL") {
        return NamingRule::INDIVIDUAL;
    }

    else {
        throw ThalesRemoteError("enum member does not exist.");
    }
}

PotentialRelation stringToPotentialRelation(std::string string) {
    if (string == "") {
        throw ThalesRemoteError("enum member does not exist.");
    }

    else if (string == "ABSOLUTE_RELATED") {
        return PotentialRelation::ABSOLUTE_RELATED;
    }

    else if (string == "RELATIVE_RELATED") {
        return PotentialRelation::RELATIVE_RELATED;
    }

    else {
        throw ThalesRemoteError("enum member does not exist.");
    }
}

IESweepMode stringToIESweepMode(std::string string) {
    if (string == "") {
        throw ThalesRemoteError("enum member does not exist.");
    }

    else if (string == "STEADYSTATE") {
        return IESweepMode::STEADYSTATE;
    }

    else if (string == "FIXEDSAMPLING") {
        return IESweepMode::FIXEDSAMPLING;
    }

    else if (string == "DYNAMICSCAN") {
        return IESweepMode::DYNAMICSCAN;
    }

    else {
        throw ThalesRemoteError("enum member does not exist.");
    }
}

Pad4Mode stringToPad4Mode(std::string string) {
    if (string == "") {
        throw ThalesRemoteError("enum member does not exist.");
    }

    else if (string == "VOLTAGE") {
        return Pad4Mode::VOLTAGE;
    }

    else if (string == "CURRENT") {
        return Pad4Mode::CURRENT;
    }

    else {
        throw ThalesRemoteError("enum member does not exist.");
    }
}

__declspec(dllexport
) bool __stdcall forceThalesIntoRemoteScript(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->forceThalesIntoRemoteScript();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall hideWindow(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->hideWindow();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall showWindow(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->showWindow();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall getThalesVersion(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->getThalesVersion();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall getWorkstationHeartBeat(ThalesRemoteScriptWrapper* handle, int* retval) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->getWorkstationHeartBeat();
        *retval       = returned;
        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall getCurrent(ThalesRemoteScriptWrapper* handle, double* retval) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->getCurrent();
        *retval       = returned;
        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall getPotential(ThalesRemoteScriptWrapper* handle, double* retval) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->getPotential();
        *retval       = returned;
        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall getVoltage(ThalesRemoteScriptWrapper* handle, double* retval) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->getVoltage();
        *retval       = returned;
        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setCurrent(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double current) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setCurrent(current);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setPotential(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double potential) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setPotential(potential);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setMaximumShuntIndex(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, int shunt) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setMaximumShuntIndex(shunt);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setMinimumShuntIndex(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, int shunt) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setMinimumShuntIndex(shunt);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setShuntIndex(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, int index) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setShuntIndex(index);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setVoltageRangeIndex(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, int index) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setVoltageRangeIndex(index);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall selectPotentiostat(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, int device) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->selectPotentiostat(device);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall selectPotentiostatWithoutPotentiostatStateChange(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, int device
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->selectPotentiostatWithoutPotentiostatStateChange(device);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall switchToSCPIControl(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->switchToSCPIControl();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall switchToSCPIControlWithoutPotentiostatStateChange(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->switchToSCPIControlWithoutPotentiostatStateChange();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall getSerialNumber(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->getSerialNumber();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall getDeviceName(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->getDeviceName();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall readSetup(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->readSetup();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall calibrateOffsets(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->calibrateOffsets();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall enablePotentiostat(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, bool enabled) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->enablePotentiostat(enabled);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall disablePotentiostat(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->disablePotentiostat();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setPotentiostatMode(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, char const* potentiostatMode
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned =
            scriptWrappers.at(handle)->setPotentiostatMode(stringToPotentiostatMode(std::string(potentiostatMode)));

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall enableRuleFileUsage(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, bool enabled) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->enableRuleFileUsage(enabled);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall disableRuleFileUsage(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->disableRuleFileUsage();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setupPad4Channel(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, int card, int channel, bool enabled
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setupPad4Channel(card, channel, enabled);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setupPad4ChannelWithVoltageRange(
    ThalesRemoteScriptWrapper* handle,
    char* retval,
    int* retvalLen,
    int card,
    int channel,
    bool enabled,
    double voltageRange
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned =
            scriptWrappers.at(handle)->setupPad4ChannelWithVoltageRange(card, channel, enabled, voltageRange);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setupPad4ChannelWithShuntResistor(
    ThalesRemoteScriptWrapper* handle,
    char* retval,
    int* retvalLen,
    int card,
    int channel,
    bool enabled,
    double shuntResistor
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned =
            scriptWrappers.at(handle)->setupPad4ChannelWithShuntResistor(card, channel, enabled, shuntResistor);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setupPad4ModeGlobal(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, char const* mode
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setupPad4ModeGlobal(stringToPad4Mode(std::string(mode)));

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall enablePad4Global(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, bool enabled) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->enablePad4Global(enabled);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall disablePad4Global(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->disablePad4Global();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall readPAD4Setup(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->readPAD4Setup();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setFrequency(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double frequency) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setFrequency(frequency);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setAmplitude(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double amplitude) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setAmplitude(amplitude);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setNumberOfPeriods(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, int numberOfPeriods
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setNumberOfPeriods(numberOfPeriods);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setUpperFrequencyLimit(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double frequency
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setUpperFrequencyLimit(frequency);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setLowerFrequencyLimit(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double frequency
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setLowerFrequencyLimit(frequency);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setStartFrequency(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double frequency) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setStartFrequency(frequency);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setUpperStepsPerDecade(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, int steps) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setUpperStepsPerDecade(steps);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setLowerStepsPerDecade(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, int steps) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setLowerStepsPerDecade(steps);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setUpperNumberOfPeriods(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, int periods) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setUpperNumberOfPeriods(periods);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setLowerNumberOfPeriods(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, int periods) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setLowerNumberOfPeriods(periods);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setScanStrategy(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, char const* strategy
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setScanStrategy(stringToScanStrategy(std::string(strategy)));

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setScanDirection(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, char const* direction
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setScanDirection(stringToScanDirection(std::string(direction)));

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall getImpedance(ThalesRemoteScriptWrapper* handle, double* real, double* imag) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->getImpedance();

        *real = returned.real();
        *imag = returned.imag();

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall getImpedancePad4(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->getImpedancePad4();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setEISNaming(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, char const* naming) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setEISNaming(stringToNamingRule(std::string(naming)));

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setEISCounter(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, int number) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setEISCounter(number);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setEISOutputPath(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, const char* path) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setEISOutputPath(std::string(path));

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setEISOutputFileName(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, const char* name
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setEISOutputFileName(std::string(name));

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall measureEIS(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->measureEIS();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setCVStartPotential(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double potential
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setCVStartPotential(potential);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setCVUpperReversingPotential(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double potential
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setCVUpperReversingPotential(potential);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setCVLowerReversingPotential(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double potential
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setCVLowerReversingPotential(potential);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setCVEndPotential(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double potential) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setCVEndPotential(potential);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setCVStartHoldTime(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double time) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setCVStartHoldTime(time);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setCVEndHoldTime(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double time) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setCVEndHoldTime(time);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setCVScanRate(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double scanRate) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setCVScanRate(scanRate);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setCVCycles(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double cycles) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setCVCycles(cycles);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setCVSamplesPerCycle(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double samples) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setCVSamplesPerCycle(samples);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setCVMaximumCurrent(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double current) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setCVMaximumCurrent(current);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setCVMinimumCurrent(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double current) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setCVMinimumCurrent(current);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setCVOhmicDrop(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double ohmicDrop) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setCVOhmicDrop(ohmicDrop);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall enableCVAutoRestartAtCurrentOverflow(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, bool enabled
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->enableCVAutoRestartAtCurrentOverflow(enabled);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall disableCVAutoRestartAtCurrentOverflow(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->disableCVAutoRestartAtCurrentOverflow();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall enableCVAutoRestartAtCurrentUnderflow(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, bool enabled
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->enableCVAutoRestartAtCurrentUnderflow(enabled);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall disableCVAutoRestartAtCurrentUnderflow(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->disableCVAutoRestartAtCurrentUnderflow();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall enableCVAnalogFunctionGenerator(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, bool enabled
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->enableCVAnalogFunctionGenerator(enabled);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall disableCVAnalogFunctionGenerator(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->disableCVAnalogFunctionGenerator();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setCVNaming(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, char const* naming) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setCVNaming(stringToNamingRule(std::string(naming)));

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setCVCounter(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, int number) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setCVCounter(number);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setCVOutputPath(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, const char* path) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setCVOutputPath(std::string(path));

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setCVOutputFileName(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, const char* name
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setCVOutputFileName(std::string(name));

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall checkCVSetup(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->checkCVSetup();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall readCVSetup(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->readCVSetup();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall measureCV(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->measureCV();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setIEFirstEdgePotential(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double potential
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setIEFirstEdgePotential(potential);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setIESecondEdgePotential(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double potential
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setIESecondEdgePotential(potential);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setIEThirdEdgePotential(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double potential
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setIEThirdEdgePotential(potential);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setIEFourthEdgePotential(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double potential
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setIEFourthEdgePotential(potential);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setIEFirstEdgePotentialRelation(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, char const* relation
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned =
            scriptWrappers.at(handle)->setIEFirstEdgePotentialRelation(stringToPotentialRelation(std::string(relation))
            );

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setIESecondEdgePotentialRelation(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, char const* relation
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned =
            scriptWrappers.at(handle)->setIESecondEdgePotentialRelation(stringToPotentialRelation(std::string(relation))
            );

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setIEThirdEdgePotentialRelation(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, char const* relation
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned =
            scriptWrappers.at(handle)->setIEThirdEdgePotentialRelation(stringToPotentialRelation(std::string(relation))
            );

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setIEFourthEdgePotentialRelation(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, char const* relation
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned =
            scriptWrappers.at(handle)->setIEFourthEdgePotentialRelation(stringToPotentialRelation(std::string(relation))
            );

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setIEPotentialResolution(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double resolution
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setIEPotentialResolution(resolution);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setIEMinimumWaitingTime(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double time) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setIEMinimumWaitingTime(time);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setIEMaximumWaitingTime(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double time) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setIEMaximumWaitingTime(time);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setIERelativeTolerance(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double tolerance
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setIERelativeTolerance(tolerance);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setIEAbsoluteTolerance(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double tolerance
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setIEAbsoluteTolerance(tolerance);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setIEOhmicDrop(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double ohmicDrop) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setIEOhmicDrop(ohmicDrop);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setIESweepMode(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, char const* sweepMode
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setIESweepMode(stringToIESweepMode(std::string(sweepMode)));

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setIEScanRate(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double scanRate) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setIEScanRate(scanRate);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setIEMaximumCurrent(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double current) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setIEMaximumCurrent(current);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setIEMinimumCurrent(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double current) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setIEMinimumCurrent(current);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setIENaming(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, char const* naming) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setIENaming(stringToNamingRule(std::string(naming)));

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setIECounter(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, int number) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setIECounter(number);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setIEOutputPath(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, const char* path) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setIEOutputPath(std::string(path));

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setIEOutputFileName(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, const char* name
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setIEOutputFileName(std::string(name));

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall checkIESetup(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->checkIESetup();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall readIESetup(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->readIESetup();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall measureIE(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->measureIE();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall selectSequence(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, int number) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->selectSequence(number);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setSequenceNaming(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, char const* naming
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setSequenceNaming(stringToNamingRule(std::string(naming)));

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setSequenceCounter(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, int number) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setSequenceCounter(number);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setSequenceOutputPath(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, const char* path
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setSequenceOutputPath(std::string(path));

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setSequenceOutputFileName(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, const char* name
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setSequenceOutputFileName(std::string(name));

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall enableSequenceAcqGlobal(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, bool state) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->enableSequenceAcqGlobal(state);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall disableSequenceAcqGlobal(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->disableSequenceAcqGlobal();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall enableSequenceAcqChannel(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, int channel, bool state
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->enableSequenceAcqChannel(channel, state);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall disableSequenceAcqChannel(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, int channel
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->disableSequenceAcqChannel(channel);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall readSequenceAcqSetup(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->readSequenceAcqSetup();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall runSequence(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->runSequence();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setSequenceOhmicDrop(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double value) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setSequenceOhmicDrop(value);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setSequenceMaximumRuntime(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double value
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setSequenceMaximumRuntime(value);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setSequenceUpperPotentialLimit(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double value
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setSequenceUpperPotentialLimit(value);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setSequenceLowerPotentialLimit(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double value
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setSequenceLowerPotentialLimit(value);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setSequenceUpperCurrentLimit(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double value
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setSequenceUpperCurrentLimit(value);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setSequenceLowerCurrentLimit(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double value
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setSequenceLowerCurrentLimit(value);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setSequenceCurrentRange(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double value
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setSequenceCurrentRange(value);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setSequencePotentialLatencyWindow(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double value
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setSequencePotentialLatencyWindow(value);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setSequenceCurrentLatencyWindow(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double value
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setSequenceCurrentLatencyWindow(value);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall enableFraMode(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, bool enabled) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->enableFraMode(enabled);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setFraVoltageInputGain(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double value) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setFraVoltageInputGain(value);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setFraVoltageInputOffset(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double value
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setFraVoltageInputOffset(value);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setFraVoltageOutputGain(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double value
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setFraVoltageOutputGain(value);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setFraVoltageOutputOffset(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double value
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setFraVoltageOutputOffset(value);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setFraVoltageMinimum(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double value) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setFraVoltageMinimum(value);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setFraVoltageMaximum(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double value) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setFraVoltageMaximum(value);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setFraCurrentInputGain(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double value) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setFraCurrentInputGain(value);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setFraCurrentInputOffset(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double value
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setFraCurrentInputOffset(value);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setFraCurrentOutputGain(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double value
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setFraCurrentOutputGain(value);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setFraCurrentOutputOffset(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double value
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setFraCurrentOutputOffset(value);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setFraCurrentMinimum(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double value) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setFraCurrentMinimum(value);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall setFraCurrentMaximum(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, double value) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->setFraCurrentMaximum(value);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall setFraPotentiostatMode(
    ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, char const* potentiostatMode
) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned =
            scriptWrappers.at(handle)->setFraPotentiostatMode(stringToPotentiostatMode(std::string(potentiostatMode)));

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall readFraSetup(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->readFraSetup();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall readAcqSetup(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->readAcqSetup();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall readAllAcqChannels(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->readAllAcqChannels();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall readAcqChannel(ThalesRemoteScriptWrapper* handle, double* retval, int channel) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->readAcqChannel(channel);
        *retval       = returned;
        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport
) bool __stdcall enableAcq(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen, bool enabled) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->enableAcq(enabled);

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}

__declspec(dllexport) bool __stdcall disableAcq(ThalesRemoteScriptWrapper* handle, char* retval, int* retvalLen) {
    try {
        std::lock_guard<std::mutex> objectLock(scriptWrapperMutexes.at(handle));
        auto returned = scriptWrappers.at(handle)->disableAcq();

        *retvalLen         = returned.copy(retval, static_cast<std::basic_string<char>::size_type>(*retvalLen - 1), 0);
        retval[*retvalLen] = '\0';
        *retvalLen += 1;

        setNoErrorErrorMessage(handle);
        return true;
    } catch (const ZahnerError& ex) {
        setErrorMessage(handle, ex.getMessage());
        return false;
    } catch (...) {
        setErrorMessage(handle, "undefined error");
        return false;
    }
}
}
