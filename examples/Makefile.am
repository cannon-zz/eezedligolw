AM_CPPFLAGS = -I../src/include
LDADD = ../src/lib/libezligolw.la ../src/ezxml/libezxml.la

noinst_PROGRAMS =
TESTS =
CLEANFILES =
if COND_LAL
  noinst_PROGRAMS += psd_example sim_burst_example
  TESTS += psd_example sim_burst_example
  CLEANFILES += output.xml
endif

psd_example_SOURCES = psd_example.c
psd_example_LDFLAGS = $(LAL_LIBS) $(LALMETAIO_LIBS)
psd_example_CFLAGS = $(LAL_CFLAGS) $(LALMETAIO_LIBS)

sim_burst_example_SOURCES = sim_burst_example.c
sim_burst_example_LDFLAGS = $(LAL_LIBS) $(LALMETAIO_LIBS) $(LALBURST_LIBS)
sim_burst_example_CFLAGS = $(LAL_CFLAGS) $(LALMETAIO_LIBS) $(LALBURST_LIBS)
