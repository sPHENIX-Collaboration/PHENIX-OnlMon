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
  libonlsvxpixelmon.la

libonlsvxpixelmon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient \
  -lonlmondb \
  -lsvx

noinst_HEADERS = \
  SvxPixelMon.h

include_HEADERS =

libonlsvxpixelmon_la_SOURCES = \
  SvxPixel_Dict.C \
  SvxPixelMonDraw.C \
  SvxPixelMon.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlsvxpixelmon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@


SvxPixel_Dict.C: SvxPixelMon.h SvxPixelMonDraw.h SvxPixelMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
