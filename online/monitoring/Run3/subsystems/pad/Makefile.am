AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
  testexternals.C

SUBDIRS = \
  calib

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLMON_MAIN)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include 

lib_LTLIBRARIES = \
  libonlpadmon.la

libonlpadmon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient \
  -lgea \
  -lpad

noinst_HEADERS = \
  PadMon.h

include_HEADERS =

libonlpadmon_la_SOURCES = \
  PadMon_Dict.C \
  PadOnlineRecFEMBasic.C \
  PadMonDraw.C \
  PadMon.C

bin_SCRIPTS = \
  PadSetup.csh

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlpadmon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

PadMon_Dict.C: PadMon.h PadMonDraw.h PadOnlineRecFEMBasic.h PadMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
