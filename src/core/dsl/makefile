CXX ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2
PROTOBUF_CFLAGS := $(shell pkg-config --cflags protobuf)
PROTOBUF_LIBS := $(shell pkg-config --libs protobuf)

COMMON_CPP := dsl_toml.cpp dsl_codegen.cpp
PROTO_SOURCES := piece.pb.cc
COMMON_OBJS := $(COMMON_CPP:.cpp=.o)
PROTO_OBJS := $(PROTO_SOURCES:.cc=.o)

TARGETS := dsl_dump dsl_codegen

.PHONY: all clean

all: $(TARGETS)

dsl_dump: $(COMMON_OBJS) $(PROTO_OBJS) dsl_dump_main.o
	$(CXX) $(CXXFLAGS) $^ $(PROTOBUF_LIBS) -o $@

dsl_codegen: $(COMMON_OBJS) $(PROTO_OBJS) dsl_codegen_main.o
	$(CXX) $(CXXFLAGS) $^ $(PROTOBUF_LIBS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(PROTOBUF_CFLAGS) -I. -c $< -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(PROTOBUF_CFLAGS) -I. -c $< -o $@

clean:
	rm -f $(TARGETS) $(COMMON_OBJS) $(PROTO_OBJS) dsl_dump_main.o dsl_codegen_main.o
