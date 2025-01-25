#include "ch32v003fun.h"
#include "pt.h"
#include <stdio.h>

// use defines to make more meaningful names for our GPIO pins
#define LED PD0

#include "proto_delay.h"

volatile static uint32_t bl_start_tick;
static uint8_t pr_start;
static int blink_led(struct pt *pt) {
	PT_BEGIN(pt);
	while(1) {
		PT_WAIT_UNTIL(pt,pr_start == 0);

		funDigitalWrite(LED, FUN_HIGH);
		DELAY(bl_start_tick, 50);
		funDigitalWrite(LED, FUN_LOW);
		DELAY(bl_start_tick, 950);

		pr_start=1;
	}
	PT_END(pt);
}

volatile uint32_t pr_start_tick;
static int prnt(struct pt *pt) {
static uint8_t n = 0;
	PT_BEGIN(pt);
	while(1) {
		PT_WAIT_UNTIL(pt, pr_start == 1); 

		printf("%d\n",n++);

		pr_start = 0;
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
	
	funPinMode( LED,     GPIO_Speed_10MHz | GPIO_CNF_OUT_PP );

	while(1)
	{
		blink_led(&pt_bl);
		prnt(&pt_pr);
	}
}
