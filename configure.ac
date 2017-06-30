dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)

AC_INIT([tidy], [0.1a], [simon@havvoric.com])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR([m4])

# add checking message
AC_MSG_CHECKING(whether to build with debug information)

# create configure parameter and init variable $debuger
debuger=no
AC_ARG_ENABLE(debug,
	AC_HELP_STRING(
	    [--enable-debug],
	    [enable debug data generation (def=no)]
	),
  	debuger="$enableval"
)

# resulting value to screen (to complete checking message)
AC_MSG_RESULT($debuger)

# set DEBUG flag and introduce additional compiler flags
if test x"$debuger" = x"yes"; then
	AC_DEFINE(DEBUG,[],[whether to build with debug information])
	CXXFLAGS="$CXXFLAGS -g -pg -O0 -Wall -Weffc++ -Wextra -Werror -Wstrict-null-sentinel -U_FORTIFY_SOURCE -Wp,-D_FORTIFY_SOURCE=2 "
	LDFLAGS="$LDFLAGS -pg"
fi

# Ensure long file access
CXXFLAGS="$CXXFLAGS --std=c++0x -rdynamic `getconf LFS_CFLAGS`"

AM_INIT_AUTOMAKE([1.9 subdir-objects])
AC_CONFIG_HEADERS([config.h:config.in])

AC_C_PRINTF_THSEP
AX_BOOST_BASE
AX_BOOST_SYSTEM
AX_BOOST_THREAD
AX_BOOST_LOG
AX_PTHREAD

# Checks for programs
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
LT_INIT

# Checks for header files
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h regex.h getopt.h stdarg.h fnmatch.h valgrind/valgrind.h pthread.h dirent.h libgen.h magic.h openssl/sha.h locale.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_CHECK_SIZEOF(size_t)
AC_TYPE_SIZE_T
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int16_t)
AC_CHECK_SIZEOF(uint16_t)
AC_CHECK_SIZEOF(int32_t)
AC_CHECK_SIZEOF(uint32_t)
AC_CHECK_SIZEOF(int64_t)
AC_CHECK_SIZEOF(off_t)
AC_CHECK_SIZEOF(off64_t)
AC_CHECK_SIZEOF(time_t)
AC_HEADER_TIME

# Checks for libraries
AC_CHECK_LIB([magic], [magic_open])
AC_CHECK_LIB([crypto], [SHA1_Init])

# Checks for pkg-check modules
PKG_CHECK_MODULES([FUSE], [fuse])
AC_SUBST(FUSE_CFLAGS)
AC_SUBST(FUSE_LIBS)

PKG_CHECK_MODULES([SQLITE], [sqlite3])
AC_SUBST(SQLITE_CFLAGS)
AC_SUBST(SQLITE_LIBS)

#PKG_CHECK_MODULES([LUA], [lua5.1],, [ PKG_CHECK_MODULES([LUA], [lua-5.1]) ])
#AC_SUBST(LUA_CFLAGS)
#AC_SUBST(LUA_LIBS)

#PKG_CHECK_MODULES([XML], [libxml-2.0])
#AC_SUBST(XML_CFLAGS)
#AC_SUBST(XML_LIBS)

# Checks for library functions
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_VPRINTF
AC_FUNC_SETVBUF_REVERSED
AC_CHECK_FUNCS([ftruncate memset strcasecmp strerror])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
