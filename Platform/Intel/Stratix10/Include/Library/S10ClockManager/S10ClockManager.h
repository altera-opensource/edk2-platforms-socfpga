/** @file
Stratix 10 Clock Manager header

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-3-Clause-Patent

**/

#ifndef _S10_CLOCK_MANAGER_
#define _S10_CLOCK_MANAGER_
#define CLOCK_MANAGER_MAINPLL                      0xffd10030
#define CLOCK_MANAGER_MAINPLL_NOCCLK               0x4c

#define CLOCK_MANAGER_PERPLL                       0xffd100a4
#define CLOCK_MANAGER_MAINPLL_PLLGLOB              0x44
#define CLOCK_MANAGER_PERPLL_PLLGLOB               0x40
#define CLOCK_MANAGER_MAINPLL_PLLGLOB_PSRC(x)      (((x) >> 16) & 3)
#define CLOCK_MANAGER_CNTR6CLK                     0x4c
#define CLOCK_MANAGER_PLLGLOB_PSRC_EOSC1           0
#define CLOCK_MANAGER_PLLGLOB_PSRC_INTOSC          1
#define CLOCK_MANAGER_PLLGLOB_PSRC_F2S             2
#define CLOCK_MANAGER_SRC                          16
#define CLOCK_MANAGER_SRC_MSK                      0x7
#define CLOCK_MANAGER_SRC_MAIN                     (0)
#define CLOCK_MANAGER_SRC_PERI                     (1)
#define CLOCK_MANAGER_SRC_OSC1                     (2)
#define CLOCK_MANAGER_SRC_INTOSC                   (3)
#define CLOCK_MANAGER_SRC_FPGA                     (4)
#define CLOCK_MANAGER_FDBCK                        0x44
#define CLOCK_MANAGER_FDBCK_MDIV(x)                ((x) >> 24 & 0xff)
#define CLOCK_MANAGER_PERPLL_FDBCK                 0x48
#define CLOCK_MANAGER_CNT_MSK                      0x3ff
#define CLOCK_MANAGER_PERPLL_CNTR6CLK              0x28
#define CLOCK_MANAGER_PLLGLOB_REFCLKDIV(x)         (((x) >> 8) & 0x3f)
#define CLOCK_MANAGER_PLLC1_DIV(x)                 ((x) & 0x7f)
#define CLOCK_MANAGER_PLLC1                        0x54
#define CLOCK_MANAGER_NOCDIV_L4SPCLK(x)            ((x) >> 16 & 0x3)
#define CLOCK_MANAGER_MAINPLL_NOCDIV               0x40

#define S10_SYSTEM_MANAGER                         0xffd12000
#define S10_SYSTEM_MANAGER_BOOTSCRATCH_COLD1       0x204
#define S10_SYSTEM_MANAGER_BOOTSCRATCH_COLD2       0x208
#define S10_CLOCK_INTOSC                           460000000

UINT32 S10ClockManagerGetMmcClock();
UINT32 S10ClockManagerGetUartClock();

#endif
