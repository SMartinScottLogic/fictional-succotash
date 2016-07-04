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
	AS_HELP_STRING([--enable-debug],[enable debug data generation (def=no)
	]),
  	debuger="$enableval"
)

# resulting value to screen (to complete checking message)
AC_MSG_RESULT($debuger)

# Ensure long file access
CXXFLAGS="$CXXFLAGS --std=c++0x -rdynamic `getconf LFS_CFLAGS`"

# set OGS2_LIB_DEBUG flag and introduce additional compiler flags
if test x"$debuger" = x"yes"; then
	AC_DEFINE(OGS2_LIB_DEBUG,[],[whether to build with debug information])
	CXXFLAGS="$CXXFLAGS -g -pg -O0 -Wall -Weffc++ -Wextra -Werror -Wstrict-null-sentinel -Wp,-D_FORTIFY_SOURCE=2 "
fi

AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_HEADERS([config.h:config.in])

AC_C_PRINTF_THSEP
# Checks for programs
AC_PROG_CXX
AC_PROG_LIBTOOL
LT_INIT

# Checks for header files
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h valgrind/valgrind.h regex.h stdarg.h pthread.h dirent.h libgen.h magic.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
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
#PKG_CHECK_MODULES([XML], [libxml-2.0])
#AC_SUBST([XML_CFLAGS])
#AC_SUBST([XML_LIBS])
AC_CHECK_LIB(magic, magic_open)

# Checks for library functions
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_VPRINTF
AC_FUNC_SETVBUF_REVERSED
AC_CHECK_FUNCS([ftruncate memset strcasecmp strerror])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
