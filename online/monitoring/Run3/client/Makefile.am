AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(OPT_PHENIX)/include \
  -isystem $(ROOTSYS)/include 

RINCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(OPT_PHENIX)/include \
  -I$(ROOTSYS)/include 

BUILT_SOURCES = \
  testexternals.C

lib_LTLIBRARIES = libonlmonclient.la   

noinst_HEADERS = \
  ClientHistoList.h \
  OnlMonHtml.h

include_HEADERS = \
  OnlMonClient.h \
  OnlMonDraw.h

libonlmonclient_la_SOURCES = \
OnlMonClient.C \
OnlMonClient_Dict.C \
OnlMonDraw.C \
OnlMonDraw_Dict.C \
OnlMonHtml.C \
ClientHistoList.C

libonlmonclient_la_LIBADD = \
  -L$(libdir) \
  -lonlmondb

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlmonclient.la


testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

OnlMonClient_Dict.C: OnlMonClient.h OnlMonClientLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(RINCLUDES) $^

OnlMonDraw_Dict.C: OnlMonDraw.h OnlMonDrawLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(RINCLUDES) $^

clean-local:
	rm -f *Dict.*
