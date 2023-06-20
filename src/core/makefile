# default target: print usage message and quit
current:
	@echo "[chuck build]: please use one of the following configurations:"
	@echo "   make linux-alsa, make linux-jack, make linux-pulse, linux-all,"
	@echo "   make mac, make mac-ub, make cygwin, or make win32"

.PHONY: mac mac-ub osx linux-all linux linux-pulse linux-jack linux-alsa cygwin osx-rl test
mac mac-ub osx linux-all linux linux-pulse linux-jack linux-alsa cygwin osx-rl: chuck-core

win32:
	make -f makefile.win32

LEX=flex
YACC=bison
CC=gcc
CXX=g++
LD=g++

CFLAGS+=-I. -Ilo

ifneq ($(CHUCK_STAT),)
CFLAGS+= -D__CHUCK_STAT_TRACK__
endif

ifneq ($(CHUCK_DEBUG),)
CFLAGS+= -g
else
CFLAGS+= -O3
endif

ifneq ($(USE_64_BIT_SAMPLE),)
CFLAGS+= -D__CHUCK_USE_64_BIT_SAMPLE__
endif

ifneq ($(CHUCK_STRICT),)
CFLAGS+= -Wall
endif

# ifneq ($(findstring arm,$(shell uname -m)),)
# some sort of arm platform- enable aggressive optimizations
# CFLAGS+= -ffast-math
# endif
# ge: commenting out; breaks Math.isinf() since it assumes all finite
# https://stackoverflow.com/questions/7420665/what-does-gccs-ffast-math-actually-do
# https://simonbyrne.github.io/notes/fastmath/

ifneq (,$(strip $(filter mac mac-ub osx bin-dist-mac,$(MAKECMDGOALS))))
include makefile.x/makefile.mac
endif

ifneq (,$(strip $(filter linux linux-all,$(MAKECMDGOALS))))
include makefile.x/makefile.linux
endif

ifneq (,$(strip $(filter linux-pulse,$(MAKECMDGOALS))))
include makefile.x/makefile.pulse
endif

ifneq (,$(strip $(filter linux-jack,$(MAKECMDGOALS))))
include makefile.x/makefile.jack
endif

ifneq (,$(strip $(filter linux-alsa,$(MAKECMDGOALS))))
include makefile.x/makefile.alsa
endif

ifneq (,$(strip $(filter cygwin,$(MAKECMDGOALS))))
include makefile.x/makefile.cygwin
endif

ifneq (,$(strip $(filter osx-rl,$(MAKECMDGOALS))))
include makefile.x/makefile.rl
endif

########################## CHUCK CORE LIB TARGETS ##############################
CSRCS_CORE+= chuck.tab.c chuck.yy.c util_network.c \
	util_raw.c util_xforms.c
CXXSRCS_CORE+= chuck_absyn.cpp chuck_parse.cpp chuck_errmsg.cpp \
	chuck_frame.cpp chuck_symbol.cpp chuck_table.cpp chuck_utils.cpp \
	chuck_vm.cpp chuck_instr.cpp chuck_scan.cpp chuck_type.cpp \
	chuck_emit.cpp chuck_compile.cpp chuck_dl.cpp chuck_oo.cpp \
	chuck_lang.cpp chuck_ugen.cpp chuck_otf.cpp chuck_stats.cpp \
	chuck_shell.cpp chuck_io.cpp chuck.cpp \
	chuck_carrier.cpp chuck_globals.cpp \
	midiio_rtmidi.cpp rtmidi.cpp hidio_sdl.cpp \
	ugen_osc.cpp ugen_filter.cpp ugen_stk.cpp ugen_xxx.cpp \
	ulib_ai.cpp ulib_doc.cpp ulib_machine.cpp ulib_math.cpp \
	ulib_std.cpp ulib_opsc.cpp util_buffers.cpp util_console.cpp \
	util_math.cpp util_string.cpp util_thread.cpp util_opsc.cpp \
	util_serial.cpp util_hid.cpp uana_xform.cpp uana_extract.cpp
LO_CSRCS+= lo/address.c lo/blob.c lo/bundle.c lo/message.c lo/method.c \
    lo/pattern_match.c lo/send.c lo/server.c lo/server_thread.c lo/timetag.c
################################################################################

CSRCS+= $(CSRCS_CORE) $(SF_CSRCS)
CXXSRCS+= $(CXXSRCS_CORE)

COBJS=$(CSRCS:.c=.o)
CXXOBJS=$(CXXSRCS:.cpp=.o)
LO_COBJS=$(LO_CSRCS:.c=.o)
OBJS=$(COBJS) $(CXXOBJS) $(LO_COBJS)

LO_CFLAGS=-DHAVE_CONFIG_H -I.

# remove -arch options
CFLAGSDEPEND=$(CFLAGS)

ifneq (,$(ARCHS))
ARCHOPTS=$(addprefix -arch ,$(ARCHS))
else
ARCHOPTS=
endif

# 1.4.1.1 (ge) intel/apple silicon universal binary
ifneq (,$(MAC_UB))
ARCHOPTS=-arch arm64 -arch x86_64
endif

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

chuck-core: $(OBJS)
	@echo '[chuck-core]: all components built...'

chuck.tab.c chuck.tab.h: chuck.y
	$(YACC) -dv -b chuck chuck.y

chuck.lex: chuck.tab.h
chuck.yy.c: chuck.lex
	$(LEX) -ochuck.yy.c chuck.lex

chuck_win32.c: chuck.yy.c
	$(LEX) --nounistd -ochuck.yy.c chuck.lex
	cat chuck.tab.c chuck.yy.c > $@

chuck_win32.h: chuck.tab.h
	cat chuck.tab.h > $@

$(CXXOBJS): %.o: %.cpp
	$(CXX) $(CFLAGS) $(ARCHOPTS) -c $< -o $@
	@$(CXX) -MM -MQ "$@" $(CFLAGSDEPEND) $< > $*.d

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) $(ARCHOPTS) -c $< -o $@
	@$(CC) -MM -MQ "$@" $(CFLAGSDEPEND) $< > $*.d

$(LO_COBJS): %.o: %.c
	$(CC) $(CFLAGS) $(LO_CFLAGS) $(ARCHOPTS) -c $< -o $@
	@$(CC) -MM -MQ "$@" $(CFLAGSDEPEND) $(LO_CFLAGS) $< > $*.d

clean:
	@rm -rf *.o *.d $(OBJS) \
         $(patsubst %.o,%.d,$(OBJS)) *~ chuck.output chuck.tab.h chuck.tab.c \
         chuck.yy.c $(DIST_DIR){,.tgz,.zip} Release Debug
