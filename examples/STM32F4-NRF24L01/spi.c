#include "spi.h"

void initSPI1(uint16_t prescaler)
{/// init SPI2 in master mode
	///wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
	/// SPI2 Pin configuration
	///-----------------------------------------------------------------
	/// PB13 = SCK => PB3
	/// PA14 = MISO => PB4
	/// PA15 = MOSI => PB5
	///wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
	
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; 								//
	GPIOB->MODER &= ~((GPIO_MODER_MODER5)|(GPIO_MODER_MODER4)|(GPIO_MODER_MODER3));
	GPIOB->MODER |= (GPIO_MODER_MODER5_1)|(GPIO_MODER_MODER4_1)|(GPIO_MODER_MODER3_1); 
	GPIOB->AFR[0] |= 0x00555000;										//   
	GPIOB->OSPEEDR |= 0x00002880;										// 
	
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER &= ~(GPIO_MODER_MODER15);
	GPIOA->MODER |= (GPIO_MODER_MODER15_0); 
	//GPIOA->OTYPER &= ~(0x03000000);
	GPIOA->OSPEEDR |= 0x80000000;										// 
	SPI1_CS_HIGH;
	
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; 								// 	
	SPI1->CR1 = (SPI_CR1_MSTR);											// enable 8 bit data & master mode			
	SPI1->CR1 |= (SPI_CR1_SSI)|(SPI_CR1_SSM);
	
	SPI1->CR1 |= prescaler;		
	SPI1->CR1 |= (SPI_CR1_SPE);											// 
}

uint8_t rxByteSPI1(void)
{///  receive data using SPI2
	uint8_t data;
	
	SPI1->DR = 0x00;						 							// 
	while(!(SPI1->SR & SPI_I2S_FLAG_TXE)); 								// 
	while(!(SPI1->SR & SPI_I2S_FLAG_RXNE)); 							// 
	while(SPI1->SR & SPI_I2S_FLAG_BSY); 								//	
	data = SPI1->DR; 													//
	
	return data;
}	

uint8_t txByteSPI1(uint8_t data)
{/// send data using SPI2
	uint8_t tmp;

	SPI1->DR = data; 													//
	while(!(SPI1->SR & SPI_I2S_FLAG_TXE));								// 
	while(!(SPI1->SR & SPI_I2S_FLAG_RXNE)); 							// 
	while(SPI1->SR & SPI_I2S_FLAG_BSY); 								// 
	SPI1->DR; 													// 
		
	return tmp;
}

void txSPI1(uint8_t * data, uint16_t size)
{
	uint16_t k;
	for(k=0;k<size;k++)
	{
		txByteSPI1(data[k]);
	}
}

void rxSPI1(uint8_t * data, uint16_t size)
{
	uint16_t k;
	for(k=0;k<size;k++)
	{
		data[k] = rxByteSPI1();
	}
}
