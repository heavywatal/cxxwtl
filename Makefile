## Programs and Options
CXXFLAGS := -O3 -std=c++11 -isystem /usr/local/include -mfpmath=sse
CPPFLAGS := -Wall -Wextra -fno-strict-aliasing -pthread
TARGET_ARCH := -march=core2 -m64 -msse -msse2 -msse3
LDFLAGS := -L/usr/local/lib
LDLIBS := -lsfmt

ifeq ($(shell type clang++ >/dev/null && echo TRUE),TRUE)
  CXX := clang++
endif

ifneq (,$(findstring clang,$(CXX)))
  CXXFLAGS += -stdlib=libc++
endif

## Targets
.DEFAULT_GOAL := all
.PHONY: all clean

a.out: test.cpp
	$(LINK.cpp) $(OUTPUT_OPTION) $^ $(LDLIBS)

all: a.out
	./$<

clean:
	$(RM) a.out
