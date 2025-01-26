all : flash

TARGET:=hello_protothreads

CH32V003FUN=/home/siuyin/ch32v003fun/ch32v003fun
include $(CH32V003FUN)/ch32v003fun.mk
CFLAGS+=-isystem ../pt

flash : cv_flash
clean : cv_clean


