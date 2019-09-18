/** @file

  Stratix 10 Mmu configuration

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-3-Clause-Patent

**/

#include <Library/ArmLib.h>
#include <Library/ArmMmuLib.h>
#include <Library/ArmPlatformLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/TimerLib.h>

// The total number of descriptors, including the final "end-of-table" descriptor.
#define MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS 16
ARM_MEMORY_REGION_DESCRIPTOR  gVirtualMemoryTable[MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS];

// DDR attributes
#define DDR_ATTRIBUTES_CACHED           ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK
#define DDR_ATTRIBUTES_UNCACHED         ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED

#define DRAM_BASE                0x0
#define DRAM_SIZE                0x40000000

#define FPGA_SLAVES_BASE         0x80000000
#define FPGA_SLAVES_SIZE         0x60000000

#define PERIPHERAL_BASE          0xF7000000
#define PERIPHERAL_SIZE          0x08E00000

#define OCRAM_BASE               0xFFE00000
#define OCRAM_SIZE               0x00100000

#define GIC_BASE                 0xFFFC0000
#define GIC_SIZE                 0x00008000

#define MEM64_BASE               0x0100000000
#define MEM64_SIZE               0x1F00000000

#define DEVICE64_BASE            0x2000000000
#define DEVICE64_SIZE            0x0100000000
/**
  Return the Virtual Memory Map of your platform

  This Virtual Memory Map is used to initialize the MMU for DXE Phase.

  @param[out]   VirtualMemoryMap    Array of ARM_MEMORY_REGION_DESCRIPTOR describing a Physical-to-
                                    Virtual Memory mapping. This array must be ended by a zero-filled
                                    entry

**/
VOID
EFIAPI
ArmPlatformGetVirtualMemoryMap (
  IN ARM_MEMORY_REGION_DESCRIPTOR** VirtualMemoryMap
  )
{
  ARM_MEMORY_REGION_DESCRIPTOR  *VirtualMemoryTable;
  ARM_MEMORY_REGION_ATTRIBUTES  CacheAttributes;
  UINTN                         Index = 0;

  VirtualMemoryTable = &gVirtualMemoryTable[0];

  CacheAttributes = DDR_ATTRIBUTES_CACHED;

  // Start create the Virtual Memory Map table
  // Our goal is to a simple 1:1 mapping where virtual==physical address

  // DDR SDRAM
  VirtualMemoryTable[Index].PhysicalBase = DRAM_BASE;
  VirtualMemoryTable[Index].VirtualBase  = VirtualMemoryTable[Index].PhysicalBase;
  VirtualMemoryTable[Index].Length       = DRAM_SIZE;
  VirtualMemoryTable[Index++].Attributes = CacheAttributes;

  // FPGA
  VirtualMemoryTable[Index].PhysicalBase = FPGA_SLAVES_BASE;
  VirtualMemoryTable[Index].VirtualBase  = VirtualMemoryTable[Index].PhysicalBase;
  VirtualMemoryTable[Index].Length       = FPGA_SLAVES_SIZE;
  VirtualMemoryTable[Index++].Attributes = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // DEVICE 142MB
  VirtualMemoryTable[Index].PhysicalBase = PERIPHERAL_BASE;
  VirtualMemoryTable[Index].VirtualBase  = VirtualMemoryTable[Index].PhysicalBase;
  VirtualMemoryTable[Index].Length       = PERIPHERAL_SIZE;
  VirtualMemoryTable[Index++].Attributes = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // OCRAM 1MB but available 256KB
  VirtualMemoryTable[Index].PhysicalBase = OCRAM_BASE;
  VirtualMemoryTable[Index].VirtualBase  = VirtualMemoryTable[Index].PhysicalBase;
  VirtualMemoryTable[Index].Length       = OCRAM_SIZE;
  VirtualMemoryTable[Index++].Attributes = CacheAttributes;

   // DEVICE 32KB
  VirtualMemoryTable[Index].PhysicalBase = GIC_BASE;
  VirtualMemoryTable[Index].VirtualBase  = VirtualMemoryTable[Index].PhysicalBase;
  VirtualMemoryTable[Index].Length       = GIC_SIZE;
  VirtualMemoryTable[Index++].Attributes = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // MEM 124GB
  VirtualMemoryTable[Index].PhysicalBase = MEM64_BASE;
  VirtualMemoryTable[Index].VirtualBase  = VirtualMemoryTable[Index].PhysicalBase;
  VirtualMemoryTable[Index].Length       = MEM64_SIZE;
  VirtualMemoryTable[Index++].Attributes = CacheAttributes;

  // DEVICE 4GB
  VirtualMemoryTable[Index].PhysicalBase = DEVICE64_BASE;
  VirtualMemoryTable[Index].VirtualBase  = VirtualMemoryTable[Index].PhysicalBase;
  VirtualMemoryTable[Index].Length       = DEVICE64_SIZE;
  VirtualMemoryTable[Index++].Attributes = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // End of Table
  VirtualMemoryTable[Index].PhysicalBase = 0;
  VirtualMemoryTable[Index].VirtualBase  = 0;
  VirtualMemoryTable[Index].Length       = 0;
  VirtualMemoryTable[Index++].Attributes = (ARM_MEMORY_REGION_ATTRIBUTES)0;

  ASSERT((Index) <= MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS);

  *VirtualMemoryMap = VirtualMemoryTable;
}


VOID
EFIAPI
InitMmu (
  VOID
  )
{
  ARM_MEMORY_REGION_DESCRIPTOR  *MemoryTable;
  VOID                          *TranslationTableBase;
  UINTN                         TranslationTableSize;
  RETURN_STATUS                 Status;
  // Construct a Virtual Memory Map for this platform
  ArmPlatformGetVirtualMemoryMap (&MemoryTable);

  // Configure the MMU
  Status = ArmConfigureMmu (MemoryTable, &TranslationTableBase, &TranslationTableSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Error: Failed to enable MMU\n"));
  }
}

