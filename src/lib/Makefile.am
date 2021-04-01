AM_CPPFLAGS = -I../include

lib_LTLIBRARIES = libezligolw.la

libezligolw_la_SOURCES = ezligolw.c ../include/libezligolw/ezligolw.h ../ezxml/ezxml.h
libezligolw_la_LIBADD = ../ezxml/libezxml.la
libezligolw_la_LDFLAGS = -version-info @LIBVERSION@
