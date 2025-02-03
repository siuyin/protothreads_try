all : flash

TARGET:=pwm1Hz

CH32V003FUN=/home/siuyin/ch32v003fun/ch32v003fun
include $(CH32V003FUN)/ch32v003fun.mk
CFLAGS+=-isystem ../pt -I../lsylibs

flash : cv_flash
clean : cv_clean

