# Application name
APPL ?= graphic_u8glib

#
# root dir of embARC
#
EMBARC_ROOT = ..

MID_SEL = common u8glib

# application source dirs
BOARD=emsk
BD_VER=23
CUR_CORE=arcem7d
TOOLCHAIN=gnu
OS_SEL = freertos
APPL_CSRC_DIR = . ./src ./src/GUI ./src/TASK/ 
APPL_ASMSRC_DIR = .

# application include dirs
APPL_INC_DIR = . ./inc

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile

### Options above must be added before include options.mk ###
# include key embARC build system makefile
override EMBARC_ROOT := $(strip $(subst \,/,$(EMBARC_ROOT)))
include $(EMBARC_ROOT)/options/options.mk
