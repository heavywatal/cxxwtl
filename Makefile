## Programs and Options
CXX := clang++
ifeq ($(strip $(shell which $(CXX))),)
  CXX := g++-4.8
endif

CXXFLAGS := -O3 -std=c++11
CPPFLAGS := -Wall -Wextra -fno-strict-aliasing -DNDEBUG
TARGET_ARCH := -march=core2 -m64 -msse -msse2 -msse3

ifeq (,$(findstring clang,$(CXX)))
  CXXFLAGS += -mfpmath=sse
else
  CXXFLAGS += -stdlib=libc++
endif

## Targets
.DEFAULT_GOAL := all
.PHONY: all clean

a.out: test.cpp
	$(LINK.cpp) -I$(includedir) $(OUTPUT_OPTION) $^ -L$(libdir) -lsfmt

all: a.out
	./$<

clean:
	$(RM) ${OBJS} ${LIBRARY}
