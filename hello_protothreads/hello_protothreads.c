#include "ch32v003fun.h"
#include "pt.h"
#include <stdio.h>

#include "proto_delay.h"

static struct pt pt_ti, pt_to;

static uint8_t n = 0;
volatile uint32_t tick_start;
static int tick(struct pt *pt) {
	PT_BEGIN(pt);
	printf("\ttick: %d\n",n++);
	DELAY(tick_start,50);
	PT_END(pt);
}

volatile uint32_t tock_start;
static int tock(struct pt *pt) {
	PT_BEGIN(pt);
	printf("tock\n");
	DELAY(tock_start,950);
	PT_END(pt);
}

#define LED PD0
static uint8_t pr_start;
static int blink_led(struct pt *pt) {
	PT_BEGIN(pt);
	while(1) {
		PT_WAIT_UNTIL(pt,pr_start == 0);
		funDigitalWrite(LED, FUN_HIGH);
		PT_SPAWN(pt, &pt_ti, tick(&pt_ti));

		funDigitalWrite(LED, FUN_LOW);
		PT_SPAWN(pt, &pt_to, tock(&pt_to));

		pr_start=1;
	}
	PT_END(pt);
}

volatile uint32_t pr_start_tick;
static int prnt(struct pt *pt) {
	PT_BEGIN(pt);
	while(1) {
		PT_WAIT_UNTIL(pt, pr_start == 1); 

		printf("\n---");

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
