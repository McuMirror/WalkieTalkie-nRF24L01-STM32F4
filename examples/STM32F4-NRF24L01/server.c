#include "server.h"

char ADDR_SRV[6] = "CHSRV"; 											// Adresa servera
char ADDR_BUS[6] = "CHBUS";												// Adresa busa											
char CALLS[5][5] = {"0x00", "0x00", "0x00", "0x00", "0x00"};	
char USED_ADDR[5][5] = {"alpha", "charl", "bravo", "delta", "echoo"};		
char ADDR_TX_P0[5] = "alpha";													
char ADDR_TX_P1[5] = "charl";
char ADDR_TX_P2[5] = "bravo";
char ADDR_TX_P3[5] = "delta";
char ADDR_TX_P4[5] = "echoo";

uint8_t i;
uint8_t cnt_addr = 0;
uint8_t bus_flag = 0;
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
