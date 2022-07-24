AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
  testexternals.C

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLMON_MAIN)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include 

lib_LTLIBRARIES = \
  libonlmuidlvl1mon.la

libonlmuidlvl1mon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient \
  -lMuidTriggerEmulator

noinst_HEADERS = \
  MuidLvl1Mon.h \
  MuidLvl1MonDraw.h \
  MuidLvl1MonDefs.h

include_HEADERS =

libonlmuidlvl1mon_la_SOURCES = \
  MuidLvl1Mon_Dict.C \
  MuidLvl1Mon.C \
  MuidLvl1MonDraw.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlmuidlvl1mon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

MuidLvl1Mon_Dict.C: MuidLvl1Mon.h MuidLvl1MonDraw.h MuidLvl1MonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
