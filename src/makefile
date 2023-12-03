###############################################################################
# name: makefile
# desc: source compilation recipes for ChucK programming language
#
# use this makefile to build:
#   * command-line executable chuck
#   * webchuck modules: webchuck.wasm webchuck.js
#
# ChucK website: https://chuck.stanford.edu/
# source repository: https://github.com/ccrma/chuck/
###############################################################################
# chuck core location
CK_CORE_DIR=core
# chuck (command line) host location
CK_HOST_DIR=host
# where to find RtAudio
RTAUDIO_DIR=RtAudio
# chuck version
CK_VERSION=1.5.2.0


########################## DEFAULT MAKE TARGET ################################
# default target: print usage message and quit
current:
	@echo "[chuck build]: please use one of the following configurations:"
	@echo "   make linux-alsa, make linux-jack, make linux-pulse, linux-all,"
	@echo "   make mac, make mac-ub, make web, make win32"


############################ MAKE WEBCHUCK ####################################
# source for emscripten compilation
EMSCRIPTENSRCS=host-web/chuck_emscripten.cpp \
    core/chuck.cpp core/chuck_absyn.cpp core/chuck_carrier.cpp \
    core/chuck_compile.cpp core/chuck_dl.cpp core/chuck_emit.cpp \
    core/chuck_errmsg.cpp core/chuck_frame.cpp core/chuck_globals.cpp \
    core/chuck_instr.cpp core/chuck_io.cpp core/chuck_lang.cpp \
    core/chuck_oo.cpp core/chuck_parse.cpp core/chuck_scan.cpp \
    core/chuck_stats.cpp core/chuck_symbol.cpp core/chuck_table.cpp \
    core/chuck_type.cpp core/chuck_ugen.cpp core/chuck_vm.cpp \
    core/uana_extract.cpp core/uana_xform.cpp \
    core/ugen_filter.cpp core/ugen_osc.cpp core/ugen_stk.cpp \
    core/ugen_xxx.cpp core/ulib_ai.cpp core/ulib_doc.cpp \
    core/ulib_machine.cpp core/ulib_math.cpp core/ulib_std.cpp \
    core/util_buffers.cpp core/util_math.cpp core/util_platforms.cpp \
    core/util_raw.c core/util_string.cpp core/util_xforms.c \
    core/chuck_yacc.c core/util_sndfile.c

EMSCRIPTEN_POSTJS=host-web/chucknode-postjs.js

# make targets
.PHONY: emscripten web
emscripten: host-web/webchuck/js/webchuck.js
web: host-web/webchuck/js/webchuck.js
# recipe
host-web/webchuck/js/webchuck.js: $(EMSCRIPTENSRCS) $(EMSCRIPTEN_POSTJS)
	emcc -O3 -s MAIN_MODULE=1 -s DISABLE_EXCEPTION_CATCHING=0 -Icore \
	-D__DISABLE_MIDI__ -D__DISABLE_WATCHDOG__ -D__DISABLE_NETWORK__ \
	-D__DISABLE_OTF_SERVER__ -D__ALTER_HID__ -D__DISABLE_HID__ \
	-D__DISABLE_SERIAL__ -D__DISABLE_ASYNCH_IO__ -D__DISABLE_THREADS__ \
	-D__DISABLE_KBHIT__ -D__DISABLE_PROMPTER__ -D__DISABLE_SHELL__ \
	-D__CHUCK_USE_PLANAR_BUFFERS__ -D__OLDSCHOOL_RANDOM__ \
	-Wformat=0 $(EMSCRIPTENSRCS) -o host-web/webchuck/js/webchuck.js \
	-s EXPORTED_FUNCTIONS="['_malloc']" \
	-s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "getValue", "setValue", "addFunction", "removeFunction", "UTF8ToString", "stringToUTF8"]' \
	-s MODULARIZE=1 -s ALLOW_MEMORY_GROWTH=1 \
	-s RESERVED_FUNCTION_POINTERS=50 -s FORCE_FILESYSTEM=1 \
	-s --extern-post-js $(EMSCRIPTEN_POSTJS) \
	-s 'EXPORT_NAME="ChucK"' \


################################ PLATFORMS ####################################
# see ../README.md for more build instruction details
# https://github.com/ccrma/chuck#readme

# make targets
.PHONY: mac mac-ub osx linux-all linux linux-pulse linux-jack linux-alsa test
mac mac-ub osx linux-all linux linux-pulse linux-jack linux-alsa: chuck

win32:
	@echo "for Windows build, see visual-studio/chuck.sln"
#	make -f $(CK_CORE_DIR)/makefile.x/makefile.win32


########################### COMPILATION TOOLS #################################
# lexer
LEX=flex
# parser
YACC=bison
# c compiler
CC=gcc
# c++ compiler
CXX=g++
# linker
LD=$(CXX)


