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
  libonlmui_mon.la

libonlmui_mon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient \
  -lpdbcalBase


noinst_HEADERS = \
  muiOptions.h \
  muiMonDraw.h \
  muiMonitor.h

include_HEADERS =

libonlmui_mon_la_SOURCES = \
  corHist.cc \
  errorHist.cc \
  fillVar.cc \
  gl1Obj.cc \
  gl1pObj.cc \
  mui_mon_Dict.C \
  muiMap.cc \
  muiMonDraw.cc \
  muiMonGlobals.cc \
  muiMonitor.cc \
  muiOptions.cc \
  muid_blt.cc \
  obj_hist.cc \
  origHist.cc \
  timeHist.cc \
  TMuiChannelId.cc \
  triggerHists.cc \
  twopackHist.cc


bin_SCRIPTS = \
  MuiSetup.csh

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlmui_mon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

mui_mon_Dict.C:  muiMonDraw.h muiOptions.h muiMonitor.h mui_monLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
