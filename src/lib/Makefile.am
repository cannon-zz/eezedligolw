AM_CPPFLAGS = -I../include

lib_LTLIBRARIES = libezligolw.la

libezligolw_la_SOURCES = \
	array.c \
	ligolw.c \
	param.c \
	table.c \
	time.c \
	tokenizer.h \
	tokenizer.c \
	types.c \
	../include/libezligolw/ezligolw.h \
	../ezxml/ezxml.h
libezligolw_la_LIBADD = ../ezxml/libezxml.la
libezligolw_la_LDFLAGS = -version-info @LIBVERSION@
