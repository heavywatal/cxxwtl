## Directories and Files
DSFMT := dSFMT-src-2.2.1
SFMT := SFMT-src-1.4
MEXP := 19937
LIBRARY := libsfmt.a
SRCDIR := ${HOME}/Downloads
URL_PREFIX := http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/SFMT

OBJS := dSFMT.o SFMT.o
HEADERS := $(wildcard ${SRCDIR}/${DSFMT}/*.h) $(wildcard ${SRCDIR}/${DSFMT}/*.h)

prefix := ${HOME}/local
exec_prefix := $(prefix)
includedir := $(prefix)/include
libdir := $(exec_prefix)/lib


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

ARFLAGS := rsv

INSTALL := install -C
INSTALL_PROGRAM := $(INSTALL)
INSTALL_DATA := $(INSTALL) -m 644


## Targets
.DEFAULT_GOAL := all
.PHONY: all clean install download

a.out: test.cpp
	$(LINK.cpp) -I$(includedir) $(OUTPUT_OPTION) $^ -L$(libdir) -lsfmt

test: a.out
	./$<

all: ${LIBRARY}
	@:

dSFMT.o: ${SRCDIR}/${DSFMT}/dSFMT.c
	$(COMPILE.cpp) -DDSFMT_MEXP=${MEXP} $(OUTPUT_OPTION) $<

SFMT.o: ${SRCDIR}/${SFMT}/SFMT.c
	$(COMPILE.cpp) -DSFMT_MEXP=${MEXP} $(OUTPUT_OPTION) $<

download:
	wget -O - ${URL_PREFIX}/${DSFMT}.tar.gz | tar xzf - -C ${SRCDIR}
	wget -O - ${URL_PREFIX}/${SFMT}.tar.gz | tar xzf - -C ${SRCDIR}

clean:
	$(RM) ${OBJS} ${LIBRARY}

install: install-lib install-include
	@:

${LIBRARY}: dSFMT.o SFMT.o
	$(AR) ${ARFLAGS} $@ $^
	$(AR) tv $@

install-lib: ${LIBRARY} | $(DESTDIR)$(libdir)
	$(INSTALL_DATA) $< $(DESTDIR)$(libdir)/$<

install-include: ${HEADERS} | $(DESTDIR)$(includedir)/sfmt
	for i in ${HEADERS}; do $(INSTALL_DATA) $$i $(DESTDIR)$(includedir)/sfmt/$$(basename $$i); done

$(DESTDIR)$(libdir):
	mkdir -p $@

$(DESTDIR)$(includedir):
	mkdir -p $@
