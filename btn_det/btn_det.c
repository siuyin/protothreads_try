#include "ch32v003fun.h"
#include "pt.h"
#include <stdio.h>

#define SSD1306_128X32
#include "ssd1306_i2c.h"
#include "ssd1306.h"

#include "proto_delay.h"

#define BTN PD2
#define LED PD0

#include "btn_det.h"

void write_str(char* s) {
	ssd1306_setbuf(0);
	ssd1306_drawstr(0,0, s, 1);
	ssd1306_refresh();
}

volatile uint32_t sca_start;
static int single_click_action(struct pt *pt){
	PT_BEGIN(pt);
	write_str("Single Clicked");

	funDigitalWrite(LED,FUN_HIGH);
	DELAY(sca_start, 50);
	funDigitalWrite(LED,FUN_LOW);

	PT_END(pt);
}

volatile uint32_t dca_start;
static int double_click_action(struct pt *pt) {
	PT_BEGIN(pt);
	write_str("Double Clicked");

	funDigitalWrite(LED,FUN_HIGH);
	DELAY(dca_start,125);
	funDigitalWrite(LED,FUN_LOW);
	DELAY(dca_start,125);
	funDigitalWrite(LED,FUN_HIGH);
	DELAY(dca_start,125);
	funDigitalWrite(LED,FUN_LOW);

	PT_END(pt);
}

volatile uint32_t lca_start;
static int long_click_action(struct pt *pt) {
	PT_BEGIN(pt);
		write_str("Long Clicked");
		funDigitalWrite(LED,FUN_HIGH);
		DELAY(lca_start, 500);
		funDigitalWrite(LED,FUN_LOW);
	PT_END(pt);
}


void init_i2c() {
 	if(ssd1306_i2c_init()!=0) {
 		while(1){}
 	}
 
 	ssd1306_init();
}


static struct pt pt_bt;
int main() {
	SystemInit();
	init_i2c();
	PT_INIT(&pt_bt);

	// Enable GPIO D
	funGpioInitD();
	funPinMode(BTN, GPIO_CNF_IN_PUPD);	
	funDigitalWrite(BTN, FUN_HIGH ); // pull-up
	funPinMode(LED, GPIO_Speed_10MHz|GPIO_CNF_OUT_PP);	
	
	write_str("Btn Clk Det");

	while(1) {
		btn_det(&pt_bt);
	}
}
