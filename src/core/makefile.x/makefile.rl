
CXX=gcc
CXX_LINK=g++
LEX=flex
YACC=bison
INCLUDES=
FLAGS=-D__MACOSX_CORE__ -D__USE_READLINE__ -O3 -c
LIBS=-framework CoreAudio -framework CoreMIDI -framework CoreFoundation -lstdc++ -lm -lreadline
SF_OBJ=util_sndfile.o

ifneq ($(CHUCK_STAT),)
FLAGS+= -D__CHUCK_STAT_TRACK__
endif

ifneq ($(CHUCK_DEBUG),)
FLAGS+= -g
endif

ifneq ($(CHUCK_STRICT),)
FLAGS+= -Wall
endif

#-----------------------------------------------------------------------------
# by default, ChucK uses a pre-configured libsndfile...
# uncomment the next 3 lines to use libsndfile on your system
#-----------------------------------------------------------------------------
#FLAGS+= -D__CK_SNDFILE_NATIVE__
#LIBS+= -lsndfile
#SF_OBJ=

OBJS=   chuck.tab.o chuck.yy.o chuck_absyn.o chuck_parse.o chuck_errmsg.o \
	chuck_frame.o chuck_symbol.o chuck_table.o chuck_utils.o \
	chuck_vm.o chuck_instr.o chuck_scan.o chuck_type.o chuck_emit.o \
	chuck_compile.o chuck_dl.o chuck_oo.o chuck_lang.o chuck_ugen.o \
	chuck_main.o chuck_otf.o chuck_stats.o chuck_bbq.o chuck_shell.o \
	chuck_console.o chuck_globals.o digiio_rtaudio.o midiio_rtmidi.o \
	rtaudio.o rtmidi.o ugen_osc.o ugen_filter.o ugen_stk.o ugen_xxx.o \
	ulib_machine.o ulib_math.o ulib_std.o ulib_opsc.o util_buffers.o \
	util_console.o util_math.o util_network.o util_raw.o \
	util_string.o util_thread.o util_xforms.o util_opsc.o $(SF_OBJ)

chuck: $(OBJS)
	$(CXX_LINK) -o chuck $(OBJS) $(LIBS)

chuck.tab.o: chuck.tab.c
	$(CXX) $(FLAGS) chuck.tab.c

chuck.tab.c: chuck.y
	$(YACC) -dv -b chuck chuck.y

chuck.tab.h: chuck.tab.c
	echo "chuck.tab.h was created at the same time as chuck.tab.c"

chuck.yy.o: chuck.yy.c chuck.tab.h chuck_errmsg.h chuck_utils.h
	$(CXX) $(FLAGS) chuck.yy.c

chuck.yy.c: chuck.lex
	$(LEX) -ochuck.yy.c chuck.lex

chuck_absyn.o: chuck_absyn.cpp chuck_absyn.h chuck_utils.h chuck_symbol.h
	$(CXX) $(FLAGS) chuck_absyn.cpp

chuck_parse.o: chuck_parse.h chuck_parse.cpp
	$(CXX) $(FLAGS) chuck_parse.cpp

chuck_scan.o: chuck_type.h chuck_scan.h chuck_scan.cpp
	$(CXX) $(FLAGS) chuck_scan.cpp

chuck_type.o: chuck_type.cpp chuck_type.h chuck_absyn.h chuck_oo.h chuck_vm.h chuck_instr.h
	$(CXX) $(FLAGS) chuck_type.cpp

chuck_emit.o: chuck_emit.cpp chuck_emit.h chuck_type.h chuck_instr.h chuck_vm.h
	$(CXX) $(FLAGS) chuck_emit.cpp

chuck_compile.o: chuck_compile.h chuck_compile.cpp
	$(CXX) $(FLAGS) chuck_compile.cpp

chuck_symbol.o: chuck_symbol.cpp chuck_symbol.h chuck_table.h chuck_utils.h
	$(CXX) $(FLAGS) chuck_symbol.cpp

chuck_frame.o: chuck_frame.h chuck_frame.cpp
	$(CXX) $(FLAGS) chuck_frame.cpp

chuck_errmsg.o: chuck_errmsg.cpp chuck_errmsg.h chuck_utils.h
	$(CXX) $(FLAGS) chuck_errmsg.cpp

chuck_utils.o: chuck_utils.cpp chuck_utils.h
	$(CXX) $(FLAGS) chuck_utils.cpp

chuck_table.o: chuck_table.cpp chuck_table.h chuck_utils.h
	$(CXX) $(FLAGS) chuck_table.cpp

chuck_instr.o: chuck_instr.h chuck_instr.cpp chuck_type.h chuck_vm.h
	$(CXX) $(FLAGS) chuck_instr.cpp

chuck_vm.o: chuck_vm.h chuck_vm.cpp chuck_instr.h chuck_oo.h chuck_type.h
	$(CXX) $(FLAGS) chuck_vm.cpp

