#include "stm32f4xx.h"
#include "client.h"

char ADDR_SERV[6] = "CHSRV"; 											
char ADDR_BUS[6] = "CHBUS";			
char MyAddr[6];

uint8_t AntenaState;
uint8_t RxData[NRF24L01_PIPE_LENGTH];
uint8_t TxData[NRF24L01_PIPE_LENGTH];

uint8_t RxCnt = 0;
uint8_t TxCnt = 0;

void appendRx(char tmp){
	RxData[RxCnt] = tmp;
	RxCnt++;
	
	if(RxCnt >= NRF24L01_PIPE_LENGTH){
		RxCnt = 0;
	}
}

void clearRx(){
	uint8_t i;
	
	for(i=0;i<NRF24L01_PIPE_LENGTH;i++){
		RxData[i] = 0;
	}
	
	RxCnt = 0;
}

void appendTx(char tmp){
	TxData[TxCnt] = tmp;
	TxCnt++;
	
	if(TxCnt >= NRF24L01_PIPE_LENGTH){
		TxCnt = 0;
	}
}

void clearTx(){
	uint8_t i;
	
	for(i=0;i<NRF24L01_PIPE_LENGTH;i++){
		TxData[i] = 0;
	}
	
	TxCnt = 0;
}
