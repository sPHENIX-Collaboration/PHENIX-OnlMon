AUTOMAKE_OPTIONS = foreign

# this supresses the warning about missing braces in the fpga lookup
# table
CXXFLAGS += -Wno-missing-braces

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLMON_MAIN)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include 

BUILT_SOURCES = \
  testexternals.C

lib_LTLIBRARIES = \
  libonlertlvl1mon.la

libonlertlvl1mon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient

noinst_HEADERS = \
  ErtLvl1Mon.h \
  ErtLvl1MonDraw.h \
  ERTLl1.h

include_HEADERS =

libonlertlvl1mon_la_SOURCES = \
  ErtLvl1Mon_Dict.C \
  ErtLvl1Mon.C \
  ErtLvl1MonDraw.C \
  ERTLl1.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlertlvl1mon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

ErtLvl1Mon_Dict.C: ErtLvl1Mon.h ErtLvl1MonDraw.h ErtLvl1MonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
