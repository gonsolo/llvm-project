#
# Determine if the printf() functions have the %a format character.
# This is modified from:
# http://www.gnu.org/software/ac-archive/htmldoc/ac_cxx_have_ext_slist.html
AC_DEFUN([AC_C_PRINTF_A],[
  AC_MSG_CHECKING([for printf %a format specifier])
  AC_LANG_PUSH([C])
  AC_RUN_IFELSE([
    AC_LANG_PROGRAM([[
#include <stdio.h>
#include <stdlib.h>
]],[[
volatile double A, B;
char Buffer[100];
A = 1;
A /= 10.0;
sprintf(Buffer, "%a", A);
B = atof(Buffer);
if (A != B)
  return (1);
if (A != 0x1.999999999999ap-4)
  return (1);
return (0);]])],
  ac_c_printf_a=yes,
  ac_c_printf_a=no,
  ac_c_printf_a=no)
 AC_LANG_POP([C])
 AC_MSG_RESULT($ac_c_printf_a)
 if test "$ac_c_printf_a" = "yes"; then
   AC_DEFINE([HAVE_PRINTF_A],[1],[Define to have the %a format string])
 fi
])
