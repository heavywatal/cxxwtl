## Programs and Options
CXXFLAGS := -O3 -std=c++14 -I${HOME}/local/include -mfpmath=sse
CPPFLAGS := -Wall -Wextra -pthread
TARGET_ARCH := -m64 -msse -msse2 -msse3
LDFLAGS := -L${HOME}/local/lib
LDLIBS := -lsfmt

## Targets
.DEFAULT_GOAL := all
.PHONY: all clean

a.out: test.cpp $(wildcard *.hpp)
	$(LINK.cpp) $(OUTPUT_OPTION) $< $(LDLIBS)

all: a.out
	./$<

clean:
	$(RM) a.out
