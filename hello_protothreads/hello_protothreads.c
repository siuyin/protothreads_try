#include "ch32v003fun.h"
#include "pt.h"
#include <stdio.h>

// use defines to make more meaningful names for our GPIO pins
#define PIN_1 PD0


#define INTVL_MS 125
#define DELAY(dly) {\
	bl_start_tick = SysTick->CNT;\
	PT_WAIT_UNTIL(pt, SysTick->CNT - bl_start_tick >= Ticks_from_Ms(dly));\
}

static int blink_led(struct pt *pt) {
	volatile static uint32_t bl_start_tick;
	//static uint32_t bl_ticks = Ticks_from_Ms(INTVL_MS);
	PT_BEGIN(pt);
	while(1) {
		// bl_start_tick = SysTick->CNT;
		funDigitalWrite(PIN_1, FUN_HIGH);
		// PT_WAIT_UNTIL(pt, SysTick->CNT - bl_start_tick >= bl_ticks);
		DELAY(INTVL_MS);
		funDigitalWrite(PIN_1, FUN_LOW);
		//bl_start_tick = SysTick->CNT;
		//PT_WAIT_UNTIL(pt, SysTick->CNT - bl_start_tick >= bl_ticks);
		DELAY(INTVL_MS);
	}
	PT_END(pt);
}

volatile uint32_t pr_start_tick;
static uint32_t pr_intvl_ticks = Ticks_from_Ms(1000);

static int prnt(struct pt *pt) {
	PT_BEGIN(pt);
	while(1) {
		pr_start_tick = SysTick->CNT;
		printf(".");
		PT_WAIT_UNTIL(pt, SysTick->CNT - pr_start_tick >= pr_intvl_ticks);
	}
	PT_END(pt);
}

static struct pt pt_bl, pt_pr;

int main()
{
	SystemInit();
	PT_INIT(&pt_bl);
	PT_INIT(&pt_pr);

	// Enable GPIOs
	funGpioInitAll();
	
	funPinMode( PIN_1,     GPIO_Speed_10MHz | GPIO_CNF_OUT_PP );

#define DELAY_TIME 250
	while(1)
	{
		blink_led(&pt_bl);
		prnt(&pt_pr);
		// funDigitalWrite( PIN_1,     FUN_HIGH );
		// Delay_Ms( 250 );
		// funDigitalWrite( PIN_1,     FUN_LOW );
		// Delay_Ms( 250 );
	}
}
