AUTOMAKE_OPTIONS = foreign

# contains unused vars, need to append it to CXXLFAGS to
# override the -Wall
CXXFLAGS += -Wno-unused

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -L$(ROOTSYS)/lib

BUILT_SOURCES = \
  testexternals.C

SUBDIRS = calib

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLMON_MAIN)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include 

lib_LTLIBRARIES = \
  libonlbbcmon.la

libonlbbcmon_la_LIBADD = \
  -lonlmonserver \
  -lonlmonclient \
  -lonlmondb \
  -lbbc \
  -lzdc \
  -lSpectrum

noinst_HEADERS = \
  BbcMon.h \
  BbcMonDraw.h

include_HEADERS =

libonlbbcmon_la_SOURCES = \
  BbcMon_Dict.C \
  BbcMon.C \
  BbcMonDraw.C

bin_SCRIPTS = \
  BbcSetup.csh

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlbbcmon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

BbcMon_Dict.C: BbcMon.h BbcMonDraw.h BbcMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
