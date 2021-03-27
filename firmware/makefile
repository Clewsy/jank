# makefile for jank
#
# Modified from a typical makefile in Dean Camera's LUFA library.
# https://github.com/abcminiuser/lufa

MCU		= atmega32u4
F_CPU		= 16000000
TARGET		= jank
INC_DIR		= ./include
LIB_DIR		= ./lib
SRC_DIR		= ./src
SRC		= $(wildcard $(SRC_DIR)/*.c)
CC_FLAGS	= -I$(INC_DIR)
LD_FLAGS	=

# Default target
all:

##############################################################################################
# Camera has abstracted away a lot of the makefile config with DMBS
ARCH		= AVR8
BOARD		= NONE
F_USB		= $(F_CPU)
OPTIMIZATION	= s
SRC		+= $(LUFA_SRC_USB)
LUFA_PATH	= $(LIB_DIR)/LUFA
CC_FLAGS	+= -DUSE_LUFA_CONFIG_HEADER
# Include LUFA-specific DMBS extension modules
DMBS_LUFA_PATH	?= $(LUFA_PATH)/Build/LUFA
include $(DMBS_LUFA_PATH)/lufa-sources.mk
include $(DMBS_LUFA_PATH)/lufa-gcc.mk
# Include common DMBS build system modules
DMBS_PATH	?= $(LUFA_PATH)/Build/DMBS/DMBS
include $(DMBS_PATH)/core.mk
include $(DMBS_PATH)/cppcheck.mk
include $(DMBS_PATH)/doxygen.mk
include $(DMBS_PATH)/dfu.mk
include $(DMBS_PATH)/gcc.mk
include $(DMBS_PATH)/hid.mk
include $(DMBS_PATH)/avrdude.mk
include $(DMBS_PATH)/atprogram.mk
##############################################################################################


PROGRAMMER_TYPE = stk500v2
PROGRAMMER_PORT = /dev/ttyACM0
PROGRAMMER_ARGS =
AVRDUDE = avrdude

program: $(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -P $(PROGRAMMER_PORT) -p $(MCU) $(PROGRAMMER_ARGS) -U flash:w:$<
