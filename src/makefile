DESTDIR?=/usr/local/bin
COREDIR=core
HOSTDIR=host
CK_PLATFORM=


########################## DEFAULT MAKE TARGET #################################
# default target: print usage message and quit
current: 
	@echo "[chuck build]: please use one of the following configurations:"
	@echo "   make linux-alsa, make linux-jack, make linux-pulse,"
	@echo "   make osx, make osx-ub, make cygwin, or make win32"


############################## MAKE INSTALL ####################################
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


############################## MAKE INSTALL ####################################
.PHONY: osx linux-pulse linux-jack linux-alsa cygwin osx-rl test
osx linux-pulse linux-jack linux-alsa cygwin osx-rl: chuck

win32:
	make -f makefile.win32

CK_VERSION=1.3.6.0


########################### COMPILATION TOOLS ##################################
LEX=flex
YACC=bison
CC=gcc
CXX=g++
LD=g++


############################# COMPILER FLAGS ###################################
CFLAGS+=-I. -I$(COREDIR) -I$(COREDIR)/lo

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

ifneq ($(findstring arm,$(shell uname -m)),)
# some sort of arm platform- enable aggressive optimizations
CFLAGS+= -ffast-math
endif


######################### PLATFORM-SPECIFIC THINGS #############################
ifneq (,$(strip $(filter osx bin-dist-osx,$(MAKECMDGOALS))))
include $(COREDIR)/makefile.x/makefile.osx
endif

ifneq (,$(strip $(filter linux-pulse,$(MAKECMDGOALS))))
include makefile.pulse
endif

ifneq (,$(strip $(filter linux-jack,$(MAKECMDGOALS))))
include makefile.jack
endif

ifneq (,$(strip $(filter linux-alsa,$(MAKECMDGOALS))))
include makefile.alsa
endif

ifneq (,$(strip $(filter cygwin,$(MAKECMDGOALS))))
include makefile.cygwin
endif

ifneq (,$(strip $(filter osx-rl,$(MAKECMDGOALS))))
include makefile.rl
endif


########################## CHUCK CORE LIB TARGETS ##############################
COBJS_CORE+= chuck.tab.o chuck.yy.o util_math.o util_network.o util_raw.o \
	util_xforms.o
CXXOBJS_CORE+= chuck.o chuck_absyn.o chuck_parse.o chuck_errmsg.o \
	chuck_frame.o chuck_symbol.o chuck_table.o chuck_utils.o \
	chuck_vm.o chuck_instr.o chuck_scan.o chuck_type.o chuck_emit.o \
	chuck_compile.o chuck_dl.o chuck_oo.o chuck_lang.o chuck_ugen.o \
	chuck_otf.o chuck_stats.o chuck_shell.o chuck_io.o hidio_sdl.o \
	midiio_rtmidi.o rtmidi.o ugen_osc.o ugen_filter.o \
	ugen_stk.o ugen_xxx.o ulib_machine.o ulib_math.o ulib_std.o \
	ulib_opsc.o ulib_regex.o util_buffers.o util_console.o \
	util_string.o util_thread.o util_opsc.o util_serial.o \
	util_hid.o uana_xform.o uana_extract.o
LO_COBJS_CORE+= lo/address.o lo/blob.o lo/bundle.o lo/message.o lo/method.o \
	lo/pattern_match.o lo/send.o lo/server.o lo/server_thread.o lo/timetag.o


############################ CHUCK HOST TARGETS ################################
CXXSRCS_HOST+= chuck_main.cpp chuck_audio.cpp chuck_console.cpp RtAudio/RtAudio.cpp


############################ OBJECT FILE TARGETS ###############################
CXXOBJS_HOST=$(addprefix $(HOSTDIR)/,$(CXXSRCS_HOST:.cpp=.o))

COBJS=$(COBJS_HOST) $(addprefix $(COREDIR)/,$(COBJS_CORE))
CXXOBJS=$(CXXOBJS_HOST) $(addprefix $(COREDIR)/,$(CXXOBJS_CORE))
LO_COBJS=$(addprefix $(COREDIR)/,$(LO_COBJS_CORE))
SF_COBJS=$(addprefix $(COREDIR)/,$(SF_CSRCS:.c=.o))
OBJS=$(COBJS) $(CXXOBJS) $(LO_COBJS) $(SF_COBJS)


############################ ADDITIONAL FLAGS ##################################
# for liblo headers
LO_CFLAGS=-DHAVE_CONFIG_H -I.

# remove -arch options
CFLAGSDEPEND=$(CFLAGS)

ifneq (,$(ARCHS))
ARCHOPTS=$(addprefix -arch ,$(ARCHS))
else
ARCHOPTS=
endif


############################ DISTRIBUTION INFO #################################
NOTES=AUTHORS DEVELOPER PROGRAMMER README TODO COPYING INSTALL QUICKSTART \
 THANKS VERSIONS
BIN_NOTES=README.txt
DOC_NOTES=GOTO
DIST_DIR=chuck-$(CK_VERSION)
DIST_DIR_EXE=chuck-$(CK_VERSION)-exe
CK_SVN=https://chuck-dev.stanford.edu/svn/chuck/

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)


