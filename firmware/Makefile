##
## This file is part of the libopencm3 project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
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

DEVICE=stm32f405
BINARY = main
#OPENCM3_DIR=/home/spock/chmura/lib/libopencm3
OPENCM3_DIR=/home/julbouln/Perso/stm/libopencm3
#OBJS += ./lib/stepper.o
OBJS = $(CCGMUSIC_OBJS) 
OBJS += $(ATOMTHREADS_OBJS) atomthreads/atomport-asm.o 
OBJS += system_setup.o systick.o rng.o sam2695.o usbmidi.o fonts.o gfx.o interface.o ssd1306.o mcp23s08.o battery.o 
OBJS += MidiRaw.o Program.o 
#programs/EmptyProgram.o programs/CcgProgram.o programs/SynthesizerProgram.o programs/TetrisProgram.o
OBJS += 
CPPFLAGS += -I$(CURDIR)/include
LDSCRIPT = ./stm32f405rg.ld
JLINK_EXE = /usr/local/bin/JLinkExe
include ./libopencm3.target.mk
