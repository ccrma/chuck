############################# COMPILER CONFIGURATION ###################################
# what to build
HOST_EXES=example-1-minimal example-2-audio example-3-globals
# where to find top-level chuck
CHUCK_DIR=../../../chuck

# where to find chuck core (chuck as library)
CHUCK_CORE_DIR=$(CHUCK_DIR)/src/core
# where to find RtAudio source (for host)
HOST_RTAUDIO_DIR=$(CHUCK_DIR)/src/host/RtAudio
# the host directory
HOST_DIR=.

# compiler tools
LEX=flex
YACC=bison
CC=gcc
CXX=g++
LD=g++

# compiler flags
CFLAGS+=-I. -I$(CHUCK_CORE_DIR) -I$(CHUCK_CORE_DIR)/lo


########################## DEFAULT MAKE TARGET #################################
# default target: print usage message and quit
current:
	@echo "[chuck build]: please use one of the following configurations:"
	@echo "   make linux-alsa, make linux-jack, make linux-pulse, linux-all,"
	@echo "   make mac, make mac-ub"



################################ PLATFORMS #####################################
## note: If you want to build ChucK on linux with multiple audio drivers,
## combine multiple build rules, e.g.:
## > make linux-pulse linux-alsa linux-jack

.PHONY: mac mac-ub osx linux-all linux linux-pulse linux-jack linux-alsa
mac mac-ub osx linux-all linux linux-pulse linux-jack linux-alsa: hosts-all

# macOS
ifneq (,$(strip $(filter mac mac-ub osx,$(MAKECMDGOALS))))
include $(CHUCK_CORE_DIR)/makefile.x/makefile.mac
endif

# linux (ALSA)
ifneq (,$(strip $(filter linux-alsa,$(MAKECMDGOALS))))
include $(CHUCK_CORE_DIR)/makefile.x/makefile.alsa
endif

# linux (PulseAudio)
ifneq (,$(strip $(filter linux-pulse,$(MAKECMDGOALS))))
include $(CHUCK_CORE_DIR)/makefile.x/makefile.pulse
endif

# linux (JACK)
ifneq (,$(strip $(filter linux-jack,$(MAKECMDGOALS))))
include $(CHUCK_CORE_DIR)/makefile.x/makefile.jack
endif

# linux (all drivers)
ifneq (,$(strip $(filter linux linux-all,$(MAKECMDGOALS))))
include $(CHUCK_CORE_DIR)/makefile.x/makefile.linux
endif

# Windows -- use visual studio project


########################## CHUCK CORE LIB TARGETS ##############################
COBJS_CORE+=chuck.tab.o chuck.yy.o util_math.o util_network.o util_raw.o \
	util_xforms.o
CXXOBJS_CORE+=chuck.o chuck_absyn.o chuck_parse.o chuck_errmsg.o \
	chuck_frame.o chuck_symbol.o chuck_table.o chuck_utils.o \
	chuck_vm.o chuck_instr.o chuck_scan.o chuck_type.o chuck_emit.o \
	chuck_compile.o chuck_dl.o chuck_oo.o chuck_lang.o chuck_ugen.o \
	chuck_otf.o chuck_stats.o chuck_shell.o chuck_io.o \
	chuck_carrier.o chuck_globals.o hidio_sdl.o midiio_rtmidi.o \
	rtmidi.o ugen_osc.o ugen_filter.o ugen_stk.o ugen_xxx.o ulib_ai.o \
	ulib_doc.o ulib_machine.o ulib_math.o ulib_std.o ulib_opsc.o \
	util_buffers.o util_console.o util_hid.o util_platforms.o \
	util_opsc.o util_serial.o util_string.o util_thread.o \
	uana_xform.o uana_extract.o
LO_COBJS_CORE+=lo/address.o lo/blob.o lo/bundle.o lo/message.o lo/method.o \
	lo/pattern_match.o lo/send.o lo/server.o lo/server_thread.o lo/timetag.o


############################ HOST TARGETS ################################
# using RtAudio for real-time audio
CXXINC_HOST+=-I${HOST_RTAUDIO_DIR} -I${HOST_RTAUDIO_DIR}/include
CXXSRCS_HOST+=${HOST_RTAUDIO_DIR}/RtAudio.cpp
CXXSRCS_HOST_EXE+=$(addsuffix .cpp, $(HOST_EXES))


