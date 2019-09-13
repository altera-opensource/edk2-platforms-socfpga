/** @file
Stratix 10 Clock Manager

Copyright (c) 2019, Intel Corporation. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause-Patent

**/

#include <Library/IoLib.h>
#include <Library/AgilexClockManager/AgilexClockManager.h>

/* Extract reference clock from platform clock source */
UINT32 GetRefClk(
IN UINT32 PllGlob
)
{
	UINT32 ARefClkDiv, RefClk;
	UINT32 ScrReg;

	switch (CLKMGR_PSRC(PllGlob)) {
	case CLKMGR_PLLGLOB_PSRC_EOSC1:
                ScrReg = AGILEX_SYSTEM_MANAGER + AGILEX_SYSTEM_MANAGER_BOOTSCRATCH_COLD1;
		RefClk = MmioRead32(ScrReg);
		break;
	case CLKMGR_PLLGLOB_PSRC_INTOSC:
		RefClk = CLKMGR_INTOSC_HZ;
		break;
	case CLKMGR_PLLGLOB_PSRC_F2S:
                ScrReg = AGILEX_SYSTEM_MANAGER + AGILEX_SYSTEM_MANAGER_BOOTSCRATCH_COLD2;
		RefClk = MmioRead32(ScrReg);
		break;
	default:
		RefClk = 0;
		break;
	}

	ARefClkDiv = CLKMGR_PLLGLOB_AREFCLKDIV(PllGlob);
	RefClk /= ARefClkDiv;

	return RefClk;
}

/* Calculate clock frequency based on parameter */
UINT32
GetClkFreq(
UINT32 PSrcReg,
UINT32 MainPllc,
UINT32 PerPllc)
{
  UINT32 ClkPsrc, Mdiv, RefClk;
  UINT32 PllmReg, PllcReg, PllcDiv, PllGlobReg;

  ClkPsrc = MmioRead32(CLKMGR_MAINPLL + PSrcReg);

  switch (CLKMGR_PSRC(ClkPsrc)) {
	case CLKMGR_PSRC_MAIN:
		PllmReg = CLKMGR_MAINPLL + CLKMGR_MAINPLL_PLLM;
		PllcReg = CLKMGR_MAINPLL + MainPllc;
		PllGlobReg = CLKMGR_MAINPLL + CLKMGR_MAINPLL_PLLGLOB;
		break;
	case CLKMGR_PSRC_PER:
		PllmReg = CLKMGR_PERPLL + CLKMGR_PERPLL_PLLM;
		PllcReg = CLKMGR_PERPLL + PerPllc;
		PllGlobReg = CLKMGR_PERPLL + CLKMGR_PERPLL_PLLGLOB;
		break;
	default:
		return 0;
	}

	RefClk = GetRefClk(MmioRead32(PllGlobReg));
	Mdiv = CLKMGR_PLLM_MDIV(MmioRead32(PllmReg));
	RefClk *= Mdiv;

	PllcDiv = MmioRead32(PllcReg) & 0x7ff;

	return RefClk / PllcDiv;
}

/* Return L3 interconnect clock */
UINT32
GetL3Clk(
)
{
	UINT32 L3Clk;

	L3Clk = GetClkFreq(CLKMGR_MAINPLL_NOCCLK, CLKMGR_MAINPLL_PLLC1,
				CLKMGR_PERPLL_PLLC1);

	return L3Clk;
}

/* Calculate clock frequency to be used for watchdog timer */
UINT32 
GetWdtClock(
)
{
	UINT32 L3Clk, L4SysClk;

	L3Clk = GetL3Clk();
	L4SysClk = L3Clk / 4;

	return L4SysClk;
}
UINT32 agxprint() {
return GetUartClk();
}

UINT32 agxprint1() {
return GetL3Clk();
}

UINT32 agxprint2() {
return CLKMGR_PLLM_MDIV(MmioRead32(CLKMGR_PERPLL + CLKMGR_PERPLL_PLLM));
}

UINT32 agxprint3() {
return GetUartClk();
}

/* Calculate clock frequency to be used for UART driver */
UINT32
GetUartClk(
)
{
	UINT32 Data32, L3Clk, L4SpClk;

	L3Clk = GetL3Clk();

	Data32 = MmioRead32(CLKMGR_MAINPLL + CLKMGR_MAINPLL_NOCDIV);
	Data32 = (Data32 >> 16) & 0x3;

	L4SpClk = L3Clk >> Data32;

	return L4SpClk;
}

/* Calculate clock frequency to be used for SDMMC driver */
UINT32
GetMmcClk(
)
{
	UINT32 Data32, MmcClk;

	MmcClk = GetClkFreq(CLKMGR_ALTERA_SDMMCCTR,
		CLKMGR_MAINPLL_PLLC3, CLKMGR_PERPLL_PLLC3);

	Data32 = MmioRead32(CLKMGR_ALTERA + CLKMGR_ALTERA_SDMMCCTR);
	Data32 = (Data32 & 0x7ff) + 1;
	MmcClk = (MmcClk / Data32) / 4;

	return MmcClk;
}
