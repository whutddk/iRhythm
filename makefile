TOOLCHAIN=gnu
BOARD=emsk
BD_VER=23
CUR_CORE=arcem7d

# Application name
APPL ?= blinky



#
# root dir of embARC
#
EMBARC_ROOT = ..
MID_SEL = common fatfs 
OS_SEL = freertos
# application source dirs
APPL_CSRC_DIR = ./src/GUI ./src/MEM ./src/MUSIC ./src/NET ./src/NET/esp8266_wifi ./src/TASK 
APPL_ASMSRC_DIR = .

# application include dirs
APPL_INC_DIR = ./inc

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile

### Options above must be added before include options.mk ###
# include key embARC build system makefile
override EMBARC_ROOT := $(strip $(subst \,/,$(EMBARC_ROOT)))
include $(EMBARC_ROOT)/options/options.mk