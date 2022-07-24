AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
  testexternals.C

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLMON_MAIN)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include 

lib_LTLIBRARIES = \
  libonltofwmon.la

libonltofwmon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient \
  -ltof \
  -lpdbcalBase

noinst_HEADERS = \
  TOFWMon.h 

include_HEADERS =

libonltofwmon_la_SOURCES = \
  TOFWMon_Dict.C \
  TOFWMonDraw.C \
  TOFWMon.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonltofwmon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

TOFWMon_Dict.C: TOFWMon.h TOFWMonDraw.h TOFWMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
