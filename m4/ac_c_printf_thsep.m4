dnl @synopsis AC_C_PRINTF_THSEP
dnl
dnl @summary check compiler support for printf apostrophe flag
dnl
dnl This macro checks whether the compiler supports the ' flag in
dnl printf, which causes the non-fractional digits to be separated
dnl using a separator and grouping determined by the locale. If true,
dnl HAVE_PRINTF_THSEP is defined in config.h
dnl
dnl @category C
dnl @author Bill Poser <billposer@alum.mit.edu>
dnl @version 2006-04-26
dnl @license AllPermissive

AC_DEFUN([AC_C_PRINTF_THSEP],
[AC_TRY_COMPILE(,[printf("%'2d",101);],ac_cv_c_printf_thsep=yes,ac_cv_c_printf_thsep=no)
 if test $ac_cv_c_printf_thsep = yes; then
  AC_DEFINE(HAVE_PRINTF_THSEP, 1, [compiler understands printf flag for thousands separation in ints])
 fi
])