############################# MAIN COMPILATION #################################
chuck-core:
	@echo -------------
	@echo [chuck-core]: compiling...
	make $(MAKECMDGOALS) -C $(COREDIR)
	@echo -------------

chuck: chuck-core $(COBJS_HOST) $(CXXOBJS_HOST)
	$(LD) -o chuck $(OBJS) $(LDFLAGS) $(ARCHOPTS)

$(COBJS_HOST): %.o: %.c
	$(CC) $(CFLAGS) $(ARCHOPTS) -c $< -o $@
	@$(CC) -MM -MQ "$@" $(CFLAGSDEPEND) $< > $*.d

$(CXXOBJS_HOST): %.o: %.cpp
	$(CXX) $(CFLAGS) $(ARCHOPTS) -c $< -o $@
	@$(CXX) -MM -MQ "$@" $(CFLAGSDEPEND) $< > $*.d

clean: 
	@rm -rf $(wildcard chuck chuck.exe) *.o *.d $(OBJS) \
        $(patsubst %.o,%.d,$(OBJS)) *~ $(COREDIR)/chuck.output \
	$(COREDIR)/chuck.tab.h $(COREDIR)/chuck.tab.c \
        $(COREDIR)/chuck.yy.c $(DIST_DIR){,.tgz,.zip} Release Debug
	

############################### RUN TEST #######################################
test:
	pushd test; ./test.py ../chuck .; popd


############################### DISTRIBUTION ###################################
# ------------------------------------------------------------------------------
# Distribution meta-targets
# ------------------------------------------------------------------------------

.PHONY: bin-dist-osx
bin-dist-osx: osx
# clean out old dists
	-rm -rf $(DIST_DIR_EXE){,.tgz,.zip}
# create directories
	mkdir $(DIST_DIR_EXE) $(DIST_DIR_EXE)/bin $(DIST_DIR_EXE)/doc
# copy binary + notes
	cp chuck $(addprefix ../notes/bin/,$(BIN_NOTES)) $(DIST_DIR_EXE)/bin
# copy manual + notes
	cp ../doc/manual/ChucK_manual.pdf $(addprefix ../notes/doc/,$(DOC_NOTES)) $(DIST_DIR_EXE)/doc
# copy examples
	svn export $(CK_SVN)/trunk/src/examples $(DIST_DIR_EXE)/examples &> /dev/null
#cp -r examples $(DIST_DIR_EXE)/examples
# remove .svn directories
#-find $(DIST_DIR_EXE)/examples/ -name '.svn' -exec rm -rf '{}' \; &> /dev/null
# copy notes
	cp $(addprefix ../notes/,$(NOTES)) $(DIST_DIR_EXE)
# tar/gzip
	tar czf $(DIST_DIR_EXE).tgz $(DIST_DIR_EXE)

.PHONY: bin-dist-win32
bin-dist-win32:
#	make win32
# clean out old dists
	-rm -rf $(DIST_DIR_EXE){,.tgz,.zip}
# create directories
	mkdir $(DIST_DIR_EXE) $(DIST_DIR_EXE)/bin $(DIST_DIR_EXE)/doc
# copy binary + notes
	cp Release/chuck.exe $(addprefix ../notes/bin/,$(BIN_NOTES)) $(DIST_DIR_EXE)/bin
# copy manual + notes
	cp ../doc/manual/ChucK_manual.pdf $(addprefix ../notes/doc/,$(DOC_NOTES)) $(DIST_DIR_EXE)/doc
# copy examples
	svn export $(CK_SVN)/trunk/src/examples $(DIST_DIR_EXE)/examples &> /dev/null
#cp -r examples $(DIST_DIR_EXE)/examples
# remove .svn directories
#-find $(DIST_DIR_EXE)/examples/ -name '.svn' -exec rm -rf '{}' \; &> /dev/null
# copy notes
	cp $(addprefix ../notes/,$(NOTES)) $(DIST_DIR_EXE)
# tar/gzip
	zip -q -9 -r -m $(DIST_DIR_EXE).zip $(DIST_DIR_EXE)

.PHONY: src-dist
src-dist:
# clean out old dists
	rm -rf $(DIST_DIR) $(DIST_DIR){.tgz,.zip}
# create directories
	mkdir $(DIST_DIR) $(DIST_DIR)/doc $(DIST_DIR)/src $(DIST_DIR)/examples
# copy src
	git archive HEAD . | tar -x -C $(DIST_DIR)/src
	rm -r $(DIST_DIR)/src/examples $(DIST_DIR)/src/test 
# copy manual + notes
	cp $(addprefix ../notes/doc/,$(DOC_NOTES)) $(DIST_DIR)/doc
# copy examples
	git archive HEAD examples | tar -x -C $(DIST_DIR)/
	# svn export $(CK_SVN)/trunk/src/examples $(DIST_DIR)/examples 2>&1 > /dev/null
#cp -r examples $(DIST_DIR)/examples
# remove .svn directories
#-find $(DIST_DIR)/examples/ -name '.svn' -exec rm -rf '{}' \; &> /dev/null
# copy notes
	cp $(addprefix ../notes/,$(NOTES)) $(DIST_DIR)
# tar/gzip
	tar czf $(DIST_DIR).tgz $(DIST_DIR)
