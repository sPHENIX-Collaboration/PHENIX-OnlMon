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
  libonlsvxstripmon.la

libonlsvxstripmon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient \
  -lonlmondb 

noinst_HEADERS = \
  SvxStripLadderMap.h \
  SvxStripMon.h

include_HEADERS =

libonlsvxstripmon_la_SOURCES = \
  SvxStripLadderMap.C \
  SvxStrip_Dict.C \
  SvxStripMonDraw.C \
  SvxStripMon.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlsvxstripmon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@


SvxStrip_Dict.C: SvxStripMon.h SvxStripMonDraw.h SvxStripMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
