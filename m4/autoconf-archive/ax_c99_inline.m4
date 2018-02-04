# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_c99_inline.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_C99_INLINE
#
# DESCRIPTION
#
#   This macro defines HAVE_C99_INLINE if the C compiler supports "inline"
#   and "extern inline" correctly. An application may replace "inline" with
#   "static inline" as a workaround for older compilers.
#
# LICENSE
#
#   Copyright (c) 2009 Michael McMaster <email@michaelmcmaster.name>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 8

AC_DEFUN([AX_C99_INLINE], [
	AC_MSG_CHECKING([whether the compiler supports C99 inline functions])
	AC_REQUIRE([AC_PROG_CC_C99])

	AC_LANG_PUSH([C])

	dnl In a conforming C99 implementation a function marked "inline" will not
	dnl	be compiled into the translation unit if the compiler was not able to
	dnl	inline the function.
	dnl	GCC versions before 4.3 would output the inline functions into all
	dnl	translation units that could require the definition.
	AC_LINK_IFELSE(
		AC_LANG_SOURCE([
			inline void* foo() { foo(); return &foo; }
			int main() { return foo() != 0;}
			]),

		dnl the invalid source compiled, so the inline keyword does not work
		dnl correctly.
		AC_MSG_RESULT([no]),

		dnl Secondary test of valid source.
		AC_LINK_IFELSE(
			AC_LANG_SOURCE([
				extern inline void* foo() { foo(); return &foo; }
				int main() { return foo() != 0;}
				]),

			AC_MSG_RESULT([yes])
			AC_DEFINE([HAVE_C99_INLINE], [1],
				[Define to 1 if the "extern" keyword controls whether an inline function appears in a translation unit.]),

			dnl Perhaps inline functions aren't supported at all ?
			AC_MSG_RESULT([no])
			)
		)

	AC_LANG_POP([C])
	]);
