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
#include <Library/UefiBootManagerLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/DevicePathFromText.h>
#include <Protocol/PlatformBootManager.h>


#include <Protocol/PlatformDwMmc.h>

#define BOOT_OPTION_NUM 1

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
  Capability->BaseClkFreq = S10ClockManagerGetMmcClock();

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

STATIC
EFI_STATUS
GetPlatformBootOptionsAndKeys (
  OUT UINTN                              *BootCount,
  OUT EFI_BOOT_MANAGER_LOAD_OPTION       **BootOptions,
  OUT EFI_INPUT_KEY                      **BootKeys
  )
{
  CHAR16                                 *BootFileNameStr;
  CHAR16                                 *LinuxBootArgs;
  EFI_STATUS                             Status;
  UINTN                                  Size;
  EFI_DEVICE_PATH_PROTOCOL        *FilePath;
  UINTN                                  NumBootCount;

  NumBootCount = 0;
  Size = sizeof (EFI_BOOT_MANAGER_LOAD_OPTION) * BOOT_OPTION_NUM;
  *BootOptions = (EFI_BOOT_MANAGER_LOAD_OPTION *)AllocateZeroPool (Size);
  if (*BootOptions == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to allocate memory for BootOptions\n"));
    return EFI_OUT_OF_RESOURCES;
  }
  Size = sizeof (EFI_INPUT_KEY) * BOOT_OPTION_NUM;
  *BootKeys = (EFI_INPUT_KEY *)AllocateZeroPool (Size);
  if (*BootKeys == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to allocate memory for BootKeys\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }

  BootFileNameStr = (CHAR16 *)PcdGetPtr (PcdSdBootFilename);
  FilePath = FileDevicePath (NULL, BootFileNameStr);

  if (FilePath == NULL) {
    DEBUG ((DEBUG_ERROR, "Fail to allocate memory for default boot file path. Unable to boot.\n"));
    goto SkipSdBoot;
  }


  LinuxBootArgs = (CHAR16 *)PcdGetPtr (PcdSocFpgaBootArgs);

  Status = EfiBootManagerInitializeLoadOption (
             &(*BootOptions)[0],
             LoadOptionNumberUnassigned,
             LoadOptionTypeBoot,
             LOAD_OPTION_ACTIVE,
             L"Default Boot",
             FilePath,
             (UINT8*)LinuxBootArgs,
             StrLen(LinuxBootArgs)*sizeof(LinuxBootArgs)
             );
  ASSERT_EFI_ERROR (Status);

  NumBootCount++;

SkipSdBoot:

  *BootCount = NumBootCount;

  return EFI_SUCCESS;
Error:
  FreePool (*BootOptions);
  return Status;
}


PLATFORM_BOOT_MANAGER_PROTOCOL mPlatformBootManager = {
  GetPlatformBootOptionsAndKeys
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

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gPlatformBootManagerProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPlatformBootManager
                  );


  return Status;
}

