AM_CPPFLAGS = -I../include

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
	../include/ezligolw/ezligolw.h \
	../include/ezligolw/ezxml.h
libezligolw_la_LIBADD = ../ezxml/libezxml.la
libezligolw_la_LDFLAGS = -version-info @LIBVERSION@
