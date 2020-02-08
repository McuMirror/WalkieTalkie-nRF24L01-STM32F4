#ifndef __RAND_H
#define __RAND_H
#include "stm32f4xx.h"
#include "delay.h"
#include "arm_math.h"

#define MAX_UINT32_RAND				0xFFFFFFFF

//void initRAND(void);

/** Generate random integer number with uniform distribution */
uint32_t genRIN(void);

/** Generate random binary integer 0 or 1 with distribution defined by p*/
uint32_t genRBN(float p);

/** Generate random float between [0,1] with uniform distribution */
float genURFN(void);

/** Generate random float number with Gauss distribution*/
float genGRFN(float m, float sigma);


#endif 
