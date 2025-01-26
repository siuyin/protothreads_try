#include "ch32v003fun.h"
#include "pt.h"
#include <stdio.h>

#include "proto_delay.h"

#define BTN PD2
#define LED PD0

#define btn_pushed() ( funDigitalRead(BTN) == 0 ? 1 : 0 )

static struct pt pt_bl;
volatile uint32_t bl_start;
static int blink_led(struct pt *pt){
	PT_BEGIN(pt);

	funDigitalWrite(LED,FUN_HIGH);
	DELAY(bl_start, 50);
	funDigitalWrite(LED,FUN_LOW);

	PT_END(pt);
}

static struct pt pt_dbl;
volatile uint32_t dbl_start;
static int double_blink_led(struct pt *pt) {
	PT_BEGIN(pt);

	funDigitalWrite(LED,FUN_HIGH);
	DELAY(dbl_start,125);
	funDigitalWrite(LED,FUN_LOW);
	DELAY(dbl_start,125);
	funDigitalWrite(LED,FUN_HIGH);
	DELAY(dbl_start,125);
	funDigitalWrite(LED,FUN_LOW);

	PT_END(pt);
}

static struct pt pt_lb;
volatile uint32_t lb_start;
static int long_blink_led(struct pt *pt) {
	PT_BEGIN(pt);

	funDigitalWrite(LED,FUN_HIGH);
	DELAY(lb_start, 500);
	funDigitalWrite(LED,FUN_LOW);

	PT_END(pt)
}

typedef enum {
	DCSReleased,
	DCSCandPushed,
	DCSPushed,
	DCSCandLongClicked,
	DCSLongClickedTimer,
	DCSLongClicked,
	DCSCandFirstClicked,
	DCSFirstClicked,
	DCSClicked,
	DCSCandDblClicked,
	DCSDblClicked,
	DCSCandReleased
} DblClkState_Typedef;

const uint32_t long_click_max_interval = Ticks_from_Ms(500);
const uint32_t double_click_max_interval = Ticks_from_Ms(150); // tune this value to properly detect a double-click
volatile uint32_t interval_timer_start_tick;
volatile uint32_t next_check_tick;
const uint32_t check_intvl = Ticks_from_Ms(20);
DblClkState_Typedef dblclick_state = DCSReleased;
// statemachine markdown:
// [![](https://mermaid.ink/img/pako:eNqVVFtvgjAU_ivNeVyEVcDByOaLbk8uWbY9TXyoUIWIYEpZ5oz_faVehrQy9clz-C7nlm4gzCMKPhSccDpMyJyRpfFlBRkSv_HNBBlGH73RlJKCRrvsIZKfBiSLzNeyiEXso_GUZ2glo4kGfAz2SLaP99i6VgVvka2hJGuUZ_NBmoQLBY9ukeiNccSTJWU1ozql0qnFHxVU66tlyuRzwgreKKHRn-LQsK1oskrUR6lIo7DKIx4zWphVIzI5TZNsgUZPwxbRZplH4YdHRVktTleXMgntEA4Lvmy_J5S2m7jq4lTGPxbNii7YpIprZav1KdxWU90N692H0zRsHpPYebRPH27pnIR6h3rmGZLa67kDaZtrrYeKd9qSfpwNygUbvM6kgW5vBTogZrckSSTe1U0lEACP6ZIG4Iu_EZ2RMuUBBNlWQEnJ8_d1FoLPWUk7wPJyHoM_I2khonIV_b3LB8iKZJ95fgxplPCcvezecfmcSwj4G_gGv4tN2_Ww5biO5d7Zdq_bgTX4hm3bJsYW7t1h13Nw19524EeqWqbjuJ6HXce5tx3sWdtf5aP94A?type=png)](https://mermaid.live/edit#pako:eNqVVFtvgjAU_ivNeVyEVcDByOaLbk8uWbY9TXyoUIWIYEpZ5oz_faVehrQy9clz-C7nlm4gzCMKPhSccDpMyJyRpfFlBRkSv_HNBBlGH73RlJKCRrvsIZKfBiSLzNeyiEXso_GUZ2glo4kGfAz2SLaP99i6VgVvka2hJGuUZ_NBmoQLBY9ukeiNccSTJWU1ozql0qnFHxVU66tlyuRzwgreKKHRn-LQsK1oskrUR6lIo7DKIx4zWphVIzI5TZNsgUZPwxbRZplH4YdHRVktTleXMgntEA4Lvmy_J5S2m7jq4lTGPxbNii7YpIprZav1KdxWU90N692H0zRsHpPYebRPH27pnIR6h3rmGZLa67kDaZtrrYeKd9qSfpwNygUbvM6kgW5vBTogZrckSSTe1U0lEACP6ZIG4Iu_EZ2RMuUBBNlWQEnJ8_d1FoLPWUk7wPJyHoM_I2khonIV_b3LB8iKZJ95fgxplPCcvezecfmcSwj4G_gGv4tN2_Ww5biO5d7Zdq_bgTX4hm3bJsYW7t1h13Nw19524EeqWqbjuJ6HXce5tx3sWdtf5aP94A)

