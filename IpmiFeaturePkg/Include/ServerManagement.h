/** @file
  Generic Definations for Server Management Header File.

  @copyright
  Copyright 1999 - 2021 Intel Corporation. <BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _SERVER_MANAGEMENT_H_
#define _SERVER_MANAGEMENT_H_

//
// Update Mode Bits
//
#define BMC_READY  0

//
// Server Management COM Addressing types
//
typedef enum {
  SmReserved,
  SmIpmb,
  SmIcmb1_0,
  SmIcmb0_9,
  Sm802_3_Lan,
  SmRs_232,
  SmOtherLan,
  SmPciSmBus,
  SmSmBus1_0,
  SmSmBus2_0,
  SmUsb1_x,
  SmUsb2_x,
  SmBmc
} SM_CHANNEL_MEDIA_TYPE;

typedef enum {
  SmTcp,
  SmUdp,
  SmIcmp,
  SmIpmi
} SM_PROTOCOL_TYPE;

typedef enum {
  SmMessage,
  SmRawData
} SM_DATA_TYPE;

typedef struct {
  BOOLEAN    IpAddressType;
  UINT16     IpPort;
  UINT8      IpAddress[16];
} SM_IP_ADDRESS;

typedef struct {
  UINT8    SlaveAddress;
  UINT8    LunAddress;
  UINT8    NetFunction;
  UINT8    ChannelAddress;
} SM_IPMI_ADDRESS;

typedef struct {
  UINT8    SerialPortNumber;
} SM_SERIAL_ADDRESS;

typedef union {
  SM_IP_ADDRESS        IpAddress;
  SM_IPMI_ADDRESS      BmcAddress;
  SM_SERIAL_ADDRESS    SerialAddress;
} SM_COM_ADDRESS_TYPE;

typedef struct {
  SM_CHANNEL_MEDIA_TYPE    ChannelType;
  SM_COM_ADDRESS_TYPE      Address;
} SM_COM_ADDRESS;

#pragma pack(1)

//
// Server Management Controller Information
//
typedef struct {
  UINT8     CompletionCode;
  UINT8     DeviceId;
  UINT8     DeviceRevision       : 4;
  UINT8     Reserved             : 3;
  UINT8     DeviceSdr            : 1;
  UINT8     MajorFirmwareRev     : 7;
  UINT8     UpdateMode           : 1;
  UINT8     MinorFirmwareRev;
  UINT8     SpecificationVersion;
  UINT8     SensorDeviceSupport  : 1;
  UINT8     SdrRepositorySupport : 1;
  UINT8     SelDeviceSupport     : 1;
  UINT8     FruInventorySupport  : 1;
  UINT8     IPMBMessageReceiver  : 1;
  UINT8     IPMBMessageGenerator : 1;
  UINT8     BridgeSupport        : 1;
  UINT8     ChassisSupport       : 1;
  UINT8     ManufacturerId[3];
  UINT16    ProductId;
  UINT32    AuxFirmwareRevInfo;
} SM_CTRL_INFO;

#pragma pack()

#endif // _SERVER_MANAGEMENT_H_
