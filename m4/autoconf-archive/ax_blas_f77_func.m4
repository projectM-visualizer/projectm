# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_blas_f77_func.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_BLAS_F77_FUNC([ACTION-IF-PASS[, ACTION-IF-FAIL[, ACTION-IF-CROSS-COMPILING]])
#   AX_BLAS_WITH_F77_FUNC([ACTION-IF-FOUND-AND-PASS[, ACTION-IF-NOT-FOUND-OR-FAIL]])
#
# DESCRIPTION
#
#   These macros are intended as a supplement to the AX_BLAS macro, to
#   verify that BLAS functions are properly callable from Fortran. This is
#   necessary, for example, if you want to build the LAPACK library on top
#   of the BLAS.
#
#   AX_BLAS_F77_FUNC uses the defined BLAS_LIBS and Fortran environment to
#   check for compatibility, and takes a specific action in case of success,
#   resp. failure, resp. cross-compilation.
#
#   AX_BLAS_WITH_F77_FUNC is a drop-in replacement wrapper for AX_BLAS that
#   calls AX_BLAS_F77_FUNC after detecting a BLAS library and rejects it on
#   failure (i.e. pretends that no library was found).
#
# LICENSE
#
#   Copyright (c) 2008 Jaroslav Hajek <highegg@gmail.com>
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

#serial 9

