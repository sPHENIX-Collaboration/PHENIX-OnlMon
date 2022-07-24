AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
  testexternals.C

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLMON_MAIN)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(OPT_PHENIX)/include \
  -I$(ROOTSYS)/include 

lib_LTLIBRARIES = \
  libonlpktsizemon.la

libonlpktsizemon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient \
  -lonlmondb \
  -lodbc++

include_HEADERS =

libonlpktsizemon_la_SOURCES = \
  PktSizeCommon.C \
  PktSizeDBodbc.C \
  PktSizeMon.C \
  PktSizeMonDraw.C \
  PktSizeMonDraw_Dict.C \
  PktSizeMon_Dict.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlpktsizemon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

PktSizeMon_Dict.C: PktSizeMon.h PktSizeDBodbc.h PktSizeMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

PktSizeMonDraw_Dict.C: PktSizeMonDraw.h PktSizeMonDrawLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^


clean-local:
	rm -f *Dict.*