############################# COMPILER FLAGS ##################################
# note: to compile with a specific c++ version (e.g., c++11) add: -std=c++11
CFLAGS+=-I. -I$(CK_CORE_DIR) -I$(CK_CORE_DIR)/lo

# track stats?
ifneq ($(CHUCK_STAT),)
CFLAGS+= -D__CHUCK_STAT_TRACK__
endif

# enable chuck debug macros?
ifneq ($(CHUCK_DEBUG),)
CFLAGS+= -g -D__CHUCK_DEBUG__
else
CFLAGS+= -O3
endif

# use 64-bit audio samples?
ifneq ($(USE_64_BIT_SAMPLE),)
CFLAGS+= -D__CHUCK_USE_64_BIT_SAMPLE__
endif

# strict error reporting?
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


######################## PLATFORM-SPECIFIC CONFIGS ############################

ifneq (,$(strip $(filter mac mac-ub osx bin-dist-mac,$(MAKECMDGOALS))))
include $(CK_CORE_DIR)/makefile.x/makefile.mac
endif

ifneq (,$(strip $(filter linux linux-all,$(MAKECMDGOALS))))
include $(CK_CORE_DIR)/makefile.x/makefile.linux
endif

ifneq (,$(strip $(filter linux-pulse,$(MAKECMDGOALS))))
include $(CK_CORE_DIR)/makefile.x/makefile.pulse
endif

ifneq (,$(strip $(filter linux-jack,$(MAKECMDGOALS))))
include $(CK_CORE_DIR)/makefile.x/makefile.jack
endif

ifneq (,$(strip $(filter linux-alsa,$(MAKECMDGOALS))))
include $(CK_CORE_DIR)/makefile.x/makefile.alsa
endif

# ifneq (,$(strip $(filter cygwin,$(MAKECMDGOALS))))
# include $(CK_CORE_DIR)/makefile.x/makefile.cygwin
# endif

# ifneq (,$(strip $(filter osx-rl,$(MAKECMDGOALS))))
# include $(CK_CORE_DIR)/makefile.x/makefile.rl
# endif


########################## CHUCK CORE LIB TARGETS #############################
CXXOBJS_CORE+= chuck.o chuck_absyn.o chuck_parse.o chuck_errmsg.o \
	chuck_frame.o chuck_symbol.o chuck_table.o \
	chuck_vm.o chuck_instr.o chuck_scan.o chuck_type.o chuck_emit.o \
	chuck_compile.o chuck_dl.o chuck_oo.o chuck_lang.o chuck_ugen.o \
	chuck_otf.o chuck_stats.o chuck_shell.o chuck_io.o \
	chuck_carrier.o chuck_globals.o hidio_sdl.o midiio_rtmidi.o \
	rtmidi.o ugen_osc.o ugen_filter.o ugen_stk.o ugen_xxx.o ulib_ai.o \
	ulib_doc.o ulib_machine.o ulib_math.o ulib_std.o ulib_opsc.o \
	util_buffers.o util_console.o util_hid.o util_platforms.o \
	util_opsc.o util_serial.o util_string.o util_thread.o \
	uana_xform.o uana_extract.o
COBJS_CORE+= chuck.tab.o chuck.yy.o util_math.o util_network.o util_raw.o \
	util_xforms.o
LO_COBJS_CORE+= lo/address.o lo/blob.o lo/bundle.o lo/message.o lo/method.o \
	lo/pattern_match.o lo/send.o lo/server.o lo/server_thread.o lo/timetag.o


############################ CHUCK HOST TARGETS ###############################
# host header include paths
CXXINC_HOST+= -I${RTAUDIO_DIR} -I${RTAUDIO_DIR}/include
# host source
CXXSRCS_HOST+= chuck_main.cpp chuck_audio.cpp chuck_console.cpp \
	${RTAUDIO_DIR}/RtAudio.cpp
# NB on some windows builds, ASIO src in RTAUDIO_DIR/include should be added


############################ OBJECT FILE TARGETS ##############################
# host c++ object files
CXXOBJS_HOST=$(addprefix $(CK_HOST_DIR)/,$(CXXSRCS_HOST:.cpp=.o))
# c object files
COBJS=$(COBJS_HOST) $(addprefix $(CK_CORE_DIR)/,$(COBJS_CORE))
# c++ object files
CXXOBJS=$(CXXOBJS_HOST) $(addprefix $(CK_CORE_DIR)/,$(CXXOBJS_CORE))
# liblo object files
LO_COBJS=$(addprefix $(CK_CORE_DIR)/,$(LO_COBJS_CORE))
# libsndfile objects files (if compiling locally)
SF_COBJS=$(addprefix $(CK_CORE_DIR)/,$(SF_CSRCS:.c=.o))
# sum of object files
OBJS=$(COBJS) $(CXXOBJS) $(LO_COBJS) $(SF_COBJS)


