#include "ch32v003fun.h"
#include "pt.h"
#include <stdio.h>

#define BTN PD2
#define LED PD0

#define initPortD() { \
	funGpioInitD(); \
}

#define initBTN() { \
	funPinMode(BTN, GPIO_CNF_IN_PUPD); \
	funDigitalWrite(BTN, FUN_HIGH ); \
}

#define initLED() { \
	funPinMode(LED, GPIO_Speed_10MHz|GPIO_CNF_OUT_PP); \
}	

#define enableTIM1() ( RCC->APB2PCENR |=RCC_APB2Periph_GPIOD | RCC_APB2Periph_TIM1 )
#define configTIM1() ( funPinMode(LED, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF) )
#define resetTIM1() { \
	RCC->APB2PRSTR |= RCC_APB2Periph_TIM1; \
	RCC->APB2PRSTR &= ~RCC_APB2Periph_TIM1; \
}
#define setTIM1PrescalePeriod() { \
	TIM1->PSC = 47999; \
	TIM1->ATRLR = 999; \
	TIM1->SWEVGR |= TIM_UG; \
}
#define enableTIM1Output() { \
	TIM1->CCER |= TIM_CC1NE;  \
	TIM1->CCER &=  ~TIM_CC1NP; \
	TIM1->CHCTLR1 |= TIM_OC1M_2 | TIM_OC1M_1; \
	TIM1->BDTR |= TIM_MOE; \
	TIM1->CTLR1 |= TIM_CEN; \
}
#define initTIM1() { \
	enableTIM1(); \
	configTIM1(); \
	resetTIM1(); \
	setTIM1PrescalePeriod(); \
	enableTIM1Output(); \
}

#define pulseWidth(n) ( TIM1->CH1CVR = n ) 

int main() {
	SystemInit();
	initPortD();
	initBTN();
	initLED();
	initTIM1();
	pulseWidth(49);

	while(1) {
	}
}