############################ OBJECT FILE TARGETS ###############################

# host object files
CXXOBJS_HOST=$(addprefix $(HOST_DIR)/,$(CXXSRCS_HOST:.cpp=.o))
CXXOBJS_HOST_EXE+=$(addprefix $(HOST_DIR)/,$(CXXSRCS_HOST_EXE:.cpp=.o))

# chuck core object files
COBJS=$(COBJS_HOST) $(addprefix $(CHUCK_CORE_DIR)/,$(COBJS_CORE))
CXXOBJS=$(CXXOBJS_HOST) $(addprefix $(CHUCK_CORE_DIR)/,$(CXXOBJS_CORE))
LO_COBJS=$(addprefix $(CHUCK_CORE_DIR)/,$(LO_COBJS_CORE))
SF_COBJS=$(addprefix $(CHUCK_CORE_DIR)/,$(SF_CSRCS:.c=.o))
# all of the above
OBJS=$(COBJS) $(CXXOBJS) $(LO_COBJS) $(SF_COBJS)


############################ ADDITIONAL FLAGS ##################################
# for liblo headers
LO_CFLAGS=-DHAVE_CONFIG_H -I.

# remove -arch options
CFLAGSDEPEND=$(CFLAGS)

# macOS architecture
ifneq (,$(ARCHS))
ARCHOPTS=$(addprefix -arch ,$(ARCHS))
else
ARCHOPTS=
endif

# macOS univeral binary
ifneq (,$(strip $(filter mac-ub,$(MAKECMDGOALS))))
ARCHOPTS=-arch arm64 -arch x86_64
ARCH_ARGS=MAC_UB=true
CFLAGS+=-D__MACOSX_UB__
endif

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

# host compiler flags
CFLAGS_HOST=${CFLAGS} -I${HOST_RTAUDIO_DIR}
CFLAGSDEPEND_HOST=${CFLAGSDEPEND} ${CFLAGS_HOST}

# host compilation rules C
$(COBJS_HOST): %.o: %.c
	$(CC) $(CFLAGS_HOST) $(ARCHOPTS) -c $< -o $@
	@$(CC) -MM -MQ "$@" $(CFLAGSDEPEND_HOST) $< > $*.d

# host compilation rules C++
$(CXXOBJS_HOST): %.o: %.cpp
	$(CXX) $(CFLAGS_HOST) $(ARCHOPTS) -c $< -o $@
	@$(CXX) -MM -MQ "$@" $(CFLAGSDEPEND_HOST) $< > $*.d

# host exe compilation rules C++
$(CXXOBJS_HOST_EXE): %.o: %.cpp
	$(CXX) $(CFLAGS_HOST) $(ARCHOPTS) -c $< -o $@
	@$(CXX) -MM -MQ "$@" $(CFLAGSDEPEND_HOST) $< > $*.d


############################# MAIN COMPILATION #################################
# build chuck core
chuck-core:
	@echo -------------
	@echo [chuck-core]: compiling...
	@$(MAKE) $(MAKECMDGOALS) -C $(CHUCK_CORE_DIR) $(ARCH_ARGS)
	@echo -------------

# build all
hosts-all: $(HOST_EXES)

# compile every example
$(HOST_EXES): chuck-core $(COBJS_HOST) $(CXXOBJS_HOST) $(CXXOBJS_HOST_EXE)
	@echo "[host-examples]: building: '"$@"'"
	@$(LD) -o $@ $@.o $(OBJS) $(LDFLAGS) $(ARCHOPTS)

# clean generated files
clean:
	@rm -rf $(HOST_EXES) *.o *.d */*.{o,d} */*/*.{o,d} \
	$(OBJS) $(patsubst %.o,%.d,$(OBJS))*~ $(CHUCK_CORE_DIR)/chuck.output \
	$(CHUCK_CORE_DIR)/chuck.tab.h $(CHUCK_CORE_DIR)/chuck.tab.c $(CHUCK_CORE_DIR)/chuck.yy.c \
	Release Debug
