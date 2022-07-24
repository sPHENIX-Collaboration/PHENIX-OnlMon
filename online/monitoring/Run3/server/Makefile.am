AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ONLINE_MAIN)/include \
  -I$(OPT_PHENIX)/include \
  -isystem $(ROOTSYS)/include \
  -isystem $(CORBADIR)/include

RINCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ONLINE_MAIN)/include \
  -I$(OPT_PHENIX)/include \
  -I$(ROOTSYS)/include \
  -I$(CORBADIR)/include

BUILT_SOURCES = \
  testexternals.C

lib_LTLIBRARIES = \
  libcorbamsgbuffer.la \
  libonlmonserver.la \
  libonlmonserver_funcs.la

libcorbamsgbuffer_la_SOURCES = \
  corba_msg_buffer.C \
  corba_msg_buffer_Dict.C

libcorbamsgbuffer_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLINE_MAIN)/lib \
  -lphmessages

libonlmonserver_funcs_la_LIBADD = \
  libonlmonserver.la \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lphool \
  -lpmonitor \
  -lmessage

libonlmonserver_la_LIBADD = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lmessage \
  -lphool \
  -ltrigger \
  -lheader \
  -lodbc++ \
  -lPgCalInstance

noinst_HEADERS = \
  pmonitorInterface.h

include_HEADERS = \
  HistoBinDefs.h \
  OnlMon.h \
  OnlMonBase.h \
  OnlMonServer.h \
  OnlMonStatus.h \
  OnlMonTrigger.h \
  PortNumber.h

libonlmonserver_funcs_la_SOURCES = \
  pmonitorInterface.C

libonlmonserver_la_SOURCES = \
  MessageSystem.C \
  OnlMon.C \
  OnlMon_Dict.C \
  OnlMonBase.C \
  OnlMonServer.C \
  OnlMonServer_Dict.C \
  OnlMonStatusDB.C \
  OnlMonTrigger.C \
  TrigInfo.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libcorbamsgbuffer.la \
  libonlmonserver.la \
  libonlmonserver_funcs.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@


OnlMonServer_Dict.C: \
  OnlMonServer.h \
  OnlMonStatusDB.h \
  OnlMonTrigger.h \
  OnlMonServerLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(RINCLUDES) $^

OnlMon_Dict.C: OnlMon.h OnlMonBase.h OnlMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(RINCLUDES) $^

corba_msg_buffer_Dict.C: corba_msg_buffer.h corba_msg_bufferLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(RINCLUDES) $^


clean-local:
	rm -f *Dict.*
