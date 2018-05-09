TOOLCHAIN=gnu
BOARD=emsk
BD_VER=23
CUR_CORE=arcem7d

# Application name
APPL ?= blinky

OLEVEL ?= O3


HEAPSZ ?= 83886080 
STACKSZ ?= 1048576

#
# root dir of embARC
#
EMBARC_ROOT = ..
MID_SEL = common fatfs u8glib
OS_SEL = freertos
# application source dirs
APPL_CSRC_DIR = ./src/GUI ./src/MEM ./src/MUSIC ./src/MUSIC/mp3_dec ./src/MUSIC/HW_RAW ./src/NET ./src/NET/esp8266_wifi ./src/TASK 

APPL_ASMSRC_DIR = .

# application include dirs
APPL_INC_DIR =  ./inc ./inc/mp3_dec

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile

### Options above must be added before include options.mk ###
# include key embARC build system makefile
override EMBARC_ROOT := $(strip $(subst \,/,$(EMBARC_ROOT)))
include $(EMBARC_ROOT)/options/options.mk