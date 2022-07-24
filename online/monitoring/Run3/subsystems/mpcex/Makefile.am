AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
  testexternals.C

SUBDIRS = calib

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLMON_MAIN)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(ONLINE_MAIN)/include \
  -I$(CORBADIR)/include \
  -I$(OPT_PHENIX)/include \
  -I$(ROOTSYS)/include 

lib_LTLIBRARIES = \
  libonlmpcexmon.la

libonlmpcexmon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -L$(ONLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient \
  -lonlmondb \
  -L$(OPT_PHENIX)/lib \
  -lodbc \
  -lodbc++ \
  -lmpcex_base \
  -lmpcex_interface \
  -lmpcex_modules \
  -lFwdCal

noinst_HEADERS = \
  MpcExMon.h

libonlmpcexmon_la_SOURCES = \
  MpcExMon_Dict.C \
  MpcExMon.C \
  MpcExMonDraw.C

bin_SCRIPTS = \
  MpcExSetup.csh

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlmpcexmon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@


MpcExMon_Dict.C: MpcExMon.h MpcExMonDraw.h MpcExMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
