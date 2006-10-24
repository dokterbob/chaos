# Copyright (c)2006 Mathijs de Bruin
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA */

CC		= gcc
CCFLAGS		= -O2

ifndef DEBUG
CCFLAGS		+= -O3 -fomit-frame-pointer -ffast-math  -fno-math-errno -funsafe-math-optimizations -fno-trapping-math
endif

ifdef CPU
CCFLAGS		+= -march=${CPU}
endif

ifdef SSE2
CCFLAGS		+= -msse2 -mfpmath=sse
endif

ifdef STEPLOG
CCFLAGS		+= -DSTEPLOG
endif

ifdef LZW
CCFLAGS		+= -DLZW
endif

LD		= ${CC}
LDFLAGS	 	= 

CPPLIBS		= -lm -ldl -lgcc -lc -ltiff

ifdef DEBUG
CCFLAGS		+= -g -pg -DDEBUG
LDFLAGS		+= -pg 
endif

LIBS		=${CPPLIBS} ${GPLLIBS}

all:		chaos.x
		./chaos.x

time:		chaos.x
		time ./chaos.x

ifdef DEBUG
profile:	chaos.x
		./chaos.x
		gprof chaos.x
endif	
.PHONY: clean
clean:
		- rm -f *.c~
		- rm -f *.o
		- rm -f *.x

.PHONY: cleanplots
cleanplots:
		- rm -f #* 

.PHONY: directories
directories:
		- mkdir ${SRCDIR}
		- mkdir ${INCDIR}
		- mkdir ${OBJDIR}
		- mkdir ${EXEDIR}

chaos.x:	$(patsubst %.c,%.o,$(wildcard *.c))
		$(LD) $(LDFLAGS) *.o -o $@ $(LIBS)


.SECONDARY: %.o
%.o:		%.c %.h 
		$(CC) -c $(CCFLAGS) $< -o $@

