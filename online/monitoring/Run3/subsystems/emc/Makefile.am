AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
  testexternals.C

SUBDIRS = \
  conf \
  ref

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLMON_MAIN)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include 

lib_LTLIBRARIES = \
  libonlemcanalys.la \
  libonlemcalmon.la

libonlemcalmon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  libonlemcanalys.la \
  -lonlmonserver \
  -lonlmonclient

libonlemcanalys_la_LIBADD = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lbbc \
  -lzdc \
  -lemc

noinst_HEADERS = \
  EMCalMon.h \
  EMCalMonDraw.h \
  EmcClusterReco.h \
  EmcAnalys.h

include_HEADERS =   

libonlemcalmon_la_SOURCES = \
  EMCalMon_Dict.C \
  EMCalMon.C \
  EMCalMonDraw.C

libonlemcanalys_la_SOURCES = \
EmcClusterReco.C \
EmcAnalys.C 

bin_SCRIPTS = \
  EmcSetup.csh

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlemcanalys.la \
  libonlemcalmon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

EMCalMon_Dict.C: EMCalMon.h EMCalMonDraw.h EMCalMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
