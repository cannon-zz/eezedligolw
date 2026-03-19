AM_CPPFLAGS = -I../include $(LAL_CFLAGS) $(LALMETAIO_LIBS)

lib_LTLIBRARIES = libezligolw-lal.la

libezligolw_lal_la_SOURCES = \
	sim_burst_row.c \
	sim_inspiral_row.c \
	time_slide_row.c \
	../include/ezligolw/ezligolw-lal.h
libezligolw_lal_la_LIBADD = ../ezxml/libezxml.la ../lib/libezligolw.la
libezligolw_lal_la_LDFLAGS = -version-info @LIBVERSION@ $(LAL_LIBS) $(LALMETAIO_LIBS)
libezligolw_lal_la_CFLAGS = -I../include $(LAL_CFLAGS) $(LALMETAIO_LIBS)
