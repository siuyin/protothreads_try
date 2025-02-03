#include "ch32v003fun.h"
#include "pt.h"
#include <stdio.h>

#define MY_FLASH_BASE 0x08003c00

#define unlockFlash() { \
	FLASH->KEYR = 0x45670123; \
	FLASH->KEYR = 0xCDEF89AB; \
}

#define unlockFastPgmMode() { \
	FLASH->MODEKEYR = 0x45670123; \
	FLASH->MODEKEYR = 0xCDEF89AB; \
}

#define isFlashLocked() (( FLASH->CTLR & 0x8080 ) ? 1 : 0 )

#define erase64BytePage(ptr) { \
	FLASH->CTLR = CR_PAGE_ER; \
	FLASH->ADDR = (intptr_t)ptr; \
	FLASH->CTLR = CR_STRT_Set | CR_PAGE_ER; \
	while( FLASH->STATR & FLASH_STATR_BSY ); \
}

#define waitForFlashNotBusy()  while( FLASH->STATR & FLASH_STATR_BSY )

#define clearFlashBuffer(ptr) { \
	FLASH->CTLR = CR_PAGE_PG; \
	FLASH->CTLR = CR_BUF_RST | CR_PAGE_PG; \
	FLASH->ADDR = (intptr_t)ptr; \
	waitForFlashNotBusy(); \
}


#define writeFlash32bits(ptr,val) { \
	*(ptr) = val; \
	FLASH->CTLR = CR_PAGE_PG | FLASH_CTLR_BUF_LOAD; \
	waitForFlashNotBusy(); \
}
#define flushFlashBuffer() FLASH->CTLR = CR_PAGE_PG|CR_STRT_Set

int main() {
	SystemInit();

	unlockFlash();
	unlockFastPgmMode();
	if (isFlashLocked()) {
		printf("FATAL: Flash could not be unlockced\n");
		while(1);
	}

	uint32_t* ptr = (uint32_t*)MY_FLASH_BASE;
	erase64BytePage(ptr);

	clearFlashBuffer(ptr);
	writeFlash32bits(ptr,0xdeadbeef);
	writeFlash32bits(ptr+1,0xbabeface);
	flushFlashBuffer();

	printf("%08lx %08lx\n",ptr[0], ptr[1]);
	while(1) {
	}
}
