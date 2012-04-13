
# faust file
FAUST_DSP?=fmv.dsp
FAUST_CPP=$(addsuffix .cpp,$(basename $(FAUST_DSP)))

# chugin name
CHUGIN_NAME?=$(addprefix Faust,$(basename $(FAUST_DSP)))

# all of the c/cpp files that compose this chugin
MODULES=$(FAUST_CPP)

# where the chuck source code is
CK_SRC_PATH=../../src

# you won't generally need to change anything below this line for a new chugin

# default: build a dynamic chugin
CK_CHUGIN_STATIC?=0

CHUGIN_PATH=/usr/lib/chuck

CFLAGS=-m32 -I$(CK_SRC_PATH) -I. -D__CHUGIN_NAME__=$(CHUGIN_NAME)
LDFLAGS=-m32 -lstdc++

ifeq ($(CK_CHUGIN_STATIC),0)
SUFFIX=.chug
else
SUFFIX=.schug
CFLAGS+= -D__CK_DLL_STATIC__
endif

ifneq ($(CHUCK_DEBUG),)
CFLAGS+= -g
else
CFLAGS+= -O3
endif

OBJECTS=$(addsuffix .o,$(basename $(MODULES)))
CHUG=$(addsuffix $(SUFFIX),$(CHUGIN_NAME))

.PHONY: osx linux linux-oss linux-jack linux-alsa win32
osx linux linux-oss linux-jack linux-alsa win32: all

ifneq (,$(strip $(filter osx,$(MAKECMDGOALS))))
include makefile.osx
endif

ifneq (,$(strip $(filter linux linux-oss linux-jack linux-alsa,$(MAKECMDGOALS))))
include makefile.linux
endif

ifneq (,$(strip $(filter win32,$(MAKECMDGOALS))))
include makefile.win32
endif

all: $(CHUG)

$(CHUG): $(OBJECTS)
ifeq ($(CK_CHUGIN_STATIC),0)
	gcc $(LDFLAGS) -o $@ $?
else
	ar rv $@ $?
	ranlib $@
endif

$(OBJECTS): %.o: %.cpp $(CK_SRC_PATH)/chuck_dl.h
	gcc $(CFLAGS) -c -o $@ $< 

$(FAUST_CPP): %.cpp: %.dsp fauck.cpp
	faust -a fauck.cpp -o $(FAUST_CPP) $(FAUST_DSP)

install: $(CHUG)
	mkdir -p $(CHUGIN_PATH)
	cp $^ $(CHUGIN_PATH)

clean: 
	rm -rf $(OBJECTS) $(CHUG)
