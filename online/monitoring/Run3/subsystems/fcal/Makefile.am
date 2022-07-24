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
  libonlfcalmon.la

libonlfcalmon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient \
  -lbbc \
  -lzdc \
  -lfcl

noinst_HEADERS = \
  FcalMon.h 

include_HEADERS = 

libonlfcalmon_la_SOURCES = \
  FcalMon_Dict.C \
  FcalMonDraw.C \
  FcalMon.C

bin_SCRIPTS = \
  FcalSetup.csh

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlfcalmon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

FcalMon_Dict.C: FcalMon.h FcalMonDraw.h FcalMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*




