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
  libonlzdcmon.la

libonlzdcmon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient \
  -lonlmondb \
  -lzdc \
  -lbbc

noinst_HEADERS = \
  ZdcMon.h \
  ZdcMonDraw.h

include_HEADERS =


libonlzdcmon_la_SOURCES = \
  ZdcMon_Dict.C \
  ZdcMon.C \
  ZdcMonDraw.C

bin_SCRIPTS = \
  ZdcSetup.csh

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlzdcmon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

ZdcMon_Dict.C: ZdcMon.h ZdcMonDraw.h ZdcMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
