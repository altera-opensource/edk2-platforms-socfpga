/** @file

  Stratix 10 Platform Entry code

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-3-Clause-Patent

**/


#include <Uefi.h>
#include <Guid/GlobalVariable.h>
#include <Library/ArmLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/NonDiscoverableDeviceRegistrationLib.h>
#include <Protocol/NonDiscoverableDevice.h>
#include <Library/PrintLib.h>
#include <Library/S10ClockManager/S10ClockManager.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/DevicePathFromText.h>

#include <Protocol/PlatformDwMmc.h>

DW_MMC_HC_SLOT_CAP
DwMmcCapability[1] = {
  {
    .HighSpeed   = 1,
    .BusWidth    = 1,
    .SlotType    = RemovableSlot,
    .CardType    = SdCardType,
    .Voltage30   = 1,
    .BaseClkFreq = 5000
  }
};

EFI_STATUS
EFIAPI
IntelSocFpgaMmcGetCapability (
  IN     EFI_HANDLE           Controller,
  IN     UINT8                Slot,
     OUT DW_MMC_HC_SLOT_CAP   *Capability
  )
{
  if (Capability == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  CopyMem (Capability, &DwMmcCapability[0], sizeof (DW_MMC_HC_SLOT_CAP));
  Capability->BaseClkFreq = S10ClockManagerGetMmcClock() / 1000;

  return EFI_SUCCESS;
}

BOOLEAN
EFIAPI
IntelSocFpgaMmcCardDetect (
  IN EFI_HANDLE               Controller,
  IN UINT8                    Slot
  )
{
  return TRUE;
}


PLATFORM_DW_MMC_PROTOCOL mDwMmcDevice = {
IntelSocFpgaMmcGetCapability,
IntelSocFpgaMmcCardDetect
};

EFI_STATUS
EFIAPI
IntelPlatformDxeEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS            Status = 0;

  Status = RegisterNonDiscoverableMmioDevice (
             NonDiscoverableDeviceTypeSdhci,
             NonDiscoverableDeviceDmaTypeNonCoherent,
             NULL,
             NULL,
             1,
             0xff808000, // SD
             SIZE_4KB
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gPlatformDwMmcProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mDwMmcDevice);


  return Status;
}

