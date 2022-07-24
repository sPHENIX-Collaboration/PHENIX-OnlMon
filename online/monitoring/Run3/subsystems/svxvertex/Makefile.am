AUTOMAKE_OPTIONS = foreign

SUBDIRS = \
  calib

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLMON_MAIN)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include 

lib_LTLIBRARIES = \
  libonlsvxvertex.la

libonlsvxvertex_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient \
  -lbbc \
  -lsvx \
  -lphool \
  -lEvent \
  -lSubsysReco

libonlsvxvertex_la_SOURCES = \
  SvxVertex.C \
  SvxVertexDraw.C \
  SvxVertex_Dict.C 

bin_SCRIPTS = \
  SvxVertexSetup.csh

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlsvxvertex.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@


SvxVertex_Dict.C: \
  SvxVertex.h \
  SvxVertexDraw.h \
  SvxVertexLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^


clean-local:
	rm -f *Dict.*
