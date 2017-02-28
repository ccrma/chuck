
CPP=g++
LD=g++

CFLAGS+=
CPPFLAGS+=$(CFLAGS) -Istk
LDFLAGS+=

CPPMODULES=audiocmp
CPPOBJECTS=$(addsuffix .o,$(CPPMODULES))

STK_CPPMODULES=FileRead FileWvIn OnePole Stk
STK_CPPOBJECTS=$(addprefix stk/,$(addsuffix .o,$(STK_CPPMODULES)))

default: audiocmp

audiocmp: $(CPPOBJECTS) $(STK_CPPOBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

$(CPPOBJECTS): %.o: %.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ $<

$(STK_CPPOBJECTS): %.o: %.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ $<

clean:
	rm -f $(CPPOBJECTS) $(STK_CPPOBJECTS)
