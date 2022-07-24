AUTOMAKE_OPTIONS = foreign

# contains unused vars, need to append it to CXXLFAGS to
# override the -Wall
CXXFLAGS += -Wno-unused

BUILT_SOURCES = \
  testexternals.C

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -L$(ONLINE_MAIN)/lib

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLMON_MAIN)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(ONLINE_MAIN)/include \
  -I$(ROOTSYS)/include \
  -I$(CORBADIR)/include

lib_LTLIBRARIES = \
  libonlmcrmon.la

libonlmcrmon_la_LIBADD = \
  -lonlmonserver \
  -lonlmonclient \
  -lonlmondb \
  -lbbc \
  -lzdc \
  -lmessage \
  -lcutilities 

noinst_HEADERS = \
  McrMon.h \
  McrMonDraw.h

include_HEADERS =

libonlmcrmon_la_SOURCES = \
  McrMon_Dict.C \
  McrMon.C \
  McrMonDraw.C \
  McrSend.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlmcrmon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

McrMon_Dict.C: McrMon.h McrMonDraw.h McrMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
