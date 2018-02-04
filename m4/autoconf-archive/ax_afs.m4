# ===========================================================================
#          https://www.gnu.org/software/autoconf-archive/ax_afs.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_AFS
#
# DESCRIPTION
#
#   This sets up the proper includes and libs needed for building programs
#   that link with AFS. It adds a --with-afsdir option that lets you specify
#   where the AFS includes and libs are (defaulting to /usr/afsws).
#
#   It also adds the -I and -L options to CPPFLAGS and LDFLAGS
#
#   It creates an AC_SUBST(AFSWS) that contains the directory being used.
#
#   It checks for -lBSD, -lsocket and -lnsl since AFS needs those if they
#   exist. It also adds -R/usr/ucblib -L/usr/ucblib -lucb if on Solaris.
#
#   It sets VICE_ETC_PATH to be the directory where /usr/vice/etc lives,
#   since AFS 3.4 uses a different define than AFS 3.5 does for that path.
#
#   Finally, it defines AFS_int32 to be the in32 type needed. In older
#   versions of afs it was 'int32', and in newer versions it's 'afs_int32'.
#
# LICENSE
#
#   Copyright (c) 2008 Scott Grosch <Scott.Grosch@intel.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 7

AU_ALIAS([SG_AFS], [AX_AFS])
AC_DEFUN([AX_AFS],
[AC_ARG_WITH(afsdir, AS_HELP_STRING([--with-afsdir=DIR],
                                      [Directory holding AFS includes/libs]),
                ax_cv_with_afsdir=$withval)
 AC_CACHE_CHECK([for location of AFS directory],
                         ax_cv_with_afsdir, ax_cv_with_afsdir=/usr/afsws)

CPPFLAGS="-I${ax_cv_with_afsdir}/include $CPPFLAGS"
LDFLAGS="-L${ax_cv_with_afsdir}/lib -L${ax_cv_with_afsdir}/lib/afs $LDFLAGS"

dnl Once we specify a directory, we try to link a test program.  If the link
dnl works, we store the value of the directory in a cache variable.  If not,
dnl we put _FAILED_ in the cache value.  In this way we don't try to link
dnl the test program if our afsdir value was cached, as we know it works.
AC_MAX_CHECKING([whether the specified AFS dir looks valid])
if test "x${ax_cv_afsdir_link_works:-set}" != "x$ax_cv_with_afsdir"; then
        save_LIBS="$LIBS"
        LIBS="$save_LIBS -lcmd"
        AC_TRY_LINK([#include <afs/cmd.h>],
                [cmd_CreateAlias((struct cmd_syndesc *)0, "foo")],
                ax_cv_afsdir_link_works=$ax_cv_with_afsdir,
                ax_cv_afsdir_link_works=_FAILED_)
        LIBS="$save_LIBS"
        wasCached=""
else
        wasCached="(cached)"
fi
if test "x$ax_cv_afsdir_link_works" = "x$ax_cv_with_afsdir"; then
        AC_MAX_RESULT([${wasCached} yes])
else
        AC_MAX_RESULT([no])
        AC_MAX_ERROR([Unable to link test program....bad AFS dir specified?])
fi

dnl Much easier to use in XXX.in files
AFSWS=$ax_cv_with_afsdir
AC_SUBST(AFSWS)

dnl Linking against AFS always needs these
AC_CHECK_LIB(BSD, signal)
AC_CHECK_LIB(socket, getservbyname)
AC_CHECK_LIB(nsl, gethostbyname)

dnl On Solaris is just always needs the -lucb library from the compatibility
dnl area.  I can't think of any other way to do this than just hardcode it.
AC_CANONICAL_HOST
case "$host" in
*-*-solaris*)
        LDFLAGS="-L/usr/ucblib -R/usr/ucblib $LDFLAGS"
        LIBS="-lucb $LIBS"
  ;;
esac

dnl And it always needs these libs added
LIBS="$LIBS -lacl -lvolser -lvldb -lprot -lkauth -lauth -lrxkad -lubik ${ax_cv_with_afsdir}/lib/afs/vlib.a -ldir ${ax_cv_with_afsdir}/lib/afs/util.a -lsys -lafsint -lrx -lsys -ldes -lcom_err -llwp -lcmd -laudit"

dnl This really should be AC_CHECK_LIB() but that always fails for some reason
if test -f "${ax_cv_with_afsdir}/lib/afs/libaudit.a"; then
        LIBS="$LIBS -laudit"
fi

dnl If dirpath.h exists and has the value, use that.  Otherwise don't
AC_CHECK_HEADERS(afs/dirpath.h,
        [AC_DEFINE(VICE_ETC_PATH, AFSDIR_CLIENT_ETC_DIRPATH)],
        [AC_DEFINE(VICE_ETC_PATH, AFSCONF_CLIENTNAME)])

dnl Find out if we should use afs_int32 or int32.  They changed the
dnl type across AFS versions.
AC_CACHE_CHECK([for AFS int32 type], ac_cv_type_int32,
[AC_EGREP_CPP(dnl
changequote(<<,>>)dnl
<<(^|[^a-zA-Z_0-9])afs_int32[^a-zA-Z_0-9]>>dnl
changequote([,]), [#include <afs/stds.h>
], ac_cv_type_int32=afs_int32, ac_cv_type_int32=int32)])
AC_DEFINE_UNQUOTED(AFS_int32, $ac_cv_type_int32)

AH_TEMPLATE([VICE_ETC_PATH],
[Define this to be the define used in the AFS header for /usr/vice/etc])

AH_TEMPLATE([AFS_int32], [Define this to be the type AFS uses for int32])
])dnl
