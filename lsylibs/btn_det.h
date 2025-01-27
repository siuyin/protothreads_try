#ifndef BTN_DET_H
#define BTN_DET_H

// btn_det.h provides routines to detect single, double and long button clicks.
// BTN must be defined:
//   eg. #define BTN PD2
// #include "btn_def.h" after including ch32v003fun.h, pt.h and proto_delay.h
//
// You need to implement in your code the following protothreads.
// For example:
// static int single_click_action(struct pt *pt){
// 	PT_BEGIN(pt);
// 	write_str("Single Clicked");
// 
// 	funDigitalWrite(LED,FUN_HIGH);
// 	DELAY(bl_start, 50);
// 	funDigitalWrite(LED,FUN_LOW);
// 
// 	PT_END(pt);
// }
// 
// static int double_click_action(struct pt *pt) {
// 	PT_BEGIN(pt);
// 	write_str("Double Clicked");
// 
// 	funDigitalWrite(LED,FUN_HIGH);
// 	DELAY(dbl_start,125);
// 	funDigitalWrite(LED,FUN_LOW);
// 	DELAY(dbl_start,125);
// 	funDigitalWrite(LED,FUN_HIGH);
// 	DELAY(dbl_start,125);
// 	funDigitalWrite(LED,FUN_LOW);
// 
// 	PT_END(pt);
// }
// 
// static int long_click_action(struct pt *pt) {
// 	PT_BEGIN(pt);
// 	write_str("Long Clicked");
// 
// 	funDigitalWrite(LED,FUN_HIGH);
// 	DELAY(lb_start, 500);
// 	funDigitalWrite(LED,FUN_LOW);
// 
// 	PT_END(pt)
// }


#define btn_pushed() ( funDigitalRead(BTN) == 0 ? 1 : 0 )

static struct pt pt_btn_sca;
static int single_click_action(struct pt *pt);

static struct pt pt_btn_dca;
static int double_click_action(struct pt *pt);

static struct pt pt_btn_lca;
static int long_click_action(struct pt *pt);

typedef enum {
	BSReleased,
	BSCandPushed,
	BSPushed,
	BSCandLongClicked,
	BSLongClickedTimer,
	BSLongClicked,
	BSCandFirstClicked,
	BSFirstClicked,
	BSClicked,
	BSCandDblClicked,
	BSDblClicked,
	BSCandReleased
} BtnState_Typedef;

const uint32_t long_click_max_interval = Ticks_from_Ms(500);
const uint32_t double_click_max_interval = Ticks_from_Ms(150); // tune this value to properly detect a double-click
volatile uint32_t interval_timer_start_tick;
volatile uint32_t next_check_tick;
const uint32_t check_intvl = Ticks_from_Ms(20);
BtnState_Typedef dblclick_state = BSReleased;
// statemachine markdown:
// [![](https://mermaid.ink/img/pako:eNqVVFtvgjAU_ivNeVyEVcDByOaLbk8uWbY9TXyoUIWIYEpZ5oz_faVehrQy9clz-C7nlm4gzCMKPhSccDpMyJyRpfFlBRkSv_HNBBlGH73RlJKCRrvsIZKfBiSLzNeyiEXso_GUZ2glo4kGfAz2SLaP99i6VgVvka2hJGuUZ_NBmoQLBY9ukeiNccSTJWU1ozql0qnFHxVU66tlyuRzwgreKKHRn-LQsK1oskrUR6lIo7DKIx4zWphVIzI5TZNsgUZPwxbRZplH4YdHRVktTleXMgntEA4Lvmy_J5S2m7jq4lTGPxbNii7YpIprZav1KdxWU90N692H0zRsHpPYebRPH27pnIR6h3rmGZLa67kDaZtrrYeKd9qSfpwNygUbvM6kgW5vBTogZrckSSTe1U0lEACP6ZIG4Iu_EZ2RMuUBBNlWQEnJ8_d1FoLPWUk7wPJyHoM_I2khonIV_b3LB8iKZJ95fgxplPCcvezecfmcSwj4G_gGv4tN2_Ww5biO5d7Zdq_bgTX4hm3bJsYW7t1h13Nw19524EeqWqbjuJ6HXce5tx3sWdtf5aP94A?type=png)](https://mermaid.live/edit#pako:eNqVVFtvgjAU_ivNeVyEVcDByOaLbk8uWbY9TXyoUIWIYEpZ5oz_faVehrQy9clz-C7nlm4gzCMKPhSccDpMyJyRpfFlBRkSv_HNBBlGH73RlJKCRrvsIZKfBiSLzNeyiEXso_GUZ2glo4kGfAz2SLaP99i6VgVvka2hJGuUZ_NBmoQLBY9ukeiNccSTJWU1ozql0qnFHxVU66tlyuRzwgreKKHRn-LQsK1oskrUR6lIo7DKIx4zWphVIzI5TZNsgUZPwxbRZplH4YdHRVktTleXMgntEA4Lvmy_J5S2m7jq4lTGPxbNii7YpIprZav1KdxWU90N692H0zRsHpPYebRPH27pnIR6h3rmGZLa67kDaZtrrYeKd9qSfpwNygUbvM6kgW5vBTogZrckSSTe1U0lEACP6ZIG4Iu_EZ2RMuUBBNlWQEnJ8_d1FoLPWUk7wPJyHoM_I2khonIV_b3LB8iKZJ95fgxplPCcvezecfmcSwj4G_gGv4tN2_Ww5biO5d7Zdq_bgTX4hm3bJsYW7t1h13Nw19524EeqWqbjuJ6HXce5tx3sWdtf5aP94A)

