AUTOMAKE_OPTIONS = foreign

AM_LDFLAGS =  -L$(libdir) -L$(ONLMON_MAIN)/lib

BUILT_SOURCES = \
  testexternals.C

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLMON_MAIN)/include \
  -I$(OFFLINE_MAIN)/include \
  -isystem $(ROOTSYS)/include 

RINCLUDES = \
  -I$(includedir) \
  -I$(ONLMON_MAIN)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include 

lib_LTLIBRARIES = \
  libonldaqmon.la

libonldaqmon_la_LIBADD = \
  -lonlmonserver \
  -lonlmonclient \
  -lonlmondb

noinst_HEADERS = \
  DaqMon.h

include_HEADERS =

libonldaqmon_la_SOURCES = \
  DaqAccMon.C \
  DaqBbcMon.C \
  DaqDchMon.C \
  DaqEmcMon.C \
  DaqErtMon.C \
  DaqFcalMon.C \
  DaqFvtxMon.C \
  DaqGl1Mon.C \
  DaqMonDraw.C \
  DaqMonDraw_Dict.C \
  DaqMon_Dict.C \
  DaqMon.C \
  DaqMpcMon.C \
  DaqMpcExMon.C \
  DaqMuidMon.C \
  DaqMuTrigMon.C \
  DaqMutrMon.C \
  DaqPadMon.C \
  DaqRichMon.C \
  DaqRpcMon.C \
  DaqTofMon.C \
  DaqVtxpMon.C \
  DaqVtxsMon.C \
  DaqZdcMon.C \
  GranMonDraw.C \
  GranuleMon.C \
  GranuleMon_Dict.C \
  VtxpMonDraw.C \
  VtxsMonDraw.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonldaqmon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

DaqMon_Dict.C: DaqMon.h DaqMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(RINCLUDES) $^

DaqMonDraw_Dict.C: \
  DaqMonDraw.h \
  GranMonDraw.h \
  VtxpMonDraw.h \
  VtxsMonDraw.h \
  DaqMonDrawLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(RINCLUDES) $^

GranuleMon_Dict.C: \
  DaqAccMon.h \
  DaqBbcMon.h \
  DaqDchMon.h \
  DaqEmcMon.h \
  DaqErtMon.h \
  DaqFcalMon.h \
  DaqFvtxMon.h \
  DaqGl1Mon.h \
  DaqMpcMon.h \
  DaqMpcExMon.h \
  DaqMuidMon.h \
  DaqMuTrigMon.h \
  DaqMutrMon.h \
  DaqPadMon.h \
  DaqRichMon.h \
  DaqRpcMon.h \
  DaqTofMon.h \
  DaqVtxpMon.h \
  DaqVtxsMon.h \
  DaqZdcMon.h \
  GranuleMon.h \
  GranuleMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(RINCLUDES) $^

clean-local:
	rm -f *Dict.*
