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
  libonlrichertmon.la \
  libonlrichmon.la

libonlrichmon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib \
  libonlrichertmon.la \
  -lonlmonserver \
  -lonlmonclient \
  -lpdbcalBase

libonlrichertmon_la_LIBADD = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lpdbcalBase

include_HEADERS = \
  CalibrationsRead.h \
  CrkCal.h \
  GetRichPmtData.h \
  RichGeom.h \
  RICHMonDefs.h

libonlrichmon_la_SOURCES = \
  RICHMon_Dict.C \
  RICHMon.C \
  RICHMonDraw.C

libonlrichertmon_la_SOURCES = \
  CalibrationsRead.C \
  TacT0Read.C \
  GetRichPmtData.C \
  RichGeom.C \
  CrkCal.cc

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlrichmon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

RICHMon_Dict.C: RICHMon.h RICHMonDraw.h RICHMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
