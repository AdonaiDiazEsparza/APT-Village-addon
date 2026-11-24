all : build flash

TARGET:=main
TARGET_MCU?=CH32V003

# GCC 15 + -flto breaks rv32ec (lto1: 'V' requires 'M'). Same flags as
# ch32fun.mk defaults, without -flto. Harmless on older GCC/Ubuntu.
CFLAGS:=-g -Os -ffunction-sections -fdata-sections -fmessage-length=0 -msmall-data-limit=8 -fno-tree-loop-distribute-patterns

include ch32fun/ch32fun/ch32fun.mk

build : $(TARGET).bin
flash : cv_flash
clean : cv_clean
