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
  libonlaccmon.la

libonlaccmon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient

noinst_HEADERS = \
  ACCMon.h \
  ACCMonDefs.h \
  ACCMonDraw.h \
  CalibrationsRead.h \
  GetAccPmtData.h \
  AccGeom.h \
  ACCMonDefs.h 

libonlaccmon_la_SOURCES = \
  CalibrationsRead.C \
  ACCMon_Dict.C \
  ACCMon.C \
  GetAccPmtData.C \
  AccGeom.C \
  ACCMonDraw.C 

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlaccmon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

ACCMon_Dict.C: ACCMon.h ACCMonDraw.h ACCMonLinkDef.h
	rootcint -f $@ -c $(DEFAUL_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
