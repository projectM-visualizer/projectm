# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_with_mpatrol.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_WITH_MPATROL(DEFAULT)
#
# DESCRIPTION
#
#   Integrates the mpatrol malloc debugging library into a new or existing
#   project and also attempts to determine the support libraries that need
#   to be linked in when libmpatrol is used.
#
#   It takes one optional parameter specifying whether mpatrol should be
#   included in the project (`yes') or not (`no'). This can also be
#   specified as `threads' if you wish to use the threadsafe version of the
#   mpatrol library. You can override the value of the optional parameter
#   with the `--with-mpatrol' option to the resulting `configure' shell
#   script.
#
# LICENSE
#
#   Copyright (c) 2008 Graeme S. Roy <graeme@epc.co.uk>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 6

AU_ALIAS([AM_WITH_MPATROL], [AX_WITH_MPATROL])
AC_DEFUN([AX_WITH_MPATROL], [
  # Firstly, determine if the mpatrol library should be used.

  AC_MSG_CHECKING(if mpatrol should be used)
  AC_ARG_WITH(mpatrol,
   [  --with-mpatrol          build with the mpatrol library],
   [case "$withval" in
     threads)
      ax_with_mpatrol=1
      ax_with_mpatrol_threads=1;;
     yes)
      ax_with_mpatrol=1
      ax_with_mpatrol_threads=0;;
     no)
      ax_with_mpatrol=0
      ax_with_mpatrol_threads=0;;
     *)
      AC_MSG_RESULT(no)
      AC_MSG_ERROR(invalid value $withval for --with-mpatrol);;
    esac
   ],
   [if test "x[$1]" = x
    then
     ax_with_mpatrol=0
     ax_with_mpatrol_threads=0
    elif test "[$1]" = no
    then
     ax_with_mpatrol=0
     ax_with_mpatrol_threads=0
    elif test "[$1]" = yes
    then
     ax_with_mpatrol=1
     ax_with_mpatrol_threads=0
    elif test "[$1]" = threads
    then
     ax_with_mpatrol=1
     ax_with_mpatrol_threads=1
    else
     AC_MSG_RESULT(no)
     AC_MSG_ERROR(invalid argument [$1])
    fi
   ]
  )

  if test "$ax_with_mpatrol" = 1
  then
   AC_MSG_RESULT(yes)

   # Next, determine which support libraries are available on this
   # system.  If we don't do this here then we can't link later with
   # the mpatrol library to perform any further tests.

   ax_with_mpatrol_libs=""
   AC_CHECK_LIB(ld, ldopen,
                ax_with_mpatrol_libs="$ax_with_mpatrol_libs -lld")
   AC_CHECK_LIB(elf, elf_begin,
                ax_with_mpatrol_libs="$ax_with_mpatrol_libs -lelf")
   AC_CHECK_LIB(bfd, bfd_init,
                ax_with_mpatrol_libs="$ax_with_mpatrol_libs -lbfd -liberty", ,
                -liberty)
   AC_CHECK_LIB(imagehlp, SymInitialize,
                ax_with_mpatrol_libs="$ax_with_mpatrol_libs -limagehlp")
   AC_CHECK_LIB(cl, U_get_previous_frame,
                ax_with_mpatrol_libs="$ax_with_mpatrol_libs -lcl")
   AC_CHECK_LIB(exc, unwind,
                ax_with_mpatrol_libs="$ax_with_mpatrol_libs -lexc")

   # Now determine which libraries really need to be linked in with
   # the version of libmpatrol that is on this system.  For example,
   # if the system has libelf and libbfd, we need to determine which
   # of these, if any, libmpatrol was built with support for.

   ax_with_mpatrol_libs2=""
   AC_CHECK_LIB(mpatrol, __mp_libld,
                ax_with_mpatrol_libs2="$ax_with_mpatrol_libs2 -lld", ,
                $ax_with_mpatrol_libs)
   AC_CHECK_LIB(mpatrol, __mp_libelf,
                ax_with_mpatrol_libs2="$ax_with_mpatrol_libs2 -lelf", ,
                $ax_with_mpatrol_libs)
   AC_CHECK_LIB(mpatrol, __mp_libbfd,
                ax_with_mpatrol_libs2="$ax_with_mpatrol_libs2 -lbfd -liberty", ,
                $ax_with_mpatrol_libs)
   AC_CHECK_LIB(mpatrol, __mp_libimagehlp,
                ax_with_mpatrol_libs2="$ax_with_mpatrol_libs2 -limagehlp", ,
                $ax_with_mpatrol_libs)
   AC_CHECK_LIB(mpatrol, __mp_libcl,
                ax_with_mpatrol_libs2="$ax_with_mpatrol_libs2 -lcl", ,
                $ax_with_mpatrol_libs)
   AC_CHECK_LIB(mpatrol, __mp_libexc,
                ax_with_mpatrol_libs2="$ax_with_mpatrol_libs2 -lexc", ,
                $ax_with_mpatrol_libs)

   # If we are using the threadsafe mpatrol library then we may also need
   # to link in the threads library.  We check blindly for pthreads here
   # even if we don't need them (in which case it doesn't matter) since
   # the threads libraries are linked in by default on AmigaOS, Windows
   # and Netware and it is only UNIX systems that we need to worry about.

   if test "$ax_with_mpatrol_threads" = 1
   then
    AC_CHECK_LIB(pthread, pthread_mutex_init,
                 ax_with_mpatrol_libs2="$ax_with_mpatrol_libs2 -lpthread", [
      AC_CHECK_LIB(pthreads, pthread_mutex_init,
                   ax_with_mpatrol_libs2="$ax_with_mpatrol_libs2 -lpthreads", [
        AC_CHECK_LIB(thread, pthread_mutex_init,
                     ax_with_mpatrol_libs2="$ax_with_mpatrol_libs2 -lthread")
       ]
      )
     ]
    )
   fi

   # We now know what libraries to use in order to link with libmpatrol.

   AC_DEFINE(HAVE_MPATROL, 1, [Define if using mpatrol])
   if test "$ax_with_mpatrol_threads" = 1
   then
    LIBS="-lmpatrolmt $ax_with_mpatrol_libs2 $LIBS"
   else
    LIBS="-lmpatrol $ax_with_mpatrol_libs2 $LIBS"
   fi

   # Finally, verify that mpatrol is correctly installed and that we can
   # link a simple program with it.

   AC_CACHE_CHECK(for working mpatrol, am_cv_with_mpatrol, [
     AC_TRY_LINK([#include <mpatrol.h>], [
int main(void)
{
    malloc(4);
    return EXIT_SUCCESS;
}
],
      [am_cv_with_mpatrol=yes],
      [am_cv_with_mpatrol=no]
     )
    ]
   )

   if test "$am_cv_with_mpatrol" = no
   then
    AC_MSG_ERROR(mpatrol not installed correctly)
   fi
  else
   AC_MSG_RESULT(no)
  fi
 ]
)
