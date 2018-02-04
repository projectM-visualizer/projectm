# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_printf_size_t.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PRINTF_SIZE_T
#
# DESCRIPTION
#
#   Check for the appropriate printf() format string modifier to use with a
#   size_t argument. If a suitable modifier is found, it is AC_DEFINE'd as
#   PRI_SIZE_T_MODIFIER, and can be used in C code as e.g.
#
#     printf("Size = %" PRI_SIZE_T_MODIFIER "u bytes.\n", sizeof(foo));
#
#   If you use the PRIxNN macros from ISO C99's <inttypes.h>, you may wish
#   to define a supplemental set of macros along similar lines:
#
#     #define PRIdSIZE PRI_SIZE_T_MODIFIER "d"
#     #define PRIiSIZE PRI_SIZE_T_MODIFIER "i"
#     #define PRIoSIZE PRI_SIZE_T_MODIFIER "o"
#     #define PRIuSIZE PRI_SIZE_T_MODIFIER "u"
#     #define PRIxSIZE PRI_SIZE_T_MODIFIER "x"
#     #define PRIXSIZE PRI_SIZE_T_MODIFIER "X"
#
#   Note: This macro invokes a test program to check whether a given format
#   modifier is understood by the system's sprintf() function. If the user
#   is cross-compiling, then this check cannot be performed, and thereby
#   PRI_SIZE_T_MODIFIER will remain undefined.
#
# LICENSE
#
#   Copyright (c) 2012 Daniel Richard G. <skunk@iSKUNK.ORG>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 2

AC_DEFUN([AX_PRINTF_SIZE_T], [

AC_MSG_CHECKING([for printf() modifier to use with size_t])
AC_LANG_PUSH([C])

save_CFLAGS="$CFLAGS"
if test "_$GCC" = _yes
then
	# Take advantage of GCC's format-string checking
	CFLAGS="$CFLAGS -Wformat -pedantic -Werror"
fi

result=unknown

for mod in z l "" ll
do
	AC_RUN_IFELSE([AC_LANG_SOURCE([[#include <stdio.h>
int main(void)
{
	char buf[64];
	sprintf(buf, "%]${mod}[u", sizeof(buf));
	if (buf[0] == '6' && buf[1] == '4' && buf[2] == '\0')
		return 0;
	return 1;
}]])],
	[result="\"$mod\""; break],
	[],
	[result=cross-compile; break])
done

CFLAGS="$save_CFLAGS"

AC_LANG_POP
AC_MSG_RESULT([$result])

case "$result" in
	cross-compile)
	AC_MSG_WARN([cannot test printf() compatibility when cross-compiling])
	;;

	unknown)
	AC_MSG_ERROR([cannot find a suitable modifier])
	;;

	*)
	AC_DEFINE_UNQUOTED([PRI_SIZE_T_MODIFIER], [$result], [Define to the printf() modifier to use with size_t.])
	;;
esac

])dnl AX_PRINTF_SIZE_T
