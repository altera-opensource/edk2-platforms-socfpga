Intel Stratix 10 Platform
=========================

# Summary

This is a port of 64-bit Tiano Core UEFI for the Intel Stratix 10 platform
based on Stratix 10 SX development board.

This UEFI port works with ATF + UEFI implementation for Intel Stratix 10 board, and
will boot to Linux port of Stratix 10.

# Status

This firmware has been validated to boot to Linux for Stratix 10 that can be obtained from
https://github.com/altera-opensource/linux-socfpga.

The default boot is the UEFI shell. The UEFI
shell will run startup.nsh by default, and you may change the startup.nsh to run commands on boot.

# Building the firmware

-  Fetch the ATF, edk2, and edk2-platforms repositories into local host.
   Make all the repositories in the same ${BUILD\_PATH}.

-  Install the AARCH64 GNU 4.8 toolchain.

-  Build UEFI using Stratix 10 platform as configuration

       . edksetup.sh

       build -a AARCH64 -p Platform/Intel/Stratix10/Stratix10SoCPkg.dsc -t GCC5 -b RELEASE -y report.log -j build.log -Y PCD -Y LIBRARY -Y FLASH -Y DEPEX -Y BUILD_FLAGS -Y FIXED_ADDRESS

Note: Refer to build instructions from the top level edk2-platforms Readme.md for further details

- Build ATF for Stratix 10 platform (commit id:2cbeee4d519bac0d79da98faae969fae9f9558f9 is tested and known working)

       make CROSS_COMPILE=aarch64-linux-gnu- device=s10

-  Build atf providing the previously generated UEFI as the BL33 image

       make CROSS_COMPILE=aarch64-linux-gnu- bl2 fip PLAT=stratix10
       BL33=PEI.ROM

Install Procedure
-----------------

- dd fip.bin to a A2 partition on the MMC drive to be booted in Stratix 10
  board.

- Generate a SOF containing bl2

.. code:: bash
        aarch64-linux-gnu-objcopy -I binary -O ihex --change-addresses 0xffe00000 bl2.bin bl2.hex
        quartus_cpf --bootloader bl2.hex <quartus_generated_sof> <output_sof_with_bl2>

- Configure SOF to board

.. code:: bash
        nios2-configure-sof <output_sof_with_bl2>


