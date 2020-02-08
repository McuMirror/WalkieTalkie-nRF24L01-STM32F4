#include "stm32f4xx.h"
#include "led.h"

uint16_t pwm[2] = {0, 1000};

void initLed(){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;  								// Enable the clock for GPIOD	
}

void ledTurnOn(char *diode){
	GPIOD->MODER &= ~0xFF000000;
	GPIOD->MODER |= 0x55000000;             							// set PORTD pin 12,13,14,15 as digital output
    GPIOD->OTYPER |= 0x00000000;
    
	if(diode == "green"){
		GPIOD->ODR |= GPIO_ODR_ODR_12;
	} else if(diode == "orange"){
		GPIOD->ODR |= GPIO_ODR_ODR_13;
	} else if(diode == "red"){
		GPIOD->ODR |= GPIO_ODR_ODR_14;
	} else if(diode == "blue"){
		GPIOD->ODR |= GPIO_ODR_ODR_15;
	} else {
		GPIOD->ODR |= (GPIO_ODR_ODR_12)|(GPIO_ODR_ODR_13)|(GPIO_ODR_ODR_14)|(GPIO_ODR_ODR_15);
	}
}

void ledTurnOff(char *diode){
	if(diode == "green"){
		GPIOD->ODR &= ~GPIO_ODR_ODR_12;
	} else if(diode == "orange"){
		GPIOD->ODR &= ~GPIO_ODR_ODR_13;
	} else if(diode == "red"){
		GPIOD->ODR &= ~GPIO_ODR_ODR_14;
	} else if(diode == "blue"){
		GPIOD->ODR &= ~GPIO_ODR_ODR_15;
	} else {
		GPIOD->ODR &= (~GPIO_ODR_ODR_12)|(~GPIO_ODR_ODR_13)|(~GPIO_ODR_ODR_14)|(~GPIO_ODR_ODR_15);
	}
}

void initBlink(){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;  								
    
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;								
				
	TIM4->PSC = 0x0054 - 0x0001;									
	TIM4->ARR = 0x03E8; 											
		
	TIM4->CCR1 = 0x0000;
	
	TIM4->CCMR1 |= (TIM_CCMR1_OC1PE)|(TIM_CCMR1_OC1M_2)|(TIM_CCMR1_OC1M_1);
	TIM4->CCMR1 |= (TIM_CCMR1_OC2PE)|(TIM_CCMR1_OC2M_2)|(TIM_CCMR1_OC2M_1);	
	TIM4->CCMR2 |= (TIM_CCMR2_OC3PE)|(TIM_CCMR2_OC3M_2)|(TIM_CCMR2_OC3M_1);	
	TIM4->CCMR2 |= (TIM_CCMR2_OC4PE)|(TIM_CCMR2_OC4M_2)|(TIM_CCMR2_OC4M_1);	
	
	TIM4->CCER &= ~((TIM_CCER_CC1P)|(TIM_CCER_CC2P)|(TIM_CCER_CC3P)|(TIM_CCER_CC4P));
	TIM4->CR1 |= (TIM_CR1_ARPE)|(TIM_CR1_URS);
	TIM4->EGR |= TIM_EGR_UG;
	TIM4->CCER |= (TIM_CCER_CC1E)|(TIM_CCER_CC2E)|(TIM_CCER_CC3E)|(TIM_CCER_CC4E);
	//TIM4->CR1 |= TIM_CR1_CEN;
	
	///wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
	/// setup TIM3 for TRGO event
	///wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; 								
	TIM3->PSC = 0x1068-0x0001;											
																		
	TIM3->ARR = 0x14E2;												
	TIM3->CR1 = 0x0084;													
				
	TIM3->CR2 = 0x0000;													
	TIM3->CR2 |= TIM_CR2_MMS_1;											

	TIM3->EGR |= (TIM_EGR_UG);											
	TIM3->DIER |= (TIM_DIER_UDE)|(TIM_DIER_CC1DE);			
	
	//TIM3->CR1 |= TIM_CR1_CEN;											

	///wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
	/// Setup DMA1 controller
	///wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
	
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;									
	
	DMA1->LISR = 0x00000000;	
	DMA1->HISR = 0x00000000;	
	
	//DMA1_Stream4->PAR = (uint32_t)&TIM4->CCR1;
	DMA1_Stream4->M0AR = (uint32_t)&pwm[0];							
	DMA1_Stream4->NDTR = 2;											
								
	DMA1_Stream4->CR |= (DMA_SxCR_CHSEL_2)|(DMA_SxCR_CHSEL_0);			
	DMA1_Stream4->CR |= DMA_SxCR_PL;									
																		
																																			
	DMA1_Stream4->CR |= DMA_SxCR_MINC;									
																																			
	DMA1_Stream4->CR |= DMA_SxCR_CIRC;									
	DMA1_Stream4->CR |= DMA_SxCR_PSIZE_0;								
																		
	DMA1_Stream4->CR |= DMA_SxCR_MSIZE_0;								
																		
	DMA1_Stream4->CR |= DMA_SxCR_DIR_0;									
																		
	//DMA1_Stream4->CR |= DMA_SxCR_EN;	
}

void startBlink(char *diode){
	GPIOD->MODER &= ~0xFF000000;	
	GPIOD->MODER |= 0xAA000000;             							
    GPIOD->OTYPER = 0x00000000;											
    
    GPIOD->AFR[1] |= 0x22220000;
    
	if(diode == "green"){
		DMA1_Stream4->PAR = (uint32_t)&TIM4->CCR1;
		//GPIOD->ODR &= ~GPIO_ODR_ODR_12;
	} else if(diode == "orange"){
		DMA1_Stream4->PAR = (uint32_t)&TIM4->CCR2;
	} else if(diode == "red"){
		DMA1_Stream4->PAR = (uint32_t)&TIM4->CCR3;
	} else if(diode == "blue"){
		DMA1_Stream4->PAR = (uint32_t)&TIM4->CCR4;
	} else {
		// Cant turn all diodes with DMA?
	}
	
	TIM4->CR1 |= TIM_CR1_CEN;
	TIM3->CR1 |= TIM_CR1_CEN;	
	DMA1_Stream4->CR |= DMA_SxCR_EN;
}

void stopBlink(char *diode){	
	TIM3->CR1 &= ~TIM_CR1_CEN;	
	DMA1_Stream4->CR &= ~DMA_SxCR_EN;
	
	if(diode == "green"){
		TIM4->CCR1 = 0x0000;
	} else if(diode == "orange"){
		TIM4->CCR2 = 0x0000;
	} else if(diode == "red"){
		TIM4->CCR3 = 0x0000;
	} else if(diode == "blue"){
		TIM4->CCR4 = 0x0000;
	} else {
		// Cant turn all diodes with DMA?
	}
	
	TIM4->CR1 &= ~TIM_CR1_CEN;

}

