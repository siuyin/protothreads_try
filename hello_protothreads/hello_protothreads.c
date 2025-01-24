#include "ch32v003fun.h"
#include "pt.h"
#include <stdio.h>

// use defines to make more meaningful names for our GPIO pins
#define PIN_1 PD0

volatile uint32_t start_tick;
static uint32_t on_ticks = Ticks_from_Ms(250);
static uint32_t off_ticks = Ticks_from_Ms(250);

static int protothread1(struct pt *pt) {
	PT_BEGIN(pt);
	while(1) {
		start_tick = SysTick->CNT;
		funDigitalWrite(PIN_1, FUN_HIGH);
		PT_WAIT_UNTIL(pt, SysTick->CNT - start_tick >= on_ticks);
		funDigitalWrite(PIN_1, FUN_LOW);
		start_tick = SysTick->CNT;
		PT_WAIT_UNTIL(pt, SysTick->CNT - start_tick >= off_ticks);
	}
	PT_END(pt);
}

static struct pt pt1;

int main()
{
	SystemInit();
	PT_INIT(&pt1);

	// Enable GPIOs
	funGpioInitAll();
	
	funPinMode( PIN_1,     GPIO_Speed_10MHz | GPIO_CNF_OUT_PP );

	while(1)
	{
		protothread1(&pt1);
		// funDigitalWrite( PIN_1,     FUN_HIGH );
		// Delay_Ms( 250 );
		// funDigitalWrite( PIN_1,     FUN_LOW );
		// Delay_Ms( 250 );
	}
}