static int btn_det(struct pt *pt) {
	PT_BEGIN(pt);
	while (1) {
		if (next_check_tick > SysTick->CNT) {
			continue;
		}
		// FIXME: printf("dblclick state=%d\n",dblclick_state); printf is unreliable here
		if (dblclick_state == DCSReleased) {
			if (btn_pushed()){
				dblclick_state = DCSCandPushed;
			} else {
				dblclick_state = DCSReleased;
			}
		} else if (dblclick_state == DCSCandPushed) {
			if (btn_pushed()){
				dblclick_state = DCSPushed;
			} else {
				dblclick_state = DCSCandReleased;
			}
		} else if (dblclick_state == DCSPushed) {
			if (btn_pushed()){
				dblclick_state = DCSCandLongClicked;
				interval_timer_start_tick = SysTick->CNT;
			} else {
				dblclick_state = DCSCandFirstClicked;
			}
		} else if (dblclick_state == DCSCandLongClicked) {
			if (btn_pushed()){
				dblclick_state = DCSLongClickedTimer;
			} else {
				dblclick_state = DCSCandFirstClicked;
			}
		} else if (dblclick_state == DCSLongClickedTimer) {
			if (SysTick->CNT - interval_timer_start_tick > long_click_max_interval) {
				dblclick_state = DCSLongClicked;
				PT_SPAWN(pt,&pt_lb,long_blink_led(&pt_lb));
				//long_blink_led();
			} else {
				dblclick_state = DCSCandLongClicked;
			}
		} else if (dblclick_state == DCSLongClicked) {
			if (btn_pushed()) {
				dblclick_state = DCSLongClicked;
			} else {
				dblclick_state = DCSCandReleased;
			}
		} else if (dblclick_state == DCSCandFirstClicked) {
			if (btn_pushed()){
				dblclick_state = DCSCandFirstClicked;
			} else {
				dblclick_state = DCSFirstClicked;
				interval_timer_start_tick = SysTick->CNT;
			}
		} else if (dblclick_state == DCSFirstClicked) {
			if (SysTick->CNT - interval_timer_start_tick <= double_click_max_interval) {
				dblclick_state = DCSCandDblClicked;
			} else {
				dblclick_state = DCSClicked;
				PT_SPAWN(pt, &pt_bl, blink_led(&pt_bl));
			}
		} else if (dblclick_state == DCSCandDblClicked) {
			if (btn_pushed()){
				dblclick_state = DCSDblClicked;
				PT_SPAWN(pt,&pt_dbl,double_blink_led(&pt_dbl));
				//double_blink_led();
			} else {
				dblclick_state = DCSFirstClicked;
			}
		} else if (dblclick_state == DCSClicked) {
			if (btn_pushed()){
				dblclick_state = DCSClicked;
			} else {
				dblclick_state = DCSCandReleased;
			}
		} else if (dblclick_state == DCSDblClicked) {
			if (btn_pushed()){
				dblclick_state = DCSDblClicked;
			} else {
				dblclick_state = DCSCandReleased;
			}
		} else if (dblclick_state == DCSCandReleased) {
			if (btn_pushed()){
				dblclick_state = DCSCandPushed;
			} else {
				dblclick_state = DCSReleased;
			}
		}
		next_check_tick = SysTick->CNT + check_intvl;
	}
	PT_END(pt);
}

static struct pt pt_bt;
int main() {
	SystemInit();
	//PT_INIT(&pt_blm);
	PT_INIT(&pt_bt);

	// Enable GPIOs
	funGpioInitAll();
	funPinMode(BTN, GPIO_CNF_IN_PUPD);	
	funDigitalWrite(BTN, FUN_HIGH ); // pull-up
	funPinMode(LED, GPIO_Speed_10MHz|GPIO_CNF_OUT_PP);	
	
	while(1) {
		next_check_tick = SysTick->CNT;
		btn_det(&pt_bt);
		//blink(&pt_blm);
	}
}
