AUTOMAKE_OPTIONS = foreign

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib

BUILT_SOURCES = \
  testexternals.C

SUBDIRS = \
  calib

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLMON_MAIN)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(OPT_PHENIX)/include \
  -isystem $(ROOTSYS)/include

RINCLUDES = \
  -I$(includedir) \
  -I$(ONLMON_MAIN)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(OPT_PHENIX)/include \
  -I$(ROOTSYS)/include

lib_LTLIBRARIES = \
  libonlertmon.la

libonlertmon_la_LIBADD = \
  -L$(OPT_PHENIX)/lib \
  -lonlmonserver \
  -lonlmonclient \
  -lodbc++


noinst_HEADERS = \
  EMCalRichDecodeOnline.h \
  ERTMaskMon.h

include_HEADERS =

libonlertmon_la_SOURCES = \
  EMCalRichDecodeOnline.C \
  ERTMaskMon_Dict.C \
  ERTMaskMonDraw.C \
  ERTMaskMon.C \
  ERTMaskMonCommon.C \
  ERTMaskMonHisto.C \
  ERTMaskMonNHit.C \
  ERTMaskMonRFactor.C

bin_SCRIPTS = \
  ErtSetup.csh

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlertmon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

ERTMaskMon_Dict.C: \
  ERTMaskMon.h \
  ERTMaskMonDraw.h \
  ERTMaskMonCommon.h \
  ERTMaskMonHisto.h \
  ERTMaskMonNHit.h \
  ERTMaskMonRFactor.h \
  EMCalRichDecodeOnline.h \
  ERTMaskMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(RINCLUDES) $^

clean-local:
	rm -f *Dict.*
