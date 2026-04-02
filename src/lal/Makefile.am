AM_CPPFLAGS = -I$(top_srcdir)/src/include

lib_LTLIBRARIES = libezligolw-lal.la

libezligolw_lal_la_SOURCES = \
	frequency_series.c \
	psd.c \
	sim_burst.c \
	sim_inspiral.c \
	sngl_inspiral.c \
	time_slide.c \
	utils.c
libezligolw_lal_la_CFLAGS = $(AM_CFLAGS) $(LAL_CFLAGS) $(LALMETAIO_LIBS)
libezligolw_lal_la_LIBADD = $(top_builddir)/src/lib/libezligolw.la $(top_builddir)/src/ezxml/libezxml.la
libezligolw_lal_la_LDFLAGS = -version-info @LIBVERSION@ $(LAL_LIBS) $(LALMETAIO_LIBS)
