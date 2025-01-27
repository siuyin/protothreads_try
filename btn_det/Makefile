all : flash

TARGET:=btn_det

CFLAGS+=-isystem ../pt -I../lsylibs -Os
CH32V003FUN=/home/siuyin/ch32v003fun/ch32v003fun
include $(CH32V003FUN)/ch32v003fun.mk

flash : cv_flash
clean : cv_clean

