/** @file
  IPMI Command - NetFnApp.

  Copyright (c) 2018 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IpmiBaseLib.h>

#include <IndustryStandard/Ipmi.h>
#include <IpmiFeature.h>

EFI_STATUS
EFIAPI
IpmiGetDeviceId (
  OUT IPMI_GET_DEVICE_ID_RESPONSE  *DeviceId
  )
{
  EFI_STATUS  Status;
  UINT32      DataSize;

  DataSize = sizeof (*DeviceId);
  Status   = IpmiSubmitCommand (
               IPMI_NETFN_APP,
               IPMI_APP_GET_DEVICE_ID,
               NULL,
               0,
               (VOID *)DeviceId,
               &DataSize
               );
  return Status;
}

EFI_STATUS
EFIAPI
IpmiGetSelfTestResult (
  OUT IPMI_SELF_TEST_RESULT_RESPONSE  *SelfTestResult
  )
{
  EFI_STATUS  Status;
  UINT32      DataSize;

  DataSize = sizeof (*SelfTestResult);
  Status   = IpmiSubmitCommand (
               IPMI_NETFN_APP,
               IPMI_APP_GET_SELFTEST_RESULTS,
               NULL,
               0,
               (VOID *)SelfTestResult,
               &DataSize
               );
  return Status;
}

EFI_STATUS
EFIAPI
IpmiResetWatchdogTimer (
  OUT UINT8  *CompletionCode
  )
{
  EFI_STATUS  Status;
  UINT32      DataSize;

  DataSize = sizeof (*CompletionCode);
  Status   = IpmiSubmitCommand (
               IPMI_NETFN_APP,
               IPMI_APP_RESET_WATCHDOG_TIMER,
               NULL,
               0,
               (VOID *)CompletionCode,
               &DataSize
               );
  return Status;
}

EFI_STATUS
EFIAPI
IpmiSetWatchdogTimer (
  IN  IPMI_SET_WATCHDOG_TIMER_REQUEST  *SetWatchdogTimer,
  OUT UINT8                            *CompletionCode
  )
{
  EFI_STATUS  Status;
  UINT32      DataSize;

  DataSize = sizeof (*CompletionCode);
  Status   = IpmiSubmitCommand (
               IPMI_NETFN_APP,
               IPMI_APP_SET_WATCHDOG_TIMER,
               (VOID *)SetWatchdogTimer,
               sizeof (*SetWatchdogTimer),
               (VOID *)CompletionCode,
               &DataSize
               );
  return Status;
}

EFI_STATUS
EFIAPI
IpmiGetWatchdogTimer (
  OUT IPMI_GET_WATCHDOG_TIMER_RESPONSE  *GetWatchdogTimer
  )
{
  EFI_STATUS  Status;
  UINT32      DataSize;

  DataSize = sizeof (*GetWatchdogTimer);
  Status   = IpmiSubmitCommand (
               IPMI_NETFN_APP,
               IPMI_APP_GET_WATCHDOG_TIMER,
               NULL,
               0,
               (VOID *)GetWatchdogTimer,
               &DataSize
               );
  return Status;
}

EFI_STATUS
EFIAPI
IpmiSetBmcGlobalEnables (
  IN  IPMI_SET_BMC_GLOBAL_ENABLES_REQUEST  *SetBmcGlobalEnables,
  OUT UINT8                                *CompletionCode
  )
{
  EFI_STATUS  Status;
  UINT32      DataSize;

  DataSize = sizeof (*CompletionCode);
  Status   = IpmiSubmitCommand (
               IPMI_NETFN_APP,
               IPMI_APP_SET_BMC_GLOBAL_ENABLES,
               (VOID *)SetBmcGlobalEnables,
               sizeof (*SetBmcGlobalEnables),
               (VOID *)CompletionCode,
               &DataSize
               );
  return Status;
}

EFI_STATUS
EFIAPI
IpmiGetBmcGlobalEnables (
  OUT IPMI_GET_BMC_GLOBAL_ENABLES_RESPONSE  *GetBmcGlobalEnables
  )
{
  EFI_STATUS  Status;
  UINT32      DataSize;

  DataSize = sizeof (*GetBmcGlobalEnables);
  Status   = IpmiSubmitCommand (
               IPMI_NETFN_APP,
               IPMI_APP_GET_BMC_GLOBAL_ENABLES,
               NULL,
               0,
               (VOID *)GetBmcGlobalEnables,
               &DataSize
               );
  return Status;
}

EFI_STATUS
EFIAPI
IpmiClearMessageFlags (
  IN  IPMI_CLEAR_MESSAGE_FLAGS_REQUEST  *ClearMessageFlagsRequest,
  OUT UINT8                             *CompletionCode
  )
{
  EFI_STATUS  Status;
  UINT32      DataSize;

  DataSize = sizeof (*CompletionCode);
  Status   = IpmiSubmitCommand (
               IPMI_NETFN_APP,
               IPMI_APP_CLEAR_MESSAGE_FLAGS,
               (VOID *)ClearMessageFlagsRequest,
               sizeof (*ClearMessageFlagsRequest),
               (VOID *)CompletionCode,
               &DataSize
               );
  return Status;
}

EFI_STATUS
EFIAPI
IpmiGetMessageFlags (
  OUT IPMI_GET_MESSAGE_FLAGS_RESPONSE  *GetMessageFlagsResponse
  )
{
  EFI_STATUS  Status;
  UINT32      DataSize;

  DataSize = sizeof (*GetMessageFlagsResponse);
  Status   = IpmiSubmitCommand (
               IPMI_NETFN_APP,
               IPMI_APP_GET_MESSAGE_FLAGS,
               NULL,
               0,
               (VOID *)GetMessageFlagsResponse,
               &DataSize
               );
  return Status;
}

EFI_STATUS
EFIAPI
IpmiGetMessage (
  OUT IPMI_GET_MESSAGE_RESPONSE  *GetMessageResponse,
  IN OUT UINT32                  *GetMessageResponseSize
  )
{
  EFI_STATUS  Status;

  Status = IpmiSubmitCommand (
             IPMI_NETFN_APP,
             IPMI_APP_GET_MESSAGE,
             NULL,
             0,
             (VOID *)GetMessageResponse,
             GetMessageResponseSize
             );
  return Status;
}

EFI_STATUS
EFIAPI
IpmiSendMessage (
  IN  IPMI_SEND_MESSAGE_REQUEST   *SendMessageRequest,
  IN  UINT32                      SendMessageRequestSize,
  OUT IPMI_SEND_MESSAGE_RESPONSE  *SendMessageResponse,
  IN OUT UINT32                   *SendMessageResponseSize
  )
{
  EFI_STATUS  Status;

  Status = IpmiSubmitCommand (
             IPMI_NETFN_APP,
             IPMI_APP_SEND_MESSAGE,
             (VOID *)SendMessageRequest,
             SendMessageRequestSize,
             (VOID *)SendMessageResponse,
             SendMessageResponseSize
             );
  return Status;
}

/**
 * Function which returns the System Guid returned from the BMC
 *
 * @param SystemGuid Pointer the the buffer to fill with System Guid
 * @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
IpmiGetSystemGuid (
  IN EFI_GUID  *SystemGuid
  )
{
  EFI_STATUS  Status;
  UINT32      DataSize;

  //  IPMI_GET_DEVICE_GUID_RESPONSE is associted with Get Device Guid (NetFn App,  Cmd 0x08), but
  //   the response from Get System Guid is the same format. Reusing the structure here.
  IPMI_GET_SYSTEM_GUID_RESPONSE  GuidResponse;

  Status = EFI_INVALID_PARAMETER;
  if (SystemGuid != NULL) {
    DataSize = sizeof (IPMI_GET_DEVICE_GUID_RESPONSE);

    Status = IpmiSubmitCommand (
               IPMI_NETFN_APP,
               IPMI_APP_GET_SYSTEM_GUID,
               NULL,
               0,
               (VOID *)&GuidResponse,
               &DataSize
               );
    if (!EFI_ERROR (Status)) {
      CopyMem ((VOID *)SystemGuid, (VOID *)(GuidResponse.Guid), sizeof (EFI_GUID));
    }
  }

  return Status;
}

/**
 * @brief Query the BMC for the information about the system interface
 *
 * @param Type The interface that is being queried (GET_SYSTEM_INTEFACE_INTERFACE_TYPE)
 * @param GetSystemInterfaceResponse Pointer to a buffer for returning the response data
 * @param ResponseDataSize Size of the buffer for returning the response data
 * @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
IpmiGetSystemInterfaceCapabilities (
  IN GET_SYSTEM_INTEFACE_INTERFACE_TYPE                 Type,
  IN OUT IPMI_GET_SYSTEM_INTERFACE_CAPABILITY_RESPONSE  *GetSystemInterfaceResponse,
  IN OUT UINT32                                         *ResponseDataSize
  )
{
  EFI_STATUS  Status;

  IPMI_GET_SYSTEM_INTERFACE_CAPABILITY_REQUEST  RequestData;

  RequestData.SystemInterfaceType = Type;
  RequestData.Reserved            = 0;

  Status = EFI_INVALID_PARAMETER;

  if ((GetSystemInterfaceResponse != NULL) &&  (ResponseDataSize != NULL) && (*ResponseDataSize >= sizeof (IPMI_GET_SYSTEM_INTERFACE_CAPABILITY_RESPONSE))) {
    Status = IpmiSubmitCommand (
               IPMI_NETFN_APP,
               IPMI_APP_GET_SYSTEM_INTERFACE_CAPABILITIES,
               (VOID *)&RequestData,
               sizeof (IPMI_GET_SYSTEM_INTERFACE_CAPABILITY_REQUEST),
               (VOID *)GetSystemInterfaceResponse,
               ResponseDataSize
               );
  }

  return Status;
}
