#ifndef __WWDG_H
#define __WWDG_H

#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"

uint8_t chk4StartupWWDG(void);
void 	initWWDG(void);
void 	feedWWDG(void);
void 	chkWWDG(void);
void 	scWWDT(void);
#endif 
