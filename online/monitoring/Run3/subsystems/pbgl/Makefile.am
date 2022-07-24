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
  libonlpbglumon.la

libonlpbglumon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient \
  -lodbc++

noinst_HEADERS = \
  PbGlUMon.h \
  PbGlUMonDraw.h 

include_HEADERS =   

libonlpbglumon_la_SOURCES = \
  PbGlUDBodbc.C \
  PbGlUMon_Dict.C \
  PbGlUMon.C \
  PbGlUMonDraw.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlpbglumon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

PbGlUMon_Dict.C: PbGlUMon.h PbGlUMonDraw.h PbGlUMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
