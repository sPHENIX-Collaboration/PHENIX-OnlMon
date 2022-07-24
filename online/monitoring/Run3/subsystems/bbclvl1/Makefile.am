AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
  testexternals.C

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLMON_MAIN)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include 

lib_LTLIBRARIES = \
  libonlbbclvl1mon.la

libonlbbclvl1mon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient

noinst_HEADERS = \
  BbcLUT.h \
  BbcLl1.h \
  BbcLvl1Mon.h \
  BbcLvl1MonDraw.h

include_HEADERS =

libonlbbclvl1mon_la_SOURCES = \
  BbcLUT.cc \
  BbcLl1.C \
  BbcLvl1Mon_Dict.C \
  BbcLvl1Mon.C \
  BbcLvl1MonDraw.C 

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlbbclvl1mon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

BbcLvl1Mon_Dict.C: BbcLvl1Mon.h BbcLvl1MonDraw.h BbcLvl1MonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
