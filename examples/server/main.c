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

int main(void){	
	initBlink();
	initUSART2(USART2_BAUDRATE_921600);
	initNRF24L01(ADDR_SRV);
    startBlink("red");
    
    pujdo();
    
    printUSART2("\n\nwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww\n");
	printUSART2("w nRF24L01 Tx-Rx - Server\n");
	printUSART2("\nwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww\n");

	while(1)															// vrti sve dok se ne unese odgovarajuća antena(0-5)
	{
		setTxAddrNRF24L01(ADDR_BUS);									// adresa sa koje sluša
		AntenaState = dataReadyNRF24L01();
				
		if(AntenaState == (NRF_DATA_READY))
		{
			rxDataNRF24L01(RxData);								    // primljeni podatak je upisan u nrf_data
						
			if(getCommand(RxData) == RESERVE){
				if(!reserved){
					startPujdo();
					
					reserved = 1;
					setServerCode();
					
					if(setClientCode(getClientCode(RxData))){
						printUSART2("Code from client %d appended to server and set as active.\n", getClientCode(RxData));
						printUSART2("Server code is: %d\n", code);
						
						packetDataLight(code, client_code, RESERVED);
						txDataNRF24L01((uint8_t*)ADDR_BUS, TxData);
					} else {
						packetDataLight(code, client_code, CLIENT_CODE_USED);
						txDataNRF24L01((uint8_t*)ADDR_BUS, TxData);
					}
					
					stopPujdo();
				} else {
					printUSART2("Someone is trying to connect to reserved channel. \n");
				}
			} else if(getCommand(RxData) == FREE_CHANNEL && getServerCode(RxData) == code){
				startPujdo();
				reserved = 0;
				setServerCode();
				
				uint8_t temp_client_code = client_code;
				
				if(clearClientCode(client_code)){
					printUSART2("Code from client %d removed from server.\n", getClientCode(RxData));
					printUSART2("Server code is: %d\n", code);
					
					packetDataLight(code, temp_client_code, FREED);
					txDataNRF24L01((uint8_t*)ADDR_BUS, TxData);
				} else {
					packetDataLight(code, temp_client_code, NOT_FREED);
					txDataNRF24L01((uint8_t*)ADDR_BUS, TxData);
				}
				stopPujdo();
			} else if(getCommand(RxData) == ADDRESS && getServerCode(RxData) == code){
				startPujdo();
				printUSART2("Code from client %d got address from server.\n", client_code);
									
				packetData(code, client_code, FETCHED_ADDRES, fetchFreeAddress(getClientCode(RxData)));
				txDataNRF24L01((uint8_t*)ADDR_BUS, TxData);
				stopPujdo();
			} else if(getCommand(RxData) == CHECK_CALLS && getServerCode(RxData) == code){
				startPujdo();
				printUSART2("Code from client %d got address from server.\n", client_code);
				
				if(getPtrAddr(client_code)){
					if(ptrAddr->attemptedCall == 1){
						ptrAddr->busy = 1;
						packetData(code, client_code, HAVE_CALL, ptrAddr->talkingTo);
						txDataNRF24L01((uint8_t*)ADDR_BUS, TxData);
					} else {
						packetDataLight(code, client_code, NO_CALL);
						txDataNRF24L01((uint8_t*)ADDR_BUS, TxData);
					}
				} else {
					packetDataLight(code, client_code, NO_CALL);
					txDataNRF24L01((uint8_t*)ADDR_BUS, TxData);
				}
				
				stopPujdo();
			} else {
				startPujdo();
				printUSART2("Invalid command.\n");
			}
		}
	}
	
	return 0;
}

void pujdo(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM5EN; 								// 
	TIM5->PSC = 0x20D0-0x0001;											// 
																		// 
	TIM5->ARR = 0xFFFF;													// 
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
}

void feedPujdo(void){
	TIM5->CNT = 0x0000;
}

void TIM5_IRQHandler(void)
{
	if(TIM5->SR & 0x0001)
	{
		TIM5->CNT = 0x0000;
		TIM5->SR = 0x0000;
	
		reserved = 0;
		
		printUSART2("Restarting!\n");
	}
}
