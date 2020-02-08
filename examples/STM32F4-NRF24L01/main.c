#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "mp45dt02.h"
#include "cs43l22.h"
#include "pdm_filter.h"
#include "nrf24l01.h"
#include "control.h"
#include "led.h"
#include "client.h"

#define DAC_BUFF_SIZE 128

void pujdo(void);
void feedPujdo(void);
void startPujdo();
void stopPujdo();

PDMFilter_InitStruct Filter;

void runMasterNodeSYS(PDMFilter_InitStruct *);
void runSlaveNodeSYS(void);
void runMasterNodeSYS1(uint8_t nrf_data);

PDMFilter_InitStruct Filter;
uint32_t utmp32 = 0, k_mic = 0, n_mic = 0;
uint16_t buff[64], cnt = 0;

uint16_t outdata[16]; 
uint16_t volume = 20, utmp16;
uint16_t dac_data[DAC_BUFF_SIZE], dac_data2[DAC_BUFF_SIZE];
uint16_t fs = 16000;
uint32_t pid = 0;

int main(void)
{
	Filter.LP_HZ = 8000;
	Filter.HP_HZ = 20;
	Filter.Fs = fs;
	Filter.Out_MicChannels = 1;
	Filter.In_MicChannels = 1;
	PDM_Filter_Init((PDMFilter_InitStruct *)&Filter);  
	
	initMIC(fs);
	 
	RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;
	
	initUSART2(USART2_BAUDRATE_921600);
	
	printUSART2("\nwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww\n");
	printUSART2("w STM32F407 - CS43L22 Audio DAC MIC I2S demo");
	printUSART2("\nwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww\n");
	
	initCS43L22(3, 8000, dac_data, dac_data2, DAC_BUFF_SIZE);
	printUSART2("-> SYS: init completed\n");
	SPI2->I2SCFGR |= SPI_I2SCFGR_I2SE; 
		
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	GPIOC->MODER &= ~(GPIO_MODER_MODER6);  
	GPIOC->PUPDR |= (GPIO_PUPDR_PUPDR6_0);		

	delay_ms(10);
	 
	printUSART2("\n\nwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww\n");
	printUSART2("w nRF24L01 Tx-Rx - Client\n");
	printUSART2("\nwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww\n");
	
	initSYSTIM();
	initNRF24L01(ADDR_BUS);
	
	uint8_t i = 0;
	pujdo();
	
	while(1){
		
	if(state == BOOT){
		ledTurnOn("red");
		appendTx(RESERVE);
		txDataNRF24L01((uint8_t *)ADDR_SERV, TxData);				
		clearTx();
		setRxMode();
		
		while(1)
		{
			setTxAddrNRF24L01(ADDR_SERV);
			AntenaState = dataReadyNRF24L01();
							
			if(AntenaState == (NRF_DATA_READY))
			{
				rxDataNRF24L01(RxData);
				code = RxData[0];
				printUSART2("CODE: %d \n", code);
				state = ADDRESS;
				break;
			} 
		}	
	} else if(state == ADDRESS){
		ledTurnOn("green");
		
		printUSART2("Trying to connect!\n");
		
		clearTx();
		appendTx(code);
		appendTx(CONNECT);
		txDataNRF24L01((uint8_t *)ADDR_SERV, TxData);				
		clearTx();
		setRxMode();
		
		printUSART2("Trying to recieve addres from server!\n");
		
		while(1)
		{
			setTxAddrNRF24L01(ADDR_SERV);
			AntenaState = dataReadyNRF24L01();
							
			if(AntenaState == (NRF_DATA_READY))
			{
								
				rxDataNRF24L01(RxData);
				
				for(i=0;i<5;i++) {	
					MyAddr[i] = RxData[i];
				}
				
				printUSART2("My address from server: %s", MyAddr);
				/*delay_ms(2000);	
				
				appendTx(code);
				appendTx(FREE_CHANNEL);
				txDataNRF24L01((uint8_t *)ADDR_SERV, TxData);				
				clearTx();*/
				
				state = CHOOSE_OPTION;
				break;
			} 
		}	
	} else if(state == CHOOSE_OPTION){
		/*while(1){
			clearTx();
			appendTx(code);
			appendTx(HANG_UP);
			txDataNRF24L01((uint8_t *)ADDR_SERV, TxData);				
			clearTx();
			
			printUSART2("KEEPING ALIVE!\n");
			
			delay_ms(1000);
		}*/
		
		startPujdo();
								
		while(1){
			printUSART2("Enter address: ");
			uint8_t addr = getcharUSART2();
			clearTx();
			appendTx(code);
			appendTx(CALL);
			appendTx(addr);
			appendTx(MyAddr[0]);
			txDataNRF24L01((uint8_t *)ADDR_SERV, TxData);				
			clearTx();
			stopPujdo();
		}
	} else {
	
	}
	
	}
	
	return 0;
}

void pujdo(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM5EN; 								// 
	TIM5->PSC = 0x20D0-0x0001;											// 
																		// 
	TIM5->ARR = 0x0FFF;													// 
	TIM5->CR1 = 0x0084;													// 
																		//
	TIM5->CR2 = 0x0000;
	TIM5->CNT = 0x0000;													// 
	TIM5->EGR |= TIM_EGR_UG;											//
	TIM5->DIER = 0x0001;												// enable 
	
	NVIC_SetPriority(TIM5_IRQn, 0);
	NVIC_EnableIRQ(TIM5_IRQn);											// 	
}

void startPujdo(){
	TIM5->CR1 |= TIM_CR1_CEN;
} 

void stopPujdo(){
	TIM5->CR1 &= ~TIM_CR1_CEN;
	TIM5->SR = 0x0000;
}

void feedPujdo(void){
	TIM5->CNT = 0x0000;
}

void TIM5_IRQHandler(void)
{
	if(TIM5->SR & 0x0001)
	{
		TIM5->SR = 0x0000;
		
		clearTx();
		appendTx(code);
		appendTx(KEEP_ALIVE);
		txDataNRF24L01((uint8_t *)ADDR_SERV, TxData);				
		clearTx();
	}
}
