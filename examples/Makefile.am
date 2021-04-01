AM_CPPFLAGS = -I../src/include
LDADD = ../src/lib/libezligolw.la ../src/ezxml/libezxml.la

noinst_PROGRAMS =
if COND_LAL
  noinst_PROGRAMS += sim_burst_example
endif

sim_burst_example_SOURCES = sim_burst_example.c
sim_burst_example_LDFLAGS = $(LAL_LIBS) $(LALMETAIO_LIBS) $(LALBURST_LIBS)
sim_burst_example_CFLAGS = $(LAL_CFLAGS) $(LALMETAIO_LIBS) $(LALBURST_LIBS)
