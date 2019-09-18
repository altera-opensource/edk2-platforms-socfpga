/** @file
Stratix 10 Clock Manager

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-3-Clause-Patent

**/

#include <Library/IoLib.h>
#include <Library/S10ClockManager/S10ClockManager.h>

UINT32
S10ClockManagerGetPerClock() {
  UINT32 PllGlob, MDiv, RefClkDiv, RefClk;

  PllGlob = MmioRead32(CLOCK_MANAGER_PERPLL + CLOCK_MANAGER_PERPLL_PLLGLOB);

  switch (CLOCK_MANAGER_MAINPLL_PLLGLOB_PSRC(PllGlob)) {
  case CLOCK_MANAGER_PLLGLOB_PSRC_EOSC1:
    RefClk = MmioRead32(S10_SYSTEM_MANAGER + S10_SYSTEM_MANAGER_BOOTSCRATCH_COLD1);
    RefClk = 2500000;
    break;
  case CLOCK_MANAGER_PLLGLOB_PSRC_INTOSC:
    RefClk = S10_CLOCK_INTOSC;
    break;
  case CLOCK_MANAGER_PLLGLOB_PSRC_F2S:
    RefClk = MmioRead32(S10_SYSTEM_MANAGER + S10_SYSTEM_MANAGER_BOOTSCRATCH_COLD2);
    RefClk = 5000000;
    break;
  }

  RefClkDiv = CLOCK_MANAGER_PLLGLOB_REFCLKDIV(PllGlob);

  MDiv = MmioRead32(CLOCK_MANAGER_MAINPLL + CLOCK_MANAGER_FDBCK);

  return (RefClk / RefClkDiv) * (6 + MDiv);
}

UINT32
S10ClockManagerGetMainClock() {
  UINT32 PllGlob, MDiv, RefClkDiv, RefClk;

  PllGlob = MmioRead32(CLOCK_MANAGER_MAINPLL + CLOCK_MANAGER_MAINPLL_PLLGLOB);

  switch (CLOCK_MANAGER_MAINPLL_PLLGLOB_PSRC(PllGlob)) {
  case CLOCK_MANAGER_PLLGLOB_PSRC_EOSC1:
    RefClk = MmioRead32(S10_SYSTEM_MANAGER + S10_SYSTEM_MANAGER_BOOTSCRATCH_COLD1);
    RefClk = 2500000;
    break;
  case CLOCK_MANAGER_PLLGLOB_PSRC_INTOSC:
    RefClk = S10_CLOCK_INTOSC;
    break;
  case CLOCK_MANAGER_PLLGLOB_PSRC_F2S:
    RefClk = MmioRead32(S10_SYSTEM_MANAGER + S10_SYSTEM_MANAGER_BOOTSCRATCH_COLD2);
    RefClk = 5000000;
    break;
  }

  RefClkDiv = CLOCK_MANAGER_PLLGLOB_REFCLKDIV(PllGlob);
  MDiv = CLOCK_MANAGER_FDBCK_MDIV(MmioRead32(CLOCK_MANAGER_MAINPLL + CLOCK_MANAGER_PERPLL_FDBCK));

  return (RefClk / RefClkDiv) * (6 + MDiv);
}

INTN
S10ClockManagerGetL3MainClock() {
  UINT32 Clock;
  UINT32 ClockSrc = MmioRead32(CLOCK_MANAGER_MAINPLL + CLOCK_MANAGER_MAINPLL_NOCCLK);

  ClockSrc = (ClockSrc >> CLOCK_MANAGER_SRC) & CLOCK_MANAGER_SRC_MSK;

  switch (ClockSrc) {
  case CLOCK_MANAGER_SRC_MAIN:
    Clock = S10ClockManagerGetMainClock() /
      CLOCK_MANAGER_PLLC1_DIV(MmioRead32(CLOCK_MANAGER_MAINPLL + CLOCK_MANAGER_PLLC1));
    break;
  case CLOCK_MANAGER_SRC_PERI:
    Clock = S10ClockManagerGetPerClock() /
      CLOCK_MANAGER_PLLC1_DIV(MmioRead32(CLOCK_MANAGER_PERPLL + CLOCK_MANAGER_PLLC1));
    break;
  case CLOCK_MANAGER_SRC_OSC1:
    Clock = MmioRead32(S10_SYSTEM_MANAGER + S10_SYSTEM_MANAGER_BOOTSCRATCH_COLD1);
    break;
  case CLOCK_MANAGER_SRC_INTOSC:
    Clock = S10_CLOCK_INTOSC;
    break;
  case CLOCK_MANAGER_SRC_FPGA:
    Clock = MmioRead32(S10_SYSTEM_MANAGER + S10_SYSTEM_MANAGER_BOOTSCRATCH_COLD2);
    break;
  }

  Clock /= 1 + (MmioRead32(CLOCK_MANAGER_MAINPLL + CLOCK_MANAGER_MAINPLL_NOCCLK) & CLOCK_MANAGER_CNT_MSK);

  return Clock;
}

UINT32
S10ClockManagerGetUartClock() {
  return  S10ClockManagerGetL3MainClock() /
             (1 << (CLOCK_MANAGER_NOCDIV_L4SPCLK(MmioRead32(CLOCK_MANAGER_MAINPLL_NOCDIV))));
}

UINT32
S10ClockManagerGetMmcClock() {
  UINT32 Clock = MmioRead32(CLOCK_MANAGER_PERPLL + CLOCK_MANAGER_PERPLL_CNTR6CLK);

  Clock = (Clock >> CLOCK_MANAGER_SRC) & CLOCK_MANAGER_SRC_MSK;

  switch (Clock) {
  case CLOCK_MANAGER_SRC_MAIN:
    Clock = S10ClockManagerGetL3MainClock();
    Clock /=  1 + (MmioRead32(CLOCK_MANAGER_PERPLL + CLOCK_MANAGER_PERPLL_CNTR6CLK) &
      CLOCK_MANAGER_CNT_MSK);
    break;
  case CLOCK_MANAGER_SRC_PERI:
    Clock = S10ClockManagerGetPerClock();
    Clock /=  1 + (MmioRead32(CLOCK_MANAGER_PERPLL + CLOCK_MANAGER_PERPLL_CNTR6CLK) &
      CLOCK_MANAGER_CNT_MSK);
    break;
  case CLOCK_MANAGER_SRC_OSC1:
    Clock = MmioRead32(S10_SYSTEM_MANAGER + S10_SYSTEM_MANAGER_BOOTSCRATCH_COLD1);
    break;
  case CLOCK_MANAGER_SRC_INTOSC:
    Clock = S10_CLOCK_INTOSC;
    break;
  case CLOCK_MANAGER_SRC_FPGA:
    Clock = MmioRead32(S10_SYSTEM_MANAGER + S10_SYSTEM_MANAGER_BOOTSCRATCH_COLD2);
    break;
  }

  return Clock / 4;
}

