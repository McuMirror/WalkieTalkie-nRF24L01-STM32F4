#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "nrf24l01.h"
#include "server.h"
#include "control_server.h"
#include "led.h"
#include "rand.h"

void pujdo(void);
void feedPujdo(void);
void startPujdo();
void stopPujdo();

uint32_t tmp;
uint8_t code;
uint8_t reserved;

int main(void)
{	
	//uint8_t cnt_addr = 0;
	//uint8_t k, i, res;
	//uint8_t nrf_data[32];
	//uint8_t bus_flag = 0;
	//uint8_t z;
	//uint8_t commands[2];	
	
	initBlink();
	initUSART2(USART2_BAUDRATE_921600);
	initNRF24L01(ADDR_SRV);
    startBlink("red");
    
    pujdo();
    
    printUSART2("\n\nwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww\n");
	printUSART2("w nRF24L01 Tx-Rx - Server\n");
	printUSART2("\nwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww\n");
      
    tmp = genRIN();
	code = tmp>>24;
	reserved = 0;
      
	while(1)															// vrti sve dok se ne unese odgovarajuća antena(0-5)
	{
		setTxAddrNRF24L01(ADDR_BUS);									// adresa sa koje sluša
		AntenaState = dataReadyNRF24L01();
				
		if(AntenaState == (NRF_DATA_READY))
		{
			rxDataNRF24L01(RxData);								    // primljeni podatak je upisan u nrf_data
			commands[0] = RxData[0];	
			commands[1] = RxData[1];
			commands[2] = RxData[2];
			
			printUSART2("%d - %d - %d\n", commands[0], commands[1], commands[2]);
			
			if(commands[0] == RESERVE){
				if(!reserved){
					reserved = 1;
					printUSART2("CODE: %d\n", code);
					appendTx(code);
					txDataNRF24L01((uint8_t*)ADDR_BUS, TxData);
					clearTx();
					clearRx();
					startPujdo();
				}
			} else if(commands[1] == (CONNECT)) {//connect
				printUSART2("Trying connect with code %d\n", commands[0]);
				printUSART2("Server code is: %d", code);
				
				if(code == commands[0]){
					feedPujdo();
					
					if(cnt_addr < 5) {
					for(i=0;i<5;i++) {
						appendTx((uint8_t)(USED_ADDR[cnt_addr][i]));
					}	
					delay_ms(1000);	  
					txDataNRF24L01((uint8_t*)ADDR_BUS, TxData);
					delay_ms(500);
					bus_flag = 0;
					clearTx();
					cnt_addr++;
					commands[0] = (CALL);
					}
					else {
						appendTx('e');
						appendTx('r');
						appendTx('r');
						appendTx('o');
						appendTx('r');
						txDataNRF24L01((uint8_t*)ADDR_BUS, TxData);
						clearTx();
						commands[0] = (NO_MORE_ADDRESS);
					}
					
					cnt_addr++;
					commands[1] = (CALL);
				}
			}
			else if(commands[1] == (FREE_CHANNEL)) {//free_chanel
				if(code == commands[0]){
					bus_flag = 0;
					printUSART2("Chanel BUS - SERVER is free!");
					clearRx();
				}
			}
			else if(commands[1] == (HANG_UP)) {//hangup
				if(code == commands[0]){
					feedPujdo();
					printUSART2("Keeping alive! \n");
				}
			}
			else {
				
			} 	
		  }
		}
	return 0;
}

void pujdo(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN; 								// 
	TIM7->PSC = 0x20D0-0x0001;											// 
																		// 
	TIM7->ARR = 0xFFFF;													// 
	TIM7->CR1 = 0x0084;													// 
																		//
	TIM7->CR2 = 0x0000;
	TIM7->CNT = 0x0000;													// 
	TIM7->EGR |= TIM_EGR_UG;											//
	TIM7->DIER = 0x0001;												// enable 
	
	NVIC_SetPriority(TIM7_IRQn, 0);
	NVIC_EnableIRQ(TIM7_IRQn);											// 	
}

void startPujdo(){
	TIM7->CR1 |= TIM_CR1_CEN;
} 

void stopPujdo(){
	TIM7->CR1 &= ~TIM_CR1_CEN;
}

void feedPujdo(void){
	TIM7->CNT = 0x0000;
}

void TIM7_IRQHandler(void)
{
	if(TIM7->SR & 0x0001)
	{
		TIM7->CR1 &= ~TIM_CR1_CEN;
		TIM7->SR = 0x0000;
		reserved = 0;
		tmp = genRIN();
		code = tmp>>24;
		printUSART2("DEAD!\n");
	}
}
