#include "rand.h"


uint32_t genRIN(void)
{/// generate random integer number [0,2^32) with uniform distribution 
	uint32_t r_val;
	RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN; 									// enable clock and bus for RNG
	
	RNG->CR |= RNG_CR_RNGEN;													// start the RNG

	while(1)
	{
		if(RNG->SR & RNG_SR_SEIS)
		{// error occured
			RNG->SR &= ~RNG_SR_SEIS;									// clear the Sequence Error Flag
			RNG->CR &= ~RNG_CR_RNGEN;									// stop the RNG
			RNG->CR |= RNG_CR_RNGEN;									// start the RNG
		}
		else
		{
			if(RNG->SR & RNG_SR_CEIS)
			{
				RNG->SR &= ~RNG_SR_CEIS;								// clear the Clock Error Flag
				RNG->CR &= ~RNG_CR_RNGEN;								// stop the RNG
				RNG->CR |= RNG_CR_RNGEN;
			}
			else
			{
				if(RNG->SR & RNG_SR_DRDY)
				{
					r_val = RNG->DR;									// get random integer
					break;
				}
			}
		}
	}
	
	RNG->CR &= ~RNG_CR_RNGEN;											// stop the RNG
	RCC->AHB2ENR &= ~RCC_AHB2ENR_RNGEN; 								// disable clock and bus for RNG
	
	return r_val;
}


uint32_t genRBN(float p)
{/// generate random binary number 0 or 1 with probability p(1) = p 
												
	uint32_t rin = genRIN();											// gen. uniform random number
	uint32_t rbn = (uint32_t)(p*(MAX_UINT32_RAND));						// scale the threshold
	uint32_t r_val = 0;
	
	if(rin < rbn)
	{
		r_val = 1;
	}
	
	return r_val;
}


float genURFN(void)
{/// generate random float number between [0,1] with uniform distribution
	float rin = (float)genRIN();
	float max = (float)(MAX_UINT32_RAND);
	volatile float r_val = rin/max;
	return r_val;
}


float genGRFN(float m, float sigma)
{/// Box - Muller transform
	float r1,r2;
	float r;
	
	r1 = genURFN();
	r2 = genURFN();
	
	r = m + sigma* sqrtf(-2*logf(r1))*sinf(r2);
	
}
