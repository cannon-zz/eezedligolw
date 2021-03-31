# Preamble
AC_INIT([libezligolw],[0.1.0],[kipp@gravity.phys.uwm.edu])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([gnuscripts])
AC_CONFIG_MACRO_DIRS([gnuscripts])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_FILES([                        \
	Makefile                         \
	libezligolw.pc                   \
	src/Makefile                     \
	src/ezxml/Makefile               \
	src/include/Makefile             \
	src/include/libezligolw/Makefile \
	src/lib/Makefile                 \
	examples/Makefile                \
])

# Set the library API info
AC_SUBST([LIBAPI], [0])
AC_SUBST([LIBREL], [0])
AC_SUBST([LIBAGE], [0])
AC_SUBST([LIBVERSION], [${LIBAPI}:${LIBREL}:${LIBAGE}])

# pkg-config module path
AC_SUBST([pkgconfigdir],[${libdir}/pkgconfig])

# Checks for programs
AC_PROG_CC
AC_PROG_LIBTOOL

# Check for zlib library
AC_CHECK_HEADER([zlib.h])
AC_CHECK_LIB([z], [main], , [AC_MSG_ERROR([cannot find the zlib library])])

# Check for LAL
PKG_CHECK_MODULES([LAL], [lal], [HAVE_LAL=yes], [HAVE_LAL=no ; AC_MSG_WARN([Not Found!])])
AC_SUBST([HAVE_LAL])
AC_SUBST([LAL_CFLAGS])
AC_SUBST([LAL_LIBS])
AM_CONDITIONAL([COND_LAL], [test "$HAVE_LAL" == "yes"])

PKG_CHECK_MODULES([LALSUPPORT], [lalsupport], [HAVE_LALSUPPORT=yes], [HAVE_LALSUPPORT=no ; AC_MSG_WARN([Not Found!])])
AC_SUBST([HAVE_LALSUPPORT])
AC_SUBST([LALSUPPORT_CFLAGS])
AC_SUBST([LALSUPPORT_LIBS])
AM_CONDITIONAL([COND_LALSUPPORT], [test "$HAVE_LALSUPPORT" == "yes"])

# Generate configured output files
AC_PROG_MAKE_SET
AC_OUTPUT
