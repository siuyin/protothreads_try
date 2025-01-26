#ifndef PROTO_DELAY_H
#define PROTO_DELAY_H

// DELAY macro provides a non-blocking delay of dly milliseconds.
// requires a start_tick_var declared as follows:
//   volatile uint32_t my_start_tick
// invoked thus:
//   DELAY(my_start_tick,500) // delay for 500ms
#define DELAY(start_tick_var, dly) {\
	start_tick_var = SysTick->CNT;\
	PT_WAIT_UNTIL(pt, SysTick->CNT - start_tick_var >= Ticks_from_Ms(dly));\
}

#endif
