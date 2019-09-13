/** @file
  Platform Hook Library

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-3-Clause-Patent

**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/PlatformHookLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/AgilexClockManager/AgilexClockManager.h>

/**
  Performs platform specific initialization required for the CPU to access
  the hardware associated with a SerialPortLib instance.  This function does
  not initialize the serial port hardware itself.  Instead, it initializes
  hardware devices that are required for the CPU to access the serial port
  hardware.  This function may be called more than once.

  @retval RETURN_SUCCESS       The platform specific initialization succeeded.
  @retval RETURN_DEVICE_ERROR  The platform specific initialization could not be completed.

**/
RETURN_STATUS
EFIAPI
PlatformHookSerialPortInitialize (
  VOID
  )
{
  RETURN_STATUS Status;

  Status = PcdSet32S (PcdSerialClockRate, GetUartClk());
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  return RETURN_SUCCESS;
}
