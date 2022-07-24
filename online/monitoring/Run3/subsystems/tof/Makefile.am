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
  libonltofmon.la

libonltofmon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient \
  -ltof \
  -lpdbcalBase

noinst_HEADERS = \
  TOFMon.h

include_HEADERS =

libonltofmon_la_SOURCES = \
  TOFMon_Dict.C \
  TOFMonDraw.C \
  TOFMon.C

bin_SCRIPTS = \
  TofSetup.csh

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonltofmon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

TOFMon_Dict.C: TOFMon.h TOFMonDraw.h TOFMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
