#include "mp45dt02.h"

void initMIC(uint32_t sample_rate)
{///
	uint32_t utmp32, i2sodd, i2sdiv; 
	///wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
	/// SPI2 Pin configuration - Master mode
	///-----------------------------------------------------------------
	/// PB10 = CLK
	/// PC3  = MISO
	///wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
	
   
	RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOBEN); 								// enable GPIOB Peripherial clock  
	RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOCEN); 								// enable GPIOC Peripherial clock
	
	GPIOB->MODER |= 0x00200000; 										// enable alternate function on pin PB10
	GPIOC->MODER |= 0x00000080; 										// enable alternate function on pin PC3
	
	GPIOB->AFR[1] |= 0x00000500;										// set alternate function SPI2 -> SCK
	GPIOC->AFR[0] |= 0x00005000;										// set alternate function SPI2 -> MISO
	
	GPIOB->OSPEEDR |= 0x00200000;
	GPIOC->OSPEEDR |= 0x00000080;
	
	RCC->APB1ENR |= (RCC_APB1ENR_SPI2EN); 								// enable SPI2 Peripherial clock 
	RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;	
	
	// I2SxCLK = ((HSE)/(PLLM))*(I2S_PLLN)/(I2S_PLLR) = 86 MHz
	// HSE 			- 8MHz
	// PLLM			- 8
	// I2S_PLLN		- 3
	// I2S_PLLR		- 258
	
	utmp32 = (86000000*10)/((sample_rate)*256);
	utmp32 = (utmp32 + 5)/10;											// rounding on next integer

	
	if(utmp32 & 0x00000001)
	{
		utmp32--;
		utmp32 = utmp32/2;
		SPI2->I2SPR = (0x0100)|utmp32;
	}
	else
	{
		SPI2->I2SPR = utmp32/2;
	}
	//SPI2->I2SPR |= (SPI_I2SPR_MCKOE)|0x0002;
	SPI2->I2SPR |= (SPI_I2SPR_MCKOE);
	
	
	
	
	SPI2->I2SCFGR = (SPI_I2SCFGR_I2SMOD)| \
				(SPI_I2SCFGR_I2SCFG)|(SPI_I2SCFGR_I2SSTD_1)| \
				(SPI_I2SCFGR_CKPOL);									// select I2S mode, enable I2S, master receiver
																		// LSB justified, CPOL -> HIGH
}
