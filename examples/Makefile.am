INCLUDES = -I../src/include
LDADD = ../src/lib/libezligolw.la

noinst_PROGRAMS =
if COND_LALSUPPORT
  noinst_PROGRAMS += sim_burst_example
endif

sim_burst_example_SOURCES = sim_burst_example.c
sim_burst_example_LDFLAGS = $(LALSUPPORT_LIBS)
sim_burst_example_CFLAGS = $(LALSUPPORT_CFLAGS)
