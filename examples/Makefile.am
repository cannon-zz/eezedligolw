AM_CPPFLAGS = -I$(top_srcdir)/src/include

noinst_PROGRAMS =
TESTS =
CLEANFILES =
if COND_LAL
  noinst_PROGRAMS += psd_example sim_burst_example
  TESTS += psd_example sim_burst_example
  CLEANFILES += output.xml
endif

psd_example_SOURCES = psd_example.c
psd_example_LDADD = $(top_builddir)/src/lal/libezligolw-lal.la $(top_builddir)/src/lib/libezligolw.la $(top_builddir)/src/ezxml/libezxml.la
psd_example_LDFLAGS = $(LAL_LIBS) $(LALMETAIO_LIBS)
psd_example_CFLAGS = $(AM_CFLAGS) $(LAL_CFLAGS) $(LALMETAIO_LIBS)

sim_burst_example_SOURCES = sim_burst_example.c
sim_burst_example_LDFLAGS = $(LAL_LIBS) $(LALMETAIO_LIBS)
sim_burst_example_LDADD = $(top_builddir)/src/lal/libezligolw-lal.la $(top_builddir)/src/lib/libezligolw.la $(top_builddir)/src/ezxml/libezxml.la
sim_burst_example_CFLAGS = $(AM_CFLAGS) $(LAL_CFLAGS) $(LALMETAIO_CLAGS)
