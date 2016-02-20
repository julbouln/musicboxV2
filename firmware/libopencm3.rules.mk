##
## This file is part of the libopencm3 project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
## Copyright (C) 2010 Piotr Esden-Tempski <piotr@esden.net>
## Copyright (C) 2013 Frantisek Burian <BuFran@seznam.cz>
##
## This library is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this library.  If not, see <http://www.gnu.org/licenses/>.
##

# Be silent per default, but 'make V=1' will show all compiler calls.
V=1
ifneq ($(V),1)
Q		:= @
NULL		:= 2>/dev/null
endif

###############################################################################
# Executables

PREFIX		?= arm-none-eabi

CC		:= $(PREFIX)-gcc
CXX		:= $(PREFIX)-g++
LD		:= $(PREFIX)-gcc
AR		:= $(PREFIX)-ar
AS		:= $(PREFIX)-as
OBJCOPY		:= $(PREFIX)-objcopy
OBJDUMP		:= $(PREFIX)-objdump
STRIP	:= $(PREFIX)-strip
SIZE	:= $(PREFIX)-size
GDB		:= $(PREFIX)-gdb
STFLASH		= $(shell which st-flash)
JLINK	= $(shell which JLinkExe)
JLINK_SPEED = 1000
JLINK_PROTO = SWD
STYLECHECK	:= /checkpatch.pl
STYLECHECKFLAGS	:= --no-tree -f --terse --mailback
STYLECHECKFILES	:= $(shell find . -name '*.[ch]')


###############################################################################
# Source files

LDSCRIPT	?= $(BINARY).ld

OBJS		+= $(BINARY).o


ifeq ($(strip $(OPENCM3_DIR)),)
# user has not specified the library path, so we try to detect it

# where we search for the library
LIBPATHS := ./libopencm3 ../../../../libopencm3 ../../../../../libopencm3

OPENCM3_DIR := $(wildcard $(LIBPATHS:=/locm3.sublime-project))
OPENCM3_DIR := $(firstword $(dir $(OPENCM3_DIR)))

ifeq ($(strip $(OPENCM3_DIR)),)
$(warning Cannot find libopencm3 library in the standard search paths.)
$(error Please specify it through OPENCM3_DIR variable!)
endif
endif

ifeq ($(V),1)
$(info Using $(OPENCM3_DIR) path to library)
endif

INCLUDE_DIR	= $(OPENCM3_DIR)/include
LIB_DIR		= $(OPENCM3_DIR)/lib
SCRIPT_DIR	= $(OPENCM3_DIR)/scripts

# ccgmusic

INCLUDE_CCGMUSIC_PATHS=-I ccgmusic/src/ -I ccgmusic/src/structuregenerators/ -I ccgmusic/src/rythmgenerators/ -I ccgmusic/src/renderers/ -I ccgmusic/src/melodycreators/ -I ccgmusic/src/ornamentors/ -I ccgmusic/src/innerstructuregenerators/ -I ccgmusic/src/arrangers/ -I ccgmusic/src/harmonygenerators/
CCGMUSIC_SRCS=$(wildcard ccgmusic/src/*.cpp) $(wildcard ccgmusic/src/*/*.cpp)
CCGMUSIC_OBJS=$(CCGMUSIC_SRCS:.cpp=.o)

