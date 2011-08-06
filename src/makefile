DESTDIR=/usr/bin

# default target: print usage message and quit
current: 
	@echo "[chuck build]: please use one of the following configurations:"
	@echo "   make linux-alsa, make linux-jack, make linux-oss,"
	@echo "   make osx, make osx-ub, or make win32"

install:
	cp $(wildcard chuck chuck.exe) $(DESTDIR)/
	chmod 755 $(DESTDIR)/$(wildcard chuck chuck.exe)

.PHONY: osx osx-intel osx-ppc osx-ub linux-oss linux-jack linux-alsa win32 osx-rl
osx osx-intel osx-ppc osx-ub linux-oss linux-jack linux-alsa win32 osx-rl: chuck

LEX=flex
YACC=bison
CXX=gcc
LD=g++

ifneq ($(CHUCK_STAT),)
CFLAGS+= -D__CHUCK_STAT_TRACK__
endif

ifneq ($(CHUCK_DEBUG),)
CFLAGS+= -g
else
CFLAGS+= -O3
endif

ifneq ($(CHUCK_STRICT),)
CFLAGS+= -Wall
endif

ifneq (,$(strip $(filter osx osx-intel osx-ppc,$(MAKECMDGOALS))))
include makefile.osx
endif

# Mac OS X universal binary
ifneq (,$(strip $(filter osx-ub,$(MAKECMDGOALS))))
include makefile.osx-ub
endif

ifneq (,$(strip $(filter linux-oss,$(MAKECMDGOALS))))
include makefile.oss
endif

ifneq (,$(strip $(filter linux-jack,$(MAKECMDGOALS))))
include makefile.jack
endif

ifneq (,$(strip $(filter linux-alsa,$(MAKECMDGOALS))))
include makefile.alsa
endif

ifneq (,$(strip $(filter win32,$(MAKECMDGOALS))))
include makefile.win32
endif

ifneq (,$(strip $(filter osx-rl,$(MAKECMDGOALS))))
include makefile.rl
endif

CSRCS+= chuck.tab.c chuck.yy.c util_math.c util_network.c util_raw.c \
	util_xforms.c
CXXSRCS+= chuck_absyn.cpp chuck_parse.cpp chuck_errmsg.cpp \
	chuck_frame.cpp chuck_symbol.cpp chuck_table.cpp chuck_utils.cpp \
	chuck_vm.cpp chuck_instr.cpp chuck_scan.cpp chuck_type.cpp chuck_emit.cpp \
	chuck_compile.cpp chuck_dl.cpp chuck_oo.cpp chuck_lang.cpp chuck_ugen.cpp \
	chuck_main.cpp chuck_otf.cpp chuck_stats.cpp chuck_bbq.cpp chuck_shell.cpp \
	chuck_console.cpp chuck_globals.cpp digiio_rtaudio.cpp hidio_sdl.cpp \
	midiio_rtmidi.cpp RtAudio/RtAudio.cpp rtmidi.cpp ugen_osc.cpp ugen_filter.cpp \
	ugen_stk.cpp ugen_xxx.cpp ulib_machine.cpp ulib_math.cpp ulib_std.cpp \
	ulib_opsc.cpp util_buffers.cpp util_console.cpp \
	util_string.cpp util_thread.cpp util_opsc.cpp \
	util_hid.cpp uana_xform.cpp uana_extract.cpp

COBJS=$(CSRCS:.c=.o)
CXXOBJS=$(CXXSRCS:.cpp=.o)
OBJS=$(COBJS) $(CXXOBJS)

CFLAGSDEPEND?=$(CFLAGS)

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

chuck: $(OBJS)
	$(LD) -o chuck $(OBJS) $(LDFLAGS)

chuck.tab.c chuck.tab.h: chuck.y
	$(YACC) -dv -b chuck chuck.y

chuck.yy.c: chuck.lex
	$(LEX) -ochuck.yy.c chuck.lex

$(COBJS): %.o: %.c
	$(CXX) $(CFLAGS) -c $< -o $@
	@$(CXX) -MM $(CFLAGSDEPEND) $< > $*.d

$(CXXOBJS): %.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@
	@$(CXX) -MM $(CFLAGSDEPEND) $< > $*.d

clean: 
	@rm -f $(wildcard chuck chuck.exe) $(OBJS) $(patsubst %.o,%.d,$(OBJS)) \
	*~ chuck.output chuck.tab.h chuck.tab.c chuck.yy.c
