CXX ?= g++
CXXFLAGS ?= -Wall -Wextra -Werror -std=c++17 -O0 -g
AR ?= ar
ARFLAGS ?= rcs

INCLUDES = -Iinclude

all: libmlpt.a mlpt_demo

mlpt.o: src/mlpt.cpp include/mlpt.h include/config.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

libmlpt.a: mlpt.o
	$(AR) $(ARFLAGS) $@ $<

mlpt_demo: examples/demo.cpp libmlpt.a
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -L. -lmlpt -o $@

clean:
	rm -f *.o libmlpt.a mlpt_demo

.PHONY: all clean