# atomthreads
INCLUDE_ATOMTHREADS_PATHS=-I atomthreads -I atomthreads/kernel
ATOMTHREADS_SRCS=$(wildcard atomthreads/kernel/*.c) $(wildcard atomthreads/*.c)
ATOMTHREADS_OBJS=$(ATOMTHREADS_SRCS:.c=.o)

ASFLAGS     += -D__ASSEMBLY__ 
ASFLAGS     += -D__NEWLIB__

###############################################################################
# C flags

CFLAGS		+= -Os -g
CFLAGS		+= -Wextra -Wshadow -Wimplicit-function-declaration
CFLAGS		+= -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes
CFLAGS		+= -fno-common -ffunction-sections -fdata-sections 

###############################################################################
# C++ flags

CXXFLAGS	+= -Os -fno-use-cxa-atexit -g
CXXFLAGS	+= -Wextra -Wshadow -Wredundant-decls  -Weffc++
CXXFLAGS	+= -fno-common -ffunction-sections -fdata-sections

###############################################################################
# C & C++ preprocessor common flags

CPPFLAGS	+= -MD
CPPFLAGS	+= -Wall -Wundef
CPPFLAGS	+= -I. -Iprograms -I$(INCLUDE_DIR) $(INCLUDE_ATOMTHREADS_PATHS) $(INCLUDE_CCGMUSIC_PATHS) $(DEFS)

###############################################################################
# Linker flags

LDFLAGS		+= -static -nostartfiles -fno-use-cxa-atexit
LDFLAGS		+= -L$(LIB_DIR)
LDFLAGS		+= -T$(LDSCRIPT)
LDFLAGS		+= -Wl,-Map=$(*).map
LDFLAGS		+= -Wl,--gc-sections
ifeq ($(V),99)
LDFLAGS		+= -Wl,--print-gc-sections
endif

###############################################################################
# Used libraries

LDLIBS		+= -Os
LDLIBS		+= -l$(LIBNAME)
LDLIBS		+= --specs=nano.specs --specs=nosys.specs
#LDLIBS		+= --specs=rdimon.specs -lgcc -lc -lm -lrdimon
LDLIBS		+= -Wl,--start-group -lc -lgcc -lnosys -lm -lstdc++ -lsupc++ -Wl,--end-group
#LDLIBS		+= --specs=nano.specs 
###############################################################################
###############################################################################
###############################################################################

.SUFFIXES: .elf .bin .hex .srec .list .map .images
.SECONDEXPANSION:
.SECONDARY:

all: elf

elf: $(BINARY).elf
bin: $(BINARY).bin
hex: $(BINARY).hex
srec: $(BINARY).srec
list: $(BINARY).list

images: $(BINARY).images
flash: $(BINARY).flash

%.images: %.bin %.hex %.srec %.list %.map
	@#printf "*** $* images generated ***\n"

%.bin: %.elf
	@#printf "  OBJCOPY $(*).bin\n"
	$(Q)$(OBJCOPY) -Obinary $(*).elf $(*).bin

%.hex: %.elf
	@#printf "  OBJCOPY $(*).hex\n"
	$(Q)$(OBJCOPY) -Oihex $(*).elf $(*).hex

%.srec: %.elf
	@#printf "  OBJCOPY $(*).srec\n"
	$(Q)$(OBJCOPY) -Osrec $(*).elf $(*).srec

%.list: %.elf
	@#printf "  OBJDUMP $(*).list\n"
	$(Q)$(OBJDUMP) -S $(*).elf > $(*).list

%.elf %.map: $(OBJS) $(LDSCRIPT) $(LIB_DIR)/lib$(LIBNAME).a
	@#printf "  LD      $(*).elf\n"
	$(Q)$(LD) $(LDFLAGS) $(ARCH_FLAGS) $(OBJS) $(LDLIBS) -o $(*).elf
#	$(STRIP) $(*).elf
	$(Q)$(SIZE) $(*).elf
	
%.o: %.S
	$(CC) $(CFLAGS) $(ARCH_FLAGS) $(ASFLAGS) -I`dirname $<` -c $< -o $@
	
%.o: %.c 
	@#printf "  CC      $(*).c\n"
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) $(ARCH_FLAGS) -o $(*).o -c $(*).c

%.o: %.cxx
	@#printf "  CXX     $(*).cxx\n"
	$(Q)$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(ARCH_FLAGS) -o $(*).o -c $(*).cxx

%.o: %.cpp
	@#printf "  CXX     $(*).cpp\n"
	$(Q)$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(ARCH_FLAGS) -o $(*).o -c $(*).cpp

clean:
	@#printf "  CLEAN\n"
	$(Q)$(RM) *.o *.d *.elf *.bin *.hex *.srec *.list *.map
	$(Q)$(RM) ./lib/*.o ./lib/*.d
	$(Q)$(RM) ./atomthreads/*.o ./atomthreads/*.d
	$(Q)$(RM) ./atomthreads/kernel/*.o ./atomthreads/kernel/*.d

stylecheck: $(STYLECHECKFILES:=.stylecheck)
styleclean: $(STYLECHECKFILES:=.styleclean)

# the cat is due to multithreaded nature - we like to have consistent chunks of text on the output
%.stylecheck: %
	$(Q)$(SCRIPT_DIR)$(STYLECHECK) $(STYLECHECKFLAGS) $* > $*.stylecheck; \
		if [ -s $*.stylecheck ]; then \
			cat $*.stylecheck; \
		else \
			rm -f $*.stylecheck; \
		fi;

%.styleclean:
	$(Q)rm -f $*.stylecheck;


%.stlink-flash: %.bin
	@printf "  FLASH  $<\n"
	$(Q)$(STFLASH) write $(*).bin 0x8000000

%.jlink-flash: %.bin
	@printf " Flashing $< via JLink to $(DEVICE)\n"
	$(CURDIR)/jlink_flash $(DEVICE) $(JLINK_SPEED) $<

program: $(BINARY).bin
	sudo dfu-util -v -c 1 -i 0 -a 0 -s 0x08000000 -D $(BINARY).bin

openocd: $(BINARY).elf
	openocd -f interface/stlink-v2.cfg -c "hla_vid_pid 0x0483 0x374b" -f target/stm32f4x_stlink.cfg

openocd_telnet: $(BINARY).elf
	telnet localhost 4444

openocd_gdb: $(BINARY).elf
	arm-none-eabi-gdb --eval-command="target remote localhost:3333" $(BINARY).elf

#ifeq ($(STLINK_PORT),)
#ifeq ($(BMP_PORT),)
#ifeq ($(OOCD_SERIAL),)
#ifeq ($(JLINK_EXE),)
#%.flash: %.hex
#	@printf "  FLASH   $<\n"
#	@# IMPORTANT: Don't use "resume", only "reset" will work correctly!
#	$(Q)$(OOCD) -f interface/$(OOCD_INTERFACE).cfg \
#		    -f board/$(OOCD_BOARD).cfg \
#		    -c "init" -c "reset init" \
#		    -c "flash write_image erase $(*).hex" \
#		    -c "reset" \
#		    -c "shutdown" $(NULL)
#else
#%.flash: %.hex
#	@printf "  FLASH   $<\n"
#	@# IMPORTANT: Don't use "resume", only "reset" will work correctly!
#	$(Q)$(OOCD) -f interface/$(OOCD_INTERFACE).cfg \
#		    -f board/$(OOCD_BOARD).cfg \
#		    -c "ft2232_serial $(OOCD_SERIAL)" \
#		    -c "init" -c "reset init" \
#		    -c "flash write_image erase $(*).hex" \
#		    -c "reset" \
#		    -c "shutdown" $(NULL)
#endif
#else
#%.flash: %.elf
#	@printf "  GDB   $(*).elf (flash)\n"
#	$(Q)$(GDB) --batch \
#		   -ex 'target extended-remote $(BMP_PORT)' \
#		   -x $(SCRIPT_DIR)/black_magic_probe_flash.scr \
#		   $(*).elf
#endif
#else
#%.flash: %.elf
#	@printf "  GDB   $(*).elf (flash)\n"
#	$(Q)$(GDB) --batch \
#		   -ex 'target extended-remote $(STLINK_PORT)' \
#		   -x $(SCRIPT_DIR)/stlink_flash.scr \
#		   $(*).elf
#endif
#else
%.flash: %.bin
	@printf "Flashing via JlinkExe $(*).bin (flash)\n"
	$(CURDIR)/jlink_flash $(DEVICE) $(JLINK_SPEED) $<
#endif


.PHONY: images clean stylecheck styleclean elf bin hex srec list

-include $(OBJS:.o=.d)