chuck_oo.o: chuck_oo.h chuck_oo.cpp
	$(CXX) $(FLAGS) chuck_oo.cpp

chuck_lang.o: chuck_lang.h chuck_lang.cpp chuck_oo.h
	$(CXX) $(FLAGS) chuck_lang.cpp

chuck_ugen.o: chuck_ugen.cpp chuck_ugen.h chuck_def.h
	$(CXX) $(FLAGS) chuck_ugen.cpp

chuck_dl.o: chuck_dl.h chuck_dl.cpp
	$(CXX) $(FLAGS) chuck_dl.cpp

chuck_otf.o: chuck_otf.h chuck_otf.cpp
	$(CXX) $(FLAGS) chuck_otf.cpp

chuck_stats.o: chuck_stats.h chuck_stats.cpp
	$(CXX) $(FLAGS) chuck_stats.cpp

chuck_bbq.o: chuck_bbq.cpp chuck_bbq.h midiio_rtmidi.h
	$(CXX) $(FLAGS) chuck_bbq.cpp

chuck_shell.o: chuck_shell.h chuck_shell.cpp
	$(CXX) $(FLAGS) chuck_shell.cpp

chuck_console.o: chuck_console.h chuck_console.cpp
	$(CXX) $(FLAGS) chuck_console.cpp

chuck_globals.o: chuck_globals.h chuck_globals.cpp
	$(CXX) $(FLAGS) chuck_globals.cpp

chuck_main.o: chuck_main.cpp chuck_absyn.h chuck_type.h chuck_emit.h chuck_vm.h chuck_errmsg.h chuck_utils.h
	$(CXX) $(FLAGS) chuck_main.cpp

digiio_rtaudio.o: digiio_rtaudio.h digiio_rtaudio.cpp rtaudio.h
	$(CXX) $(FLAGS) digiio_rtaudio.cpp

midiio_rtmidi.o: midiio_rtmidi.h midiio_rtmidi.cpp
	$(CXX) $(FLAGS) midiio_rtmidi.cpp

rtaudio.o: rtaudio.h rtaudio.cpp rterror.h
	$(CXX) $(FLAGS) rtaudio.cpp

rtmidi.o: rtmidi.h rtmidi.cpp
	$(CXX) $(FLAGS) rtmidi.cpp

ugen_osc.o: ugen_osc.h ugen_osc.cpp
	$(CXX) $(FLAGS) ugen_osc.cpp

ugen_xxx.o: ugen_xxx.h ugen_xxx.cpp
	$(CXX) $(FLAGS) ugen_xxx.cpp
    
ugen_filter.o: ugen_filter.h ugen_filter.cpp
	$(CXX) $(FLAGS) ugen_filter.cpp

ugen_stk.o: ugen_stk.h ugen_stk.cpp
	$(CXX) $(FLAGS) ugen_stk.cpp

ulib_machine.o: ulib_machine.h ulib_machine.cpp
	$(CXX) $(FLAGS) ulib_machine.cpp

ulib_math.o: ulib_math.h ulib_math.cpp
	$(CXX) $(FLAGS) ulib_math.cpp

ulib_net.o: ulib_net.h ulib_net.cpp
	$(CXX) $(FLAGS) ulib_net.cpp

ulib_std.o: ulib_std.h ulib_std.cpp
	$(CXX) $(FLAGS) ulib_std.cpp

ulib_opsc.o: ulib_opsc.h ulib_opsc.cpp
	$(CXX) $(FLAGS) ulib_opsc.cpp

util_buffers.o: util_buffers.h util_buffers.cpp
	$(CXX) $(FLAGS) util_buffers.cpp

util_console.o: util_console.h util_console.cpp
	$(CXX) $(FLAGS) util_console.cpp

util_math.o: util_math.h util_math.c
	$(CXX) $(FLAGS) util_math.c

util_network.o: util_network.h util_network.c
	$(CXX) $(FLAGS) util_network.c

util_opsc.o: util_opsc.h util_opsc.cpp
	$(CXX) $(FLAGS) util_opsc.cpp

util_raw.o: util_raw.h util_raw.c
	$(CXX) $(FLAGS) util_raw.c

util_string.o: util_string.h util_string.cpp
	$(CXX) $(FLAGS) util_string.cpp

util_thread.o: util_thread.h util_thread.cpp
	$(CXX) $(FLAGS) util_thread.cpp

util_xforms.o: util_xforms.h util_xforms.c
	$(CXX) $(FLAGS) util_xforms.c

util_sndfile.o: util_sndfile.h util_sndfile.c
	$(CXX) $(FLAGS) util_sndfile.c

clean: 
	rm -f chuck *~ *.o chuck.output chuck.tab.h chuck.tab.c chuck.yy.c

