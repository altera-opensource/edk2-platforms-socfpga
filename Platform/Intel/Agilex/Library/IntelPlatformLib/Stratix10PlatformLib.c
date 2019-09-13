/** @file
  Stratix 10 Platform Library

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-3-Clause-Patent

**/

#include <Library/ArmLib.h>
#include <Library/ArmPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/TimerLib.h>
#include <Ppi/ArmMpCoreInfo.h>

#define ALT_RSTMGR_OFST                            0xffd11000
#define ALT_RSTMGR_PER1MODRST_OFST                 0x28
#define ALT_RSTMGR_PER1MODRST_WATCHDOG0_SET_MSK    0x00000001
#define ALT_RSTMGR_PER1MODRST_L4SYSTIMER0_CLR_MSK  0xffffffef

STATIC EFI_GUID mArmMpCoreInfoPpiGuid = ARM_MP_CORE_INFO_PPI_GUID;
// This Table will be consume by Hob init code to publish it into HOB as MPCore Info
// Hob init code will retrieve it by calling PrePeiCoreGetMpCoreInfo via Ppi
STATIC ARM_CORE_INFO mArmPlatformNullMpCoreInfoTable[] = {
  {
    // Cluster 0, Core 0
    0x0, 0x0,

    // MP Core MailBox Set/Get/Clear Addresses and Clear Value
    (EFI_PHYSICAL_ADDRESS)0,
    (EFI_PHYSICAL_ADDRESS)0,
    (EFI_PHYSICAL_ADDRESS)0,
    (UINT64)0xFFFFFFFF
  },
  {
    // Cluster 0, Core 1
    0x0, 0x1,

    // MP Core MailBox Set/Get/Clear Addresses and Clear Value
    (EFI_PHYSICAL_ADDRESS)0,
    (EFI_PHYSICAL_ADDRESS)0,
    (EFI_PHYSICAL_ADDRESS)0,
    (UINT64)0xFFFFFFFF
  },
  {
    // Cluster 0, Core 2
    0x0, 0x2,

    // MP Core MailBox Set/Get/Clear Addresses and Clear Value
    (EFI_PHYSICAL_ADDRESS)0,
    (EFI_PHYSICAL_ADDRESS)0,
    (EFI_PHYSICAL_ADDRESS)0,
    (UINT64)0xFFFFFFFF
  },
  {
    // Cluster 0, Core 3
    0x0, 0x3,

    // MP Core MailBox Set/Get/Clear Addresses and Clear Value
    (EFI_PHYSICAL_ADDRESS)0,
    (EFI_PHYSICAL_ADDRESS)0,
    (EFI_PHYSICAL_ADDRESS)0,
    (UINT64)0xFFFFFFFF
  }
};

STATIC
VOID
AssertWatchDogTimerZeroReset (
  VOID
  )
{
  // Assert the Reset signal of Watchdog Timer 0 which may have been enabled by BootROM
  MmioOr32 (ALT_RSTMGR_OFST +
            ALT_RSTMGR_PER1MODRST_OFST,
            ALT_RSTMGR_PER1MODRST_WATCHDOG0_SET_MSK);
}

/**
 *   Return the current Boot Mode
 *
 *     This function returns the boot reason on the platform
 *
 *     **/
EFI_BOOT_MODE
ArmPlatformGetBootMode (
  VOID
  )
{
  return BOOT_WITH_FULL_CONFIGURATION;
}


/**
  Initialize controllers that must setup before entering PEI MAIN
**/
RETURN_STATUS
ArmPlatformInitialize (
  IN  UINTN                     MpId
  )
{
  AssertWatchDogTimerZeroReset();
  return EFI_SUCCESS;
}

//-----------------------------------------------------------------------------------------
// BEGIN ARM CPU RELATED CODE
//-----------------------------------------------------------------------------------------

EFI_STATUS
PrePeiCoreGetMpCoreInfo (
  OUT UINTN                   *CoreCount,
  OUT ARM_CORE_INFO           **ArmCoreTable
  )
{
  *CoreCount    = ARRAY_SIZE(mArmPlatformNullMpCoreInfoTable);
  *ArmCoreTable = mArmPlatformNullMpCoreInfoTable;
  return EFI_SUCCESS;
}

ARM_MP_CORE_INFO_PPI mMpCoreInfoPpi = { PrePeiCoreGetMpCoreInfo };

EFI_PEI_PPI_DESCRIPTOR      gPlatformPpiTable[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    &mArmMpCoreInfoPpiGuid,
    &mMpCoreInfoPpi
  }
};

VOID
ArmPlatformGetPlatformPpiList (
  OUT UINTN                   *PpiListSize,
  OUT EFI_PEI_PPI_DESCRIPTOR  **PpiList
  )
{
  *PpiListSize = sizeof(gPlatformPpiTable);
  *PpiList = gPlatformPpiTable;
}

//-----------------------------------------------------------------------------------------
// END ARM CPU RELATED CODE
//-----------------------------------------------------------------------------------------

