# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_f77_cmain_fflags.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_F77_CMAIN_FFLAGS([ACTION-IF-SUCCEED], [ACTION-IF-FAIL])
#
# DESCRIPTION
#
#   This macro figures out if extra Fortran compiler flags are required in
#   order to use the Fortran linker to link programs where the main()
#   function is defined via C (or other language). On some systems, notably
#   the Alpha with Compaq compilers, the Fortran libraries have their own
#   main() function which must be disabled.
#
#   Runs ACTION-IF-SUCCEED if successful, and ACTION-IF-FAIL if not. Defines
#   the output variable F77_CMAIN_FFLAGS to any discovered flags. (If
#   ACTION-IF-FAIL is not specified, defaults to halting with an error.)
#
#   This macro is especially useful in conjunction with automake, since by
#   default automake uses $F77 to link programs mixing C and Fortran,
#   leading to a link error on some systems. In this case, you should set
#   the FFLAGS for that program to include F77_CMAIN_FFLAGS.
#
# LICENSE
#
#   Copyright (c) 2008 Steven G. Johnson <stevenj@alum.mit.edu>
#
#   This program is free software: you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation, either version 3 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <https://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Archive. When you make and distribute a
#   modified version of the Autoconf Macro, you may extend this special
#   exception to the GPL to apply to your modified version as well.

#serial 6

AU_ALIAS([ACX_F77_CMAIN_FFLAGS], [AX_F77_CMAIN_FFLAGS])
AC_DEFUN([AX_F77_CMAIN_FFLAGS],
[AC_CACHE_CHECK([for f77 flags to use C main function], ax_cv_f77_cmain_fflags,
[ax_cv_f77_cmain_fflags="unknown"
AC_LANG_PUSH(C)
AC_COMPILE_IFELSE([[int main(void) { return 0; }]],
		  [mv conftest.$ac_objext conftest_cmain.$ac_objext],
		  [ax_cv_f77_cmain_fflags=error])
AC_LANG_POP(C)
if test "x$ax_cv_f77_cmain_fflags" != xerror; then
    AC_LANG_PUSH(Fortran 77)
    ax_save_LIBS=$LIBS
    LIBS="conftest_cmain.$ac_objext $LIBS"
    ax_save_FFLAGS=$FFLAGS
    for ax_flag in none -nofor_main; do
	case $ax_flag in
	    none) FFLAGS=$ax_save_FFLAGS ;;
	    *)    FFLAGS="$ax_save_FFLAGS $ax_flag" ;;
	esac
	AC_LINK_IFELSE([
      subroutine foobar()
      return
      end
], [ax_cv_f77_cmain_fflags=$ax_flag; break]);
    done
    FFLAGS=$ax_save_FFLAGS
    LIBS=$ax_save_LIBS
    AC_LANG_POP(Fortran 77)
fi])
    case $ax_cv_f77_cmain_fflags in
	error|unknown)
	    F77_CMAIN_FFLAGS=""
	    ifelse([$2],,[AC_MSG_ERROR([cannot link C main with Fortran])],[$2])
	    ;;
	*)
	    if test "x$ax_cv_f77_cmain_fflags" = xnone; then
		F77_CMAIN_FFLAGS=""
	    else
		F77_CMAIN_FFLAGS="$ax_cv_f77_cmain_fflags"
	    fi
	    $1
	    ;;
    esac
    AC_SUBST(F77_CMAIN_FFLAGS)
])
