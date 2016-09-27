## Programs and Options
CXXFLAGS := -O3 -std=c++14 -isystem ${HOME}/local/include -mfpmath=sse
CPPFLAGS := -Wall -Wextra -fno-strict-aliasing -pthread
TARGET_ARCH := -m64 -msse -msse2 -msse3
LDFLAGS := -L${HOME}/local/lib
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

a.out: test.cpp $(wildcard *.hpp)
	$(LINK.cpp) $(OUTPUT_OPTION) $< $(LDLIBS)

all: a.out
	./$<

clean:
	$(RM) a.out
