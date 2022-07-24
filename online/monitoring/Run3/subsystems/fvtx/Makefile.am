AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
  testexternals.C

SUBDIRS = \
  calib

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLMON_MAIN)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(OPT_PHENIX)/include \
  -I$(ROOTSYS)/include 

lib_LTLIBRARIES = \
  libonlfvtxmon.la

libonlfvtxmon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient \
  -lfvtxgeom \
  -lfvtxoo_modules

noinst_HEADERS = \
  FvtxMon.h \
  FvtxMonConsts.h \
  FvtxMonDraw.h \
  FvtxOnlineChannel.hh 

include_HEADERS =

libonlfvtxmon_la_SOURCES = \
  FvtxMon_Dict.C \
  FvtxMon.C \
  FvtxMonDraw.C \
  FvtxOnlineChannel.cc 

bin_SCRIPTS = \
  FvtxSetup.csh

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlfvtxmon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

FvtxMon_Dict.C: FvtxMon.h FvtxMonDraw.h FvtxMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
