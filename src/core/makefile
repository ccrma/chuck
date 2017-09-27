# default target: print usage message and quit
current: 
	@echo "[chuck build]: please use one of the following configurations:"
	@echo "   make linux-alsa, make linux-jack, make linux-pulse,"
	@echo "   make osx, make osx-ub, make cygwin, or make win32"

.PHONY: osx linux-pulse linux-jack linux-alsa cygwin osx-rl test
osx linux-pulse linux-jack linux-alsa cygwin osx-rl: chuck-core

win32:
	make -f makefile.win32

CK_VERSION=1.3.6.0

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

ifneq ($(findstring arm,$(shell uname -m)),)
# some sort of arm platform- enable aggressive optimizations
CFLAGS+= -ffast-math
endif

ifneq (,$(strip $(filter osx bin-dist-osx,$(MAKECMDGOALS))))
include makefile.x/makefile.osx
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
CSRCS_CORE+= chuck.tab.c chuck.yy.c util_math.c util_network.c util_raw.c \
	util_xforms.c
CXXSRCS_CORE+= chuck_absyn.cpp chuck_parse.cpp chuck_errmsg.cpp \
	chuck_frame.cpp chuck_symbol.cpp chuck_table.cpp chuck_utils.cpp \
	chuck_vm.cpp chuck_instr.cpp chuck_scan.cpp chuck_type.cpp \
	chuck_emit.cpp chuck_compile.cpp chuck_dl.cpp chuck_oo.cpp \
	chuck_lang.cpp chuck_ugen.cpp chuck_otf.cpp chuck_stats.cpp \
	chuck_shell.cpp chuck_io.cpp hidio_sdl.cpp chuck.cpp \
	midiio_rtmidi.cpp rtmidi.cpp ugen_osc.cpp ugen_filter.cpp \
	ugen_stk.cpp ugen_xxx.cpp ulib_machine.cpp ulib_math.cpp ulib_std.cpp \
	ulib_opsc.cpp ulib_regex.cpp util_buffers.cpp util_console.cpp \
	util_string.cpp util_thread.cpp util_opsc.cpp util_serial.cpp \
	util_hid.cpp uana_xform.cpp uana_extract.cpp
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

NOTES=AUTHORS DEVELOPER PROGRAMMER README TODO COPYING INSTALL QUICKSTART \
 THANKS VERSIONS
BIN_NOTES=README.txt
DOC_NOTES=GOTO
DIST_DIR=chuck-$(CK_VERSION)
DIST_DIR_EXE=chuck-$(CK_VERSION)-exe
CK_SVN=https://chuck-dev.stanford.edu/svn/chuck/

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

chuck-core: $(OBJS)
	@echo '[chuck-core]: all components built...'

chuck.tab.c chuck.tab.h: chuck.y
	$(YACC) -dv -b chuck chuck.y

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


