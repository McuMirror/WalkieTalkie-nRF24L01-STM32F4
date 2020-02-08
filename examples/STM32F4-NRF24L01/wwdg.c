#include "wwdg.h"

volatile uint8_t g_wwdg_flag = 0;
volatile uint8_t g_wwdg_rst = 0;
__inline uint8_t chk4StartupWWDG(void)
{
	uint8_t wdg_rst = 0;
	if(RCC->CSR&(RCC_CSR_WWDGRSTF))										// check if this is start from WDT reset
	{
		wdg_rst = 1;
		RCC->CSR |= (RCC_CSR_RMVF);
	}
	return wdg_rst;
}

void initWWDG(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_WWDGEN;
	
	
	WWDG->CFR = 0x000003FF;												// enable EWI, set prescaler to 8 and window value to 0x3F
																		// WWDG counter freq 42MHz/8 = 5.25 MHz for APB1 -> 42 MHz
																		// 5.25MHz/4096 = 1281.7 Hz actual clock rate
	WWDG->SR = 0x0000;													// clear EWI statu bit
	
	
	WWDG->CR = 0x0000007F;												// activate WWDG and set EWI IRW period to 0x40 = 12.2 Hz cca 82 ms
	NVIC_EnableIRQ(WWDG_IRQn);
	WWDG->CR |= 0x00000080;
	
}

__inline void feedWWDG(void)
{
	if((WWDG->CR & 0x00000040) == 0)
	{
		g_wwdg_flag = 1;
	}
}

void chkWWDG(void)
{
	if(g_wwdg_flag)
	{
		g_wwdg_flag = 0;
		printUSART2("-> WWDG: stalling\n");
	}
}

__inline void scWWDT(void)
{
	g_wwdg_rst ^= 0x01; 
}

void WWDG_IRQHandler(void)
{
	//NVIC_DisableIRQ(WWDG_IRQn);	
	if(g_wwdg_rst == 0x00)
		WWDG->CR = 0x000000FF;												// disable WWDG
	
	WWDG->SR = 0x00000000;
	
	if(g_wwdg_rst)
		g_wwdg_flag = 1;
}

