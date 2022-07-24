AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(includedir) -I$(OFFLINE_MAIN)/include -I$(ROOTSYS)/include 

include_HEADERS = 

bin_PROGRAMS = \
  runtype \
  xname

xname_SOURCES = \
  xname.c

runtype_SOURCES = \
  runtype.C

runtype_LDADD = \
 -L$(libdir) \
 -lonlmonodbc \
 $(ROOTLIBS)