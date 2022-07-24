AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
  testexternals.C

SUBDIRS = calib

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLMON_MAIN)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include 

lib_LTLIBRARIES = \
  libonlmutrmon.la

libonlmutrmon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient \
  -lmutgeom

noinst_HEADERS = \
  MutrMon.h \
  MutrMonConsts.h \
  MutrMonDrawConsts.h \
  MutOnlineChannel.hh \
  MutrMonDraw.h

include_HEADERS =

libonlmutrmon_la_SOURCES = \
  MutrMon_Dict.C \
  MutrMon.C \
  MutOnlineChannel.cc \
  MutrMonDraw.C

bin_SCRIPTS = \
  MutrSetup.csh

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlmutrmon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

MutrMon_Dict.C: MutrMon.h MutrMonDraw.h MutrMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
