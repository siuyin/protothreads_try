#include "ch32v003fun.h"
#include "pt.h"
#include <stdio.h>

#define BTN PD2
#define LED PD0

#include "proto_delay.h"
#include "btn_det.h"

#define pulseWidth(n) ( TIM1->CH1CVR = n ) 
#define currentWidth() ( TIM1->CH1CVR ) 
#define decrPWM() ( TIM1->CH1CVR = (TIM1->CH1CVR >> 1) );
#define incrPWM() ( TIM1->CH1CVR = (TIM1->CH1CVR << 1) );

int8_t dim_mode = 0; // 0 = off, 1 = on incr, 2 = on decr
int8_t dim_curr_lvl = 0;
void dimLvl(uint8_t n) {
	switch(n) {
		case 0: pulseWidth(0);
			break;
		case 1: pulseWidth(1);
			break;
		case 2: pulseWidth(3);
			break;
		case 3: pulseWidth(7);
			break;
		case 4: pulseWidth(15);
			break;
		case 5: pulseWidth(31);
			break;
		case 6: pulseWidth(63);
			break;
		case 7: pulseWidth(127);
			break;
		case 8: pulseWidth(255);
			break;
	}
}
//volatile uint32_t sca_start;
static int single_click_action(struct pt *pt){
	PT_BEGIN(pt);
	if (dim_curr_lvl >= 8) {
		dim_mode = 2;
		dimLvl(--dim_curr_lvl);
	} else if (dim_curr_lvl == 0) {
		dim_mode = 1;
		dimLvl(++dim_curr_lvl);
	} else if (dim_mode ==1) {
		dimLvl(++dim_curr_lvl);
	} else if (dim_mode == 2) {
		dimLvl(--dim_curr_lvl);
	}
	PT_END(pt);
}

//volatile uint32_t dca_start;
static int double_click_action(struct pt *pt) {
	PT_BEGIN(pt);
	if (dim_mode == 1) dim_mode = 2;
	else if (dim_mode == 2) dim_mode = 1;
	PT_END(pt);
}

static int long_click_action(struct pt *pt) {
	PT_BEGIN(pt);
	if (dim_mode == 0) {
		dim_mode = 1;
		dimLvl(dim_curr_lvl);
	} else { 
		dim_mode = 0;
		dimLvl(0);
	}
	PT_END(pt)
}

#define initBTN() { \
	funGpioInitD(); \
	funPinMode(BTN, GPIO_CNF_IN_PUPD); \
	funDigitalWrite(BTN, FUN_HIGH ); \
}

#define enableTIM1() ( RCC->APB2PCENR |=RCC_APB2Periph_GPIOD | RCC_APB2Periph_TIM1 )
#define configTIM1() ( funPinMode(LED, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF) )
#define resetTIM1() { \
	RCC->APB2PRSTR |= RCC_APB2Periph_TIM1; \
	RCC->APB2PRSTR &= ~RCC_APB2Periph_TIM1; \
}
#define setTIM1PrescalePeriod() { \
	TIM1->PSC = 0x0000; \
	TIM1->ATRLR = 255; \
	TIM1->SWEVGR |= TIM_UG; \
}
#define enableTIM1Output() { \
	TIM1->CCER |= TIM_CC1NE;  \
	TIM1->CCER &=  ~TIM_CC1NP; \
	TIM1->CHCTLR1 |= TIM_OC1M_2 | TIM_OC1M_1; \
	TIM1->CH1CVR = 127; \
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
static struct pt pt_bt;
int main() {
	SystemInit();
	PT_INIT(&pt_bt);

	initBTN();
	initTIM1();	
	printf("Entering main loop\n");

	while(1) {
		//next_check_tick = SysTick->CNT;
		btn_det(&pt_bt);
	}
}
