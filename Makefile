CC		= gcc
CCFLAGS		= -O3 -fomit-frame-pointer -ffast-math  -fno-math-errno -funsafe-math-optimizations -fno-trapping-math
ifdef CPU
CCFLAGS		+= -march=${CPU}
endif

ifdef SSE2
CCFLAGS		+= -msse2 -mfpmath=sse
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

