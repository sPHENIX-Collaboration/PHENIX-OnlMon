AUTOMAKE_OPTIONS = foreign

SUBDIRS = calib

BUILT_SOURCES = \
  testexternals.C

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLMON_MAIN)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include 

lib_LTLIBRARIES = \
  libonlmpcmon.la

libonlmpcmon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient \
  -lonlmondb \
  -lbbc \
  -lnanoDST \
  -ltrigger \
  -lmpc

libonlmpcmon_la_SOURCES = \
  MpcMon_Dict.C \
  MpcMonDraw.C \
  MpcMon.C

bin_SCRIPTS = \
  MpcSetup.csh

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlmpcmon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@


MpcMon_Dict.C: MpcMon.h MpcMonDraw.h MpcMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
