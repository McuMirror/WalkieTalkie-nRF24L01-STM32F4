#ifndef CLIENT_H
#define CLIENT_H

#include "nrf24l01.h"

#define RESERVE				97
#define CONNECT				98
#define FREE_CHANNEL		99
#define CONFIRM				100
#define CALL				101
#define HANG_UP				102

char ADDR_SERV[6]; 											
char ADDR_BUS[6];
char MyAddr[6];

uint8_t AntenaState;
uint8_t RxData[NRF24L01_PIPE_LENGTH];
uint8_t TxData[NRF24L01_PIPE_LENGTH];

uint8_t RxCnt;
uint8_t TxCnt;

uint8_t code;

void appendRx(char tmp);
void clearRx();
void appendTx(char tmp);
void clearTx();

#endif 
