AM_CPPFLAGS = -I$(top_srcdir)/src/include

lib_LTLIBRARIES = libezligolw.la

libezligolw_la_SOURCES = \
	array.c \
	ligolw.c \
	param.c \
	table.c \
	time.c \
	stream.h \
	stream.c \
	types.c \
	$(top_srcdir)/src/include/ezligolw/ezligolw.h \
	$(top_srcdir)/src/include/ezligolw/ezxml.h
libezligolw_la_LIBADD = $(top_builddir)/src/ezxml/libezxml.la
libezligolw_la_LDFLAGS = -version-info @LIBVERSION@ $(AM_LDFLAGS)