static int btn_det(struct pt *pt) {
	PT_BEGIN(pt);
	while (1) {
		if (next_check_tick > SysTick->CNT) {
			continue;
		}
		// IMPORTANT: switch cannot be used in a protothread. Thus this long series of if, else if statements.
		//printf("dblclick state=%d\n",dblclick_state); 
		if (dblclick_state == BSReleased) {
			if (btn_pushed()){
				dblclick_state = BSCandPushed;
			} else {
				dblclick_state = BSReleased;
			}
		} else if (dblclick_state == BSCandPushed) {
			if (btn_pushed()){
				dblclick_state = BSPushed;
			} else {
				dblclick_state = BSCandReleased;
			}
		} else if (dblclick_state == BSPushed) {
			if (btn_pushed()){
				dblclick_state = BSCandLongClicked;
				interval_timer_start_tick = SysTick->CNT;
			} else {
				dblclick_state = BSCandFirstClicked;
			}
		} else if (dblclick_state == BSCandLongClicked) {
			if (btn_pushed()){
				dblclick_state = BSLongClickedTimer;
			} else {
				dblclick_state = BSCandFirstClicked;
			}
		} else if (dblclick_state == BSLongClickedTimer) {
			if (SysTick->CNT - interval_timer_start_tick > long_click_max_interval) {
				dblclick_state = BSLongClicked;
				PT_SPAWN(pt,&pt_btn_lca,long_click_action(&pt_btn_lca));
				//long_click_action();
			} else {
				dblclick_state = BSCandLongClicked;
			}
		} else if (dblclick_state == BSLongClicked) {
			if (btn_pushed()) {
				dblclick_state = BSLongClicked;
			} else {
				dblclick_state = BSCandReleased;
			}
		} else if (dblclick_state == BSCandFirstClicked) {
			if (btn_pushed()){
				dblclick_state = BSCandFirstClicked;
			} else {
				dblclick_state = BSFirstClicked;
				interval_timer_start_tick = SysTick->CNT;
			}
		} else if (dblclick_state == BSFirstClicked) {
			if (SysTick->CNT - interval_timer_start_tick <= double_click_max_interval) {
				dblclick_state = BSCandDblClicked;
			} else {
				dblclick_state = BSClicked;
				PT_SPAWN(pt, &pt_btn_sca, single_click_action(&pt_btn_sca));
			}
		} else if (dblclick_state == BSCandDblClicked) {
			if (btn_pushed()){
				dblclick_state = BSDblClicked;
				PT_SPAWN(pt,&pt_btn_dca,double_click_action(&pt_btn_dca));
				//double_click_action();
			} else {
				dblclick_state = BSFirstClicked;
			}
		} else if (dblclick_state == BSClicked) {
			if (btn_pushed()){
				dblclick_state = BSClicked;
			} else {
				dblclick_state = BSCandReleased;
			}
		} else if (dblclick_state == BSDblClicked) {
			if (btn_pushed()){
				dblclick_state = BSDblClicked;
			} else {
				dblclick_state = BSCandReleased;
			}
		} else if (dblclick_state == BSCandReleased) {
			if (btn_pushed()){
				dblclick_state = BSCandPushed;
			} else {
				dblclick_state = BSReleased;
			}
		}
		next_check_tick = SysTick->CNT + check_intvl;
	}
	PT_END(pt);
}

#endif