AU_ALIAS([ACX_BLAS_F77_FUNC], [AX_BLAS_F77_FUNC])
AC_DEFUN([AX_BLAS_F77_FUNC], [
AC_PREREQ(2.50)
AC_REQUIRE([AX_BLAS])

# F77 call-compatibility checks
if test "$cross_compiling" = yes ; then
	ifelse($3, ,$1,$3)
elif test x"$ax_blas_ok" = xyes; then
	save_ax_blas_f77_func_LIBS="$LIBS"
	LIBS="$BLAS_LIBS $LIBS"
	AC_LANG_PUSH(Fortran 77)
# LSAME check (LOGICAL return values)
	AC_MSG_CHECKING([whether LSAME is called correctly from Fortran])
	AC_RUN_IFELSE(AC_LANG_PROGRAM(,[[
      logical lsame,w
      external lsame
      character c1,c2
      c1 = 'A'
      c2 = 'B'
      w = lsame(c1,c2)
      if (w) stop 1
      w = lsame(c1,c1)
      if (.not. w) stop 1
      ]]),[ax_blas_lsame_fcall_ok=yes],
	[ax_blas_lsame_fcall_ok=no])
	AC_MSG_RESULT([$ax_blas_lsame_fcall_ok])
# ISAMAX check (INTEGER return values)
	AC_MSG_CHECKING([whether ISAMAX is called correctly from Fortran])
	AC_RUN_IFELSE(AC_LANG_PROGRAM(,[[
      integer isamax,i
      external isamax
      real a(2)
      a(1) = 1e0
      a(2) = -2e0
      i = isamax(2,a,1)
      if (i.ne.2) stop 1
      ]]),[ax_blas_isamax_fcall_ok=yes],
	[ax_blas_isamax_fcall_ok=no])
	AC_MSG_RESULT([$ax_blas_isamax_fcall_ok])
# SDOT check (REAL return values)
	AC_MSG_CHECKING([whether SDOT is called correctly from Fortran])
	AC_RUN_IFELSE(AC_LANG_PROGRAM(,[[
      real sdot,a(1),b(1),w
      external sdot
      a(1) = 1e0
      b(1) = 2e0
      w = sdot(1,a,1,b,1)
      if (w .ne. a(1)*b(1)) stop 1
      ]]),[ax_blas_sdot_fcall_ok=yes],
	[ax_blas_sdot_fcall_ok=no])
	AC_MSG_RESULT([$ax_blas_sdot_fcall_ok])
# DDOT check (DOUBLE return values)
	AC_MSG_CHECKING([whether DDOT is called correctly from Fortran])
	AC_RUN_IFELSE(AC_LANG_PROGRAM(,[[
      double precision ddot,a(1),b(1),w
      external ddot
      a(1) = 1d0
      b(1) = 2d0
      w = ddot(1,a,1,b,1)
      if (w .ne. a(1)*b(1)) stop 1
      ]]),[ax_blas_ddot_fcall_ok=yes],
	[ax_blas_ddot_fcall_ok=no])
	AC_MSG_RESULT([$ax_blas_ddot_fcall_ok])
# CDOTU check (COMPLEX return values)
	AC_MSG_CHECKING([whether CDOTU is called correctly from Fortran])
	AC_RUN_IFELSE(AC_LANG_PROGRAM(,[[
      complex cdotu,a(1),b(1),w
      external cdotu
      a(1) = cmplx(1e0,1e0)
      b(1) = cmplx(1e0,2e0)
      w = cdotu(1,a,1,b,1)
      if (w .ne. a(1)*b(1)) stop 1
      ]]),[ax_blas_cdotu_fcall_ok=yes],
	[ax_blas_cdotu_fcall_ok=no])
	AC_MSG_RESULT([$ax_blas_cdotu_fcall_ok])
# ZDOTU check (DOUBLE COMPLEX return values)
	AC_MSG_CHECKING([whether ZDOTU is called correctly from Fortran])
	AC_RUN_IFELSE(AC_LANG_PROGRAM(,[[
      double complex zdotu,a(1),b(1),w
      external zdotu
      a(1) = dcmplx(1d0,1d0)
      b(1) = dcmplx(1d0,2d0)
      w = zdotu(1,a,1,b,1)
      if (w .ne. a(1)*b(1)) stop 1
      ]]),[ax_blas_zdotu_fcall_ok=yes],
	[ax_blas_zdotu_fcall_ok=no])
	AC_MSG_RESULT([$ax_blas_zdotu_fcall_ok])
# Check for correct integer size
# FIXME: this may fail with things like -ftrapping-math.
        AC_MSG_CHECKING([whether the integer size is correct])
        AC_RUN_IFELSE(AC_LANG_PROGRAM(,[[
      integer n,nn(3)
      real s,a(1),b(1),sdot
      a(1) = 1.0
      b(1) = 1.0
c Generate -2**33 + 1, if possible
      n = 2
      n = -4 * (n ** 30)
      n = n + 1
      if (n >= 0) goto 1
c This means we're on 64-bit integers. Check whether the BLAS is, too.
      s = sdot(n,a,1,b,1)
      if (s .ne. 0.0) stop 1
    1 continue
c We may be on 32-bit integers, and the BLAS on 64 bits. This is almost bound
c to have already failed, but just in case, we'll check.
      nn(1) = -1
      nn(2) = 1
      nn(3) = -1
      s = sdot(nn(2),a,1,b,1)
      if (s .ne. 1.0) stop 1
       ]]),[ax_blas_integer_size_ok=yes],
	[ax_blas_integer_size_ok=no])
	AC_MSG_RESULT([$ax_blas_integer_size_ok])

	AC_LANG_POP(Fortran 77)

# if any of the tests failed, reject the BLAS library
	if test $ax_blas_lsame_fcall_ok = yes \
		-a $ax_blas_sdot_fcall_ok = yes \
		-a $ax_blas_ddot_fcall_ok = yes \
		-a $ax_blas_cdotu_fcall_ok = yes \
		-a $ax_blas_zdotu_fcall_ok = yes \
		-a $ax_blas_integer_size_ok = yes; then
		ax_blas_f77_func_ok=yes;
		$1
	else
		ax_blas_f77_func_ok=no;
		$2
	fi
	LIBS="$save_ax_blas_f77_func_LIBS"
fi

])dnl AX_BLAS_F77_FUNC

AC_DEFUN([AX_BLAS_WITH_F77_FUNC], [
AC_PREREQ(2.50)
AX_BLAS([# disable special action], [])
if test x$ax_blas_ok = xyes ; then
	AX_BLAS_F77_FUNC(
	[ifelse([$1],,AC_DEFINE(HAVE_BLAS,1,[Define if you have a BLAS library.]),[$1])],
	[ax_blas_ok=no; BLAS_LIBS=])
fi
if test x$ax_blas_ok = xno ; then
	$2
fi
])dnl AX_BLAS_WITH_F77_FUNC
