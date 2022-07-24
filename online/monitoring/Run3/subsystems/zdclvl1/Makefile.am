AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
  testexternals.C

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLMON_MAIN)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include 

lib_LTLIBRARIES = \
  libonlzdclvl1mon.la

libonlzdclvl1mon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient

noinst_HEADERS = \
  ZdcLvl1Mon.h \
  ZdcLvl1MonDraw.h \
  ZDCLut.h \
  ZDCLl1.h 

include_HEADERS =

libonlzdclvl1mon_la_SOURCES = \
  ZdcLvl1Mon_Dict.C \
  ZdcLvl1Mon.C \
  ZdcLvl1MonDraw.C \
  ZDCLut.cc \
  ZDCLl1.C 

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlzdclvl1mon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

ZdcLvl1Mon_Dict.C: ZdcLvl1Mon.h ZdcLvl1MonDraw.h ZdcLvl1MonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
