AM_CPPFLAGS = -I../include $(LAL_CFLAGS) $(LALMETAIO_LIBS)

lib_LTLIBRARIES = libezligolw-lal.la

libezligolw_lal_la_SOURCES = \
	frequency_series.c \
	psd.c \
	sim_burst.c \
	sim_inspiral.c \
	sngl_inspiral.c \
	time_slide.c \
	utils.c
libezligolw_lal_la_LIBADD = ../ezxml/libezxml.la ../lib/libezligolw.la
libezligolw_lal_la_LDFLAGS = -version-info @LIBVERSION@ $(LAL_LIBS) $(LALMETAIO_LIBS)
libezligolw_lal_la_CFLAGS = -I../include $(LAL_CFLAGS) $(LALMETAIO_LIBS)
