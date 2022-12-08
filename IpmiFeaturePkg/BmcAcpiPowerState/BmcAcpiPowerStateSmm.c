/** @file
    BmcAcpiPowerStateSmm.c

    Copyright (c) Microsoft Corporation. All rights reserved.
    SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiSmm.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/IpmiBaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <IndustryStandard/IpmiNetFnApp.h>

#include <Protocol/SmmExitBootServices.h>
#include <Protocol/SmmSxDispatch2.h>

/**
  Send IPMI command to set ACPI power state to BMC.

  @param[in]  SystemPowerState        System Power State enumeration
  @param[in]  DevicePowerState        Device Power State enumeration

  @retval EFI_SUCCESS     Set ACPI power state to BMC successfully.
  @retval Other           Failed to set ACPI power state to BMC.
**/
EFI_STATUS
SetBmcAcpiPowerState (
  IN  UINT8  SystemPowerState,
  IN  UINT8  DevicePowerState
  )
{
  EFI_STATUS                         Status;
  IPMI_SET_ACPI_POWER_STATE_REQUEST  SetAcpiPowerState;
  UINT8                              CompletionCode;
  UINT32                             DataSize;

  ZeroMem (&SetAcpiPowerState, sizeof (SetAcpiPowerState));
  SetAcpiPowerState.SystemPowerState.Bits.StateChange = 1;
  SetAcpiPowerState.SystemPowerState.Bits.PowerState  = SystemPowerState;
  SetAcpiPowerState.DevicePowerState.Bits.StateChange = 1;
  SetAcpiPowerState.DevicePowerState.Bits.PowerState  = DevicePowerState;

  DataSize = sizeof (CompletionCode);
  Status   = IpmiSubmitCommand (
               IPMI_NETFN_APP,
               IPMI_APP_SET_ACPI_POWERSTATE,
               (UINT8 *)&SetAcpiPowerState,
               sizeof (SetAcpiPowerState),
               &CompletionCode,
               &DataSize
               );

  DEBUG ((DEBUG_ERROR, "[%a] --- %r\n", __FUNCTION__, Status));

  return Status;
}

/**
  Main entry point for an SMM handler dispatch or communicate-based callback.

  @param[in]     DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]     Context         Points to an optional handler context which was specified when the
                                 handler was registered.
  @param[in,out] CommBuffer      A pointer to a collection of data in memory that will
                                 be conveyed from a non-SMM environment into an SMM environment.
  @param[in,out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS            Update ACPI power state to BMC successfully.
**/
EFI_STATUS
EFIAPI
BmcAcpiPowerStateS5EntryCallBack (
  IN           EFI_HANDLE  DispatchHandle,
  IN     CONST VOID        *Context         OPTIONAL,
  IN OUT       VOID        *CommBuffer      OPTIONAL,
  IN OUT       UINTN       *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS  Status;

  DEBUG ((DEBUG_INFO, "[%a] --- Start\n", __FUNCTION__));

  Status = SetBmcAcpiPowerState (IPMI_SYSTEM_POWER_STATE_S5_G2, IPMI_DEVICE_POWER_STATE_D3);

  DEBUG ((DEBUG_INFO, "[%a] --- End %r\n", __FUNCTION__, Status));

  return Status;
}

/**
  Notification for installation of SMM Exit Boot Service protocol
  which indicates system booted to OS or runtime.

  @param[in] Protocol   Points to the protocol's unique identifier.
  @param[in] Interface  Points to the interface instance.
  @param[in] Handle     The handle on which the interface was installed.

  @retval EFI_SUCCESS   Update ACPI power state to BMC successfully.
**/
EFI_STATUS
EFIAPI
BmcAcpiPowerStateExitBootServicesCallback (
  IN      CONST EFI_GUID  *Protocol,
  IN      VOID            *Interface,
  IN      EFI_HANDLE      Handle
  )
{
  EFI_STATUS  Status;

  DEBUG ((DEBUG_INFO, "[%a] --- Start\n", __FUNCTION__));

  Status = SetBmcAcpiPowerState (IPMI_SYSTEM_POWER_STATE_S0_G0, IPMI_DEVICE_POWER_STATE_D0);

  DEBUG ((DEBUG_INFO, "[%a] --- End %r\n", __FUNCTION__, Status));
  return Status;
}

/**
  This is the standard driver entry point.
  This function will create event of ExitBootService.

  @param ImageHandle      Handle for the image of this driver.
  @param SystemTable      Pointer to the EFI System Table.

  @retval EFI_SUCCESS     Action is performed successfully.
  @retval Other           Error occurred during execution.
**/
EFI_STATUS
EFIAPI
InitializeBmcAcpiPowerStateSmm (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                     Status = EFI_SUCCESS;
  EFI_SMM_SX_DISPATCH2_PROTOCOL  *SxDispatch;
  EFI_SMM_SX_REGISTER_CONTEXT    EntryRegisterContext;
  EFI_HANDLE                     S5EntryHandle;
  VOID                           *Registration;

  DEBUG ((DEBUG_INFO, "[%a] --- Start\n", __FUNCTION__));

  if (gSmst == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // Locate SmmSxDispatch2 protocol.
  //
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSxDispatch2ProtocolGuid,
                    NULL,
                    (VOID **)&SxDispatch
                    );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[%a] SmmLocateProtocol(gEfiSmmSxDispatch2ProtocolGuid): %r\n", __FUNCTION__, Status));
    return Status;
  }

  //
  // Register a S5 entry callback function
  //
  EntryRegisterContext.Type  = SxS5;
  EntryRegisterContext.Phase = SxEntry;
  Status                     = SxDispatch->Register (
                                             SxDispatch,
                                             BmcAcpiPowerStateS5EntryCallBack,
                                             &EntryRegisterContext,
                                             &S5EntryHandle
                                             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[%a] SxDispatch(SmmS5EntryCallBack): %r\n", __FUNCTION__, Status));
  }

  //
  // Register a SMM exit boot service callback
  //
  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEdkiiSmmExitBootServicesProtocolGuid,
                    BmcAcpiPowerStateExitBootServicesCallback,
                    &Registration
                    );

  DEBUG ((DEBUG_INFO, "[%a] --- End %r\n", __FUNCTION__, Status));

  return Status;
}
