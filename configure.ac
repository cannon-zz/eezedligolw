# Preamble
AC_INIT([ezligolw],[0.3.0],[kipp@gravity.phys.uwm.edu])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([gnuscripts])
AC_CONFIG_MACRO_DIRS([gnuscripts])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_FILES([                        \
	Makefile                         \
	examples/Makefile                \
	ezligolw.pc                      \
	ezligolw-lal.pc                  \
	debian/control                   \
	debian/Makefile                  \
	src/Makefile                     \
	src/ezxml/Makefile               \
	src/include/Makefile             \
	src/include/ezligolw/Makefile    \
	src/lal/Makefile                 \
	src/lib/Makefile                 \
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
LT_INIT
AX_CFLAGS_WARN_ALL

# Check for zlib library
AC_CHECK_HEADER([zlib.h])
AC_CHECK_LIB([z], [main], , [AC_MSG_ERROR([cannot find the zlib library])])

# Check for libb64 library
AC_CHECK_HEADER([b64/cdecode.h])
AC_CHECK_LIB([b64], [main], , [AC_MSG_ERROR([cannot find the b64 library])])

# Check for LAL
AC_SUBST([MIN_LAL_VERSION], [7.0.0])
AC_SUBST([MIN_LALMETAIO_VERSION], [4.0.0])
PKG_CHECK_MODULES([LAL], [lal >= ${MIN_LAL_VERSION} lalsupport >= ${MIN_LAL_VERSION} lalmetaio >= ${MIN_LALMETAIO_VERSION}], [HAVE_LAL=yes], [HAVE_LAL=no ; AC_MSG_WARN([Not Found!])])
AC_SUBST([HAVE_LAL])
AC_SUBST([LAL_CFLAGS])
AC_SUBST([LAL_LIBS])
AM_CONDITIONAL([COND_LAL], [test "$HAVE_LAL" == "yes"])

# Generate configured output files
AC_PROG_MAKE_SET
AC_OUTPUT
