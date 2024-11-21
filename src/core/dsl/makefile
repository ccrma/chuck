CXX ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2
PROTOBUF_CFLAGS := $(shell pkg-config --cflags protobuf)
PROTOBUF_LIBS := $(shell pkg-config --libs protobuf)

TARGET := dsl_dump
CPP_SOURCES := dsl_toml.cpp dsl_dump_main.cpp
PROTO_SOURCES := piece.pb.cc
OBJECTS := $(CPP_SOURCES:.cpp=.o) $(PROTO_SOURCES:.cc=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(PROTOBUF_LIBS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(PROTOBUF_CFLAGS) -I. -c $< -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(PROTOBUF_CFLAGS) -I. -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)