############################ ADDITIONAL FLAGS #################################
# for liblo headers
LO_CFLAGS=-DHAVE_CONFIG_H -I.

# remove -arch options
CFLAGSDEPEND=$(CFLAGS)

ifneq (,$(ARCHS))
ARCHOPTS=$(addprefix -arch ,$(ARCHS))
else
ARCHOPTS=
endif

# macOS universal binary
ifneq (,$(strip $(filter mac-ub,$(MAKECMDGOALS))))
ARCHOPTS=-arch arm64 -arch x86_64
ARCH_ARGS=MAC_UB=true
CFLAGS+=-D__MACOS_UB__
endif


############################ DISTRIBUTION INFO ################################
# release notes
NOTES=AUTHORS DEVELOPERS INSTALL LICENSE QUICKSTART README.md THANKS VERSIONS
# where to put the newly constructed distribution
DIST_DIR=chuck-$(CK_VERSION)
# root directory where the above is found
DIST_ROOT=..

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)


############################# MAIN COMPILATION ################################
chuck-core:
	@echo -------------
	@echo [chuck-core]: compiling...
	$(MAKE) $(MAKECMDGOALS) -C $(CK_CORE_DIR) $(ARCH_ARGS)
	@echo -------------

# host compiler flags
CFLAGS_HOST=${CFLAGS} -I${RTAUDIO_DIR}
CFLAGSDEPEND_HOST=${CFLAGSDEPEND} ${CFLAGS_HOST}

# comile chuck
chuck: chuck-core $(COBJS_HOST) $(CXXOBJS_HOST)
	$(LD) -o chuck $(OBJS) $(LDFLAGS) $(ARCHOPTS)

# build receipt for host c objects
$(COBJS_HOST): %.o: %.c
	$(CC) $(CFLAGS_HOST) $(ARCHOPTS) -c $< -o $@
	@$(CC) -MM -MQ "$@" $(CFLAGSDEPEND_HOST) $< > $*.d

# build receipt for host c++ objects
$(CXXOBJS_HOST): %.o: %.cpp
	$(CXX) $(CFLAGS_HOST) $(ARCHOPTS) -c $< -o $@
	@$(CXX) -MM -MQ "$@" $(CFLAGSDEPEND_HOST) $< > $*.d


############################## RUN UNIT TEST ##################################
test:
	(cd test && ./test.py ../chuck .)


############################### DISTRIBUTION ##################################
# distribution meta-targets
.PHONY: src-dist

# chuck source distribution
src-dist:
# clean out old dists (ASSUME: done from chuck)
	rm -rf $(DIST_DIR) $(DIST_DIR){.tgz,.zip}
# create directories
	mkdir $(DIST_DIR) $(DIST_DIR)/src $(DIST_DIR)/examples
# copy src
	git archive HEAD src | tar -x -C $(DIST_DIR)
# 	rm -r $(DIST_DIR)/src/test
# copy examples
	git archive HEAD examples | tar -x -C $(DIST_DIR)
# copy notes
	cp $(addprefix ./,$(NOTES)) $(DIST_DIR)
# tar/gzip
	tar czf $(DIST_DIR).tgz $(DIST_DIR)


########################## MANUAL INSTALL ###################################
# manually install chuck
DESTDIR?=/usr/local/bin

install:
	mkdir -p $(DESTDIR)
	cp $(wildcard chuck chuck.exe) $(DESTDIR)/
	chmod 755 $(DESTDIR)/$(wildcard chuck chuck.exe)

ifneq ($(CK_TARGET),)
.DEFAULT_GOAL:=$(CK_TARGET)
ifeq ($(MAKECMDGOALS),)
MAKECMDGOALS:=$(.DEFAULT_GOAL)
endif
endif


############################## CLEAN UP #######################################
clean:
	@echo "removing generated files..."
	@rm -rf $(wildcard chuck chuck.exe) *.o *.d */*.{o,d} */*/*.{o,d} \
	$(OBJS) $(patsubst %.o,%.d,$(OBJS))*~ $(CK_CORE_DIR)/chuck.output \
	$(CK_CORE_DIR)/chuck.tab.h $(CK_CORE_DIR)/chuck.tab.c $(CK_CORE_DIR)/chuck.yy.c \
	$(DIST_ROOT)/$(DIST_DIR) $(DIST_ROOT)/$(DIST_DIR){,.tgz,.zip} \
	host-web/webchuck/js/webchuck.js host-web/webchuck/js/webchuck.wasm \
	Release Debug

