// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/bluetooth/bluez/metrics_recorder.h"

#include "base/metrics/histogram_functions.h"

namespace bluetooth {
namespace {

// Note: These values must stay in sync with BlueZ's error.c file.
const char kBlueZInvalidArgumentsError[] = "org.bluez.Error.InvalidArguments";
const char kBlueZInProgressError[] = "org.bluez.Error.InProgress";
const char kBlueZAlreadyExistsError[] = "org.bluez.Error.AlreadyExists";
const char kBlueZNotSupportedError[] = "org.bluez.Error.NotSupported";
const char kBlueZNotConnectedError[] = "org.bluez.Error.NotConnected";
const char kBlueZAlreadyConnectedError[] = "org.bluez.Error.AlreadyConnected";
const char kBlueZNotAvailableError[] = "org.bluez.Error.NotAvailable";
const char kBlueZDoesNotExistError[] = "org.bluez.Error.DoesNotExist";
const char kBlueZNotAuthorizedError[] = "org.bluez.Error.NotAuthorized";
const char kBlueZNotPermittedError[] = "org.bluez.Error.NotPermitted";
const char kBlueZNoSuchAdapterError[] = "org.bluez.Error.NoSuchAdapter";
const char kBlueZAgentNotAvailableError[] = "org.bluez.Error.AgentNotAvailable";
const char kBlueZNotReadyError[] = "org.bluez.Error.NotReady";
const char kBlueZFailedError[] = "org.bluez.Error.Failed";

// Note: These values are sourced from the "BR/EDR connection failure reasons"
// in BlueZ's error.h file, and should be kept in sync.
const char kBlueZConnectionAlreadyConnected[] =
    "br-connection-already-connected";
const char kBlueZPageTimeout[] = "br-connection-page-timeout";
const char kBlueZProfileUnavailable[] = "br-connection-profile-unavailable";
const char kBlueZSdpSearch[] = "br-connection-sdp-search";
const char kBlueZCreateSocket[] = "br-connection-create-socket";
const char kBlueZInvalidArgument[] = "br-connection-invalid-argument";
const char kBlueZAdapterNotPowered[] = "br-connection-adapter-not-powered";
const char kBlueZNotSupported[] = "br-connection-not-supported";
const char kBlueZBadSocket[] = "br-connection-bad-socket";
const char kBlueZMemoryAllocation[] = "br-connection-memory-allocation";
const char kBlueZBusy[] = "br-connection-busy";
const char kBlueZConcurrentConnectionLimit[] =
    "br-connection-concurrent-connection-limit";
const char kBlueZTimeout[] = "br-connection-timeout";
const char kBlueZRefused[] = "br-connection-refused";
const char kBlueZAbortedByRemote[] = "br-connection-aborted-by-remote";
const char kBlueZAbortedByLocal[] = "br-connection-aborted-by-local";
const char kBlueZLmpProtocolError[] = "br-connection-lmp-protocol-error";
const char kBlueZCanceled[] = "br-connection-canceled";
const char kBlueZUnknown[] = "br-connection-unknown";

}  // namespace

std::optional<ConnectToServiceInsecurelyResult> ExtractResultFromErrorString(
    const std::string& error_string) {
#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZInvalidArgumentsError)) {
#else
  if (error_string.find(kBlueZInvalidArgumentsError) != std::string::npos) {
#endif
    return ConnectToServiceInsecurelyResult::kInvalidArgumentsError;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZInProgressError)) {
#else
  if (error_string.find(kBlueZInProgressError) != std::string::npos) {
#endif
    return ConnectToServiceInsecurelyResult::kInProgressError;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZAlreadyExistsError)) {
#else
  if (error_string.find(kBlueZAlreadyExistsError) != std::string::npos) {
#endif
    return ConnectToServiceInsecurelyResult::kAlreadyExistsError;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZNotSupportedError)) {
#else
  if (error_string.find(kBlueZNotSupportedError) != std::string::npos) {
#endif
    return ConnectToServiceInsecurelyResult::kNotSupportedError;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZNotConnectedError)) {
#else
  if (error_string.find(kBlueZNotConnectedError) != std::string::npos) {
#endif
    return ConnectToServiceInsecurelyResult::kNotConnectedError;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZAlreadyConnectedError)) {
#else
  if (error_string.find(kBlueZAlreadyConnectedError) != std::string::npos) {
#endif
    return ConnectToServiceInsecurelyResult::kAlreadyConnectedError;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZNotAvailableError)) {
#else
  if (error_string.find(kBlueZNotAvailableError) != std::string::npos) {
#endif
    return ConnectToServiceInsecurelyResult::kNotAvailableError;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZDoesNotExistError)) {
#else
  if (error_string.find(kBlueZDoesNotExistError) != std::string::npos) {
#endif
    return ConnectToServiceInsecurelyResult::kDoesNotExistError;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZNotAuthorizedError)) {
#else
  if (error_string.find(kBlueZNotAuthorizedError) != std::string::npos) {
#endif
    return ConnectToServiceInsecurelyResult::kNotAuthorizedError;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZNotPermittedError)) {
#else
  if (error_string.find(kBlueZNotPermittedError) != std::string::npos) {
#endif
    return ConnectToServiceInsecurelyResult::kNotPermittedError;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZNoSuchAdapterError)) {
#else
  if (error_string.find(kBlueZNoSuchAdapterError) != std::string::npos) {
#endif
    return ConnectToServiceInsecurelyResult::kNoSuchAdapterError;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZAgentNotAvailableError)) {
#else
  if (error_string.find(kBlueZAgentNotAvailableError) != std::string::npos) {
#endif
    return ConnectToServiceInsecurelyResult::kAgentNotAvailableError;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZNotReadyError)) {
#else
  if (error_string.find(kBlueZNotReadyError) != std::string::npos) {
#endif
    return ConnectToServiceInsecurelyResult::kNotReadyError;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZFailedError)) {
#else
  if (error_string.find(kBlueZFailedError) != std::string::npos) {
#endif
    return ConnectToServiceInsecurelyResult::kFailedError;
  }

  return std::nullopt;
}

ConnectToServiceFailureReason ExtractFailureReasonFromErrorString(
    const std::string& error_string) {
#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZConnectionAlreadyConnected)) {
#else
  if (error_string.find(kBlueZConnectionAlreadyConnected) != std::string::npos) {
#endif
    return ConnectToServiceFailureReason::kReasonConnectionAlreadyConnected;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZPageTimeout)) {
#else
  if (error_string.find(kBlueZPageTimeout) != std::string::npos) {
#endif
    return ConnectToServiceFailureReason::kReasonPageTimeout;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZProfileUnavailable)) {
#else
  if (error_string.find(kBlueZProfileUnavailable) != std::string::npos) {
#endif
    return ConnectToServiceFailureReason::kReasonProfileUnavailable;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZSdpSearch)) {
#else
  if (error_string.find(kBlueZSdpSearch) != std::string::npos) {
#endif
    return ConnectToServiceFailureReason::kReasonSdpSearch;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZCreateSocket)) {
#else
  if (error_string.find(kBlueZCreateSocket) != std::string::npos) {
#endif
    return ConnectToServiceFailureReason::kReasonCreateSocket;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZInvalidArgument)) {
#else
  if (error_string.find(kBlueZInvalidArgument) != std::string::npos) {
#endif
    return ConnectToServiceFailureReason::kReasonInvalidArgument;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZAdapterNotPowered)) {
#else
  if (error_string.find(kBlueZAdapterNotPowered) != std::string::npos) {
#endif
    return ConnectToServiceFailureReason::kReasonAdapterNotPowered;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZNotSupported)) {
#else
  if (error_string.find(kBlueZNotSupported) != std::string::npos) {
#endif
    return ConnectToServiceFailureReason::kReasonNotSupported;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZBadSocket)) {
#else
  if (error_string.find(kBlueZBadSocket) != std::string::npos) {
#endif
    return ConnectToServiceFailureReason::kReasonBadSocket;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZMemoryAllocation)) {
#else
  if (error_string.find(kBlueZMemoryAllocation) != std::string::npos) {
#endif
    return ConnectToServiceFailureReason::kReasonMemoryAllocation;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZBusy)) {
#else
  if (error_string.find(kBlueZBusy) != std::string::npos) {
#endif
    return ConnectToServiceFailureReason::kReasonBusy;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZConcurrentConnectionLimit)) {
#else
  if (error_string.find(kBlueZConcurrentConnectionLimit) != std::string::npos) {
#endif
    return ConnectToServiceFailureReason::kReasonConcurrentConnectionLimit;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZTimeout)) {
#else
  if (error_string.find(kBlueZTimeout) != std::string::npos) {
#endif
    return ConnectToServiceFailureReason::kReasonTimeout;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZRefused)) {
#else
  if (error_string.find(kBlueZRefused) != std::string::npos) {
#endif
    return ConnectToServiceFailureReason::kReasonRefused;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZAbortedByRemote)) {
#else
  if (error_string.find(kBlueZAbortedByRemote) != std::string::npos) {
#endif
    return ConnectToServiceFailureReason::kReasonAbortedByRemote;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZAbortedByLocal)) {
#else
  if (error_string.find(kBlueZAbortedByLocal) != std::string::npos) {
#endif
    return ConnectToServiceFailureReason::kReasonAbortedByLocal;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZLmpProtocolError)) {
#else
  if (error_string.find(kBlueZLmpProtocolError) != std::string::npos) {
#endif
    return ConnectToServiceFailureReason::kReasonLmpProtocolError;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZCanceled)) {
#else
  if (error_string.find(kBlueZCanceled) != std::string::npos) {
#endif
    return ConnectToServiceFailureReason::kReasonCanceled;
  }

#if __cpp_lib_string_contains
  if (error_string.contains(kBlueZUnknown)) {
#else
  if (error_string.find(kBlueZUnknown) != std::string::npos) {
#endif
    return ConnectToServiceFailureReason::kReasonUnknown;
  }

  return ConnectToServiceFailureReason::kReasonUnknown;
}

void RecordConnectToServiceInsecurelyResult(
    ConnectToServiceInsecurelyResult result) {
  base::UmaHistogramEnumeration(
      "Bluetooth.Linux.ConnectToServiceInsecurelyResult", result);
}

void RecordConnectToServiceFailureReason(ConnectToServiceFailureReason reason) {
  base::UmaHistogramEnumeration(
      "Bluetooth.Linux.ConnectToService.FailureReason", reason);
}

void RecordBondedConnectToServiceFailureReason(
    ConnectToServiceFailureReason reason) {
  base::UmaHistogramEnumeration(
      "Bluetooth.Linux.ConnectToService.Bonded.FailureReason", reason);
}

}  // namespace bluetooth
