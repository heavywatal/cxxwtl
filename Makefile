## Directories and Files
DSFMTDIR := ${HOME}/Downloads/dSFMT-src-2.2.1
SFMTDIR := ${HOME}/Downloads/SFMT-src-1.4
MEXP := 19937
LIBRARY := libsfmt.a

OBJS := dSFMT.o SFMT.o
HEADERS := $(wildcard ${DSFMTDIR}/*.h) $(wildcard ${SFMTDIR}/*.h)

prefix := ${HOME}/local
exec_prefix := $(prefix)
includedir := $(prefix)/include/sfmt
libdir := $(exec_prefix)/lib


## Programs and Options
CXX := clang
ifeq ($(strip $(shell which $(CXX))),)
  CXX := g++-4.8
endif

CXXFLAGS := -O3
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
.PHONY: all clean install

dSFMT.o: ${DSFMTDIR}/dSFMT.c
	$(COMPILE.cpp) -DDSFMT_MEXP=${MEXP} $(OUTPUT_OPTION) $<

SFMT.o: ${SFMTDIR}/SFMT.c
	$(COMPILE.cpp) -DSFMT_MEXP=${MEXP} $(OUTPUT_OPTION) $<

all: ${LIBRARY}
	@:

clean:
	$(RM) ${OBJS} ${LIBRARY}

install: install-lib install-include
	@:

${LIBRARY}: dSFMT.o SFMT.o
	$(AR) ${ARFLAGS} $@ $^
	$(AR) tv $@

install-lib: ${LIBRARY} | $(DESTDIR)$(libdir)
	$(INSTALL_DATA) $< $(DESTDIR)$(libdir)/$<

install-include: ${HEADERS} | $(DESTDIR)$(includedir)
	for i in ${HEADERS}; do $(INSTALL_DATA) $$i $(DESTDIR)$(includedir)/$$(basename $$i); done

$(DESTDIR)$(libdir):
	mkdir -p $@

$(DESTDIR)$(includedir):
	mkdir -p $@
