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
  libonlgl1mon.la

libonlgl1mon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient

noinst_HEADERS = \
  Gl1Mon.h

include_HEADERS =

libonlgl1mon_la_SOURCES = \
  Gl1Mon_Dict.C \
  Gl1MonDraw.C \
  Gl1Mon.C

bin_SCRIPTS = \
  Gl1Setup.csh

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlgl1mon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

Gl1Mon_Dict.C: Gl1Mon.h Gl1MonDraw.h Gl1MonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
