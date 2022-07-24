AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLMON_MAIN)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include 

lib_LTLIBRARIES = \
  libonlrecomon.la

libonlrecomon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient \
  -lbbc \
  -lemc \
  -lphool \
  -lEvent \
  -lSubsysReco \
  -lpad \
  -ldch \
  -lrxnp_interface \
  -lreactionplane

libonlrecomon_la_SOURCES = \
  RecoHistos.C \
  RecoMon_Dict.C \
  RecoMon.C \
  RecoMonDraw.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlrecomon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@


RecoMon_Dict.C: \
  RecoHistos.h \
  RecoMon.h \
  RecoMonDraw.h \
  RecoMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^


clean-local:
	rm -f *Dict.*
