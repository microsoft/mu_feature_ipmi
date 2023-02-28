/** @file
  IPMI Command - NetFnSensor.

  Copyright (c) Microsoft Corporation
  PDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IpmiBaseLib.h>

#include <IpmiFeature.h>

/**
 * @brief Send the Set Sensor Threshold command for a specified SensorNumber
 *
 * @param SetSensorThresholdRequestData The filled out Set Sensor Threshold command structure
 * @param CompletionCode Pointer to a buffer for returning the completion code
 * @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
IpmiSetSensorThreshold (
  IN IPMI_SENSOR_SET_SENSOR_THRESHOLD_REQUEST_DATA  *SetSensorThresholdRequestData,
  OUT UINT8                                         *CompletionCode
  )
{
  EFI_STATUS  Status;
  UINT32      ResponseDataSize;

  Status = EFI_INVALID_PARAMETER;

  if ((SetSensorThresholdRequestData != NULL) && (CompletionCode != NULL)) {
    ResponseDataSize = sizeof (*CompletionCode);

    Status = IpmiSubmitCommand (
               IPMI_NETFN_SENSOR_EVENT,
               IPMI_SENSOR_SET_SENSOR_THRESHOLDS,
               (UINT8 *)SetSensorThresholdRequestData,
               sizeof (*SetSensorThresholdRequestData),
               (UINT8 *)&CompletionCode,
               &ResponseDataSize
               );
  }

  return Status;
}

/**
 * @brief Query the threshold data of the specified SensorNumber.
 *
 * @param SensorNumber The unique identifier of the sensor being queried.
 * @param GetSensorThresholdResponse Pointer to a buffer for returning the threshold response data.
 * @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
IpmiGetSensorThreshold (
  IN UINT8                                            SensorNumber,
  OUT IPMI_SENSOR_GET_SENSOR_THRESHOLD_RESPONSE_DATA  *GetSensorThresholdResponse
  )
{
  EFI_STATUS  Status;
  UINT32      ResponseDataSize;

  Status = EFI_INVALID_PARAMETER;
  if (GetSensorThresholdResponse != NULL) {
    ResponseDataSize = sizeof (IPMI_SENSOR_GET_SENSOR_THRESHOLD_RESPONSE_DATA);

    ZeroMem (&GetSensorThresholdResponse, sizeof (*GetSensorThresholdResponse));

    Status = IpmiSubmitCommand (
               IPMI_NETFN_SENSOR_EVENT,
               IPMI_SENSOR_GET_SENSOR_THRESHOLDS,
               (UINT8 *)&SensorNumber,
               sizeof (UINT8),
               (UINT8 *)&GetSensorThresholdResponse,
               &ResponseDataSize
               );
  }

  return Status;
}
