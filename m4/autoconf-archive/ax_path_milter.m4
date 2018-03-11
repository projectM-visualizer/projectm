# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_path_milter.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PATH_MILTER([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
#
# DESCRIPTION
#
#   This macro tries to automatically find the library libmilter.a and the
#   header file "libmilter/mfapi.h", which are required when compiling a
#   milter for Sendmail. When successful, it sets the output variable
#   MILTER_LIBS to "-lmilter", MILTER_LDFLAGS to contain an -Lpathtolib
#   option, and MILTER_CPPFLAGS to contain an -Ipathtoinclude option, if
#   they are necessary.
#
#   The easiest way to use this macro is something like:
#
#     AX_PATH_MILTER([8.12],[
#       LIBS="$MILTER_LIBS $LIBS"
#       LDFLAGS="$MILTER_LDFLAGS $LDFLAGS"
#       CPPFLAGS="$CPPFLAGS $MILTER_CPPFLAGS"
#     ],[
#       AC_MSG_ERROR([required milter library and header not found])
#     ])
#
#   If the macro is successful, it just adds any flags to the necessary
#   environment. If it is not successful, it would likely be a fatal error,
#   because if an application is linking with libmilter.a, it is probably
#   because it is a milter.
#
#   There are two optional "--with" options for configure which are added.
#   If they are specified, they override any searching that is done. They
#   are:
#
#     --with-sendmail-base=<DIR>  This option is used to explicitly
#            specify the base of the sendmail distribution.
#
#     --with-sendmail-obj=<DIR>   The option is used to explicitly specify
#            the "obj.*" subdirectory in the sendmail distribution
#            that should be used.
#
#   When sendmail-base is not specified, the current environment is first
#   tested to see if the header and library are available, and if so
#   MILTER_LDFLAGS and MILTER_CPPFLAGS are left empty.
#
#   There are two places that are searched for the sendmail base directory.
#   The first location is one directory down from the current directory. It
#   checks if there is a directory of the form sendmail-8.1*, limited to
#   version 8.12.x or higher, then chooses the directory with the highest
#   version number. If that method does not succeed, it then looks in the
#   file /etc/mail/sendmail.cf for the directory it was built from, and uses
#   the base of that distribution. If neither of these methods work, then it
#   fails.
#
#   There are two methods for finding the "obj.*" directory when it is not
#   specified. The first is to try to run sendmail's Build program with the
#   -M option which will print out the name of the obj. directory for the
#   tool in the directory where it is run from. If this does not work, is
#   looks for the newest directory of the form "obj.*" in the sendmail base
#   directory.
#
#   Two addition output variables that are defined, whether or not the files
#   are found are SENDMAIL_BASE_DIR and SENDMAIL_OBJ_DIR, which are the
#   suspected location of the sendmail base directory and obj.*
#   subdirectory.
#
#   NOTE: POSIX threads MUST be configured BEFORE this function is called or
#   it will not find libmilter.a even if it exists. The easiest way is to
#   use the AX_PTHREAD macro by Steven G. Johnson and Alejandro Forero
#   Cuervo which is available from the Autoconf Macro Archive.
#
# LICENSE
#
#   Copyright (c) 2008 Tim Toolan <toolan@ele.uri.edu>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 12

###############################################################################
AC_DEFUN([AX_PATH_MILTER], [

# Used to indicate success or failure of this function.
ax_path_milter_ok=no

# Convert sections of MINIMUM-VERSION to three digit numbers by adding zeros.
# For example 8.12.9 would become 008.012.009
ac_milter_minimum_version=`echo "$1" | sed 's,\([[0-9]]*\),x\1x,g;s,x\([[0-9]]\)x,x0\1x,g;s,x\([[0-9]][[0-9]]\)x,x0\1x,g;s,x,,g'`

# Add options --with-sendmail-base and --with-sendmail-obj to configure.
AC_ARG_WITH([sendmail-base],
  [  --with-sendmail-base=<DIR>  base directory of sendmail distribution])
AC_ARG_WITH([sendmail-obj],
  [  --with-sendmail-obj=<DIR>   obj.* subdirectory in sendmail distribution])

# Check for functions required by libmilter.
AC_CHECK_FUNC(inet_aton, [], [AC_SEARCH_LIBS(inet_aton, [socket nsl resolv])])
AC_CHECK_FUNC(socket, [], [AC_SEARCH_LIBS(socket, [socket nsl])])
AC_CHECK_FUNC(gethostbyname, [], [AC_SEARCH_LIBS(gethostbyname, [socket nsl])])

###############################################################################
#
# If neither --with-sendmail-base or --with-sendmail-obj is specified
# check the existing environment first for mfapi.h and libmilter without
# modifying CPPFLAGS, LDFLAGS, and LIBS first.
#
if test "x$with_sendmail_base$with_sendmail_obj" = "x" ; then
  AC_CHECK_HEADER([libmilter/mfapi.h],[
    AC_CHECK_LIB([milter],[smfi_main],[
      # both tests succeeded so indicate success
      ax_path_milter_ok=yes

      # add -lmilter to the libraries to link
      MILTER_LIBS="-lmilter"
    ])
  ])

  if test "$ax_path_milter_ok" = "no" ; then
    # Unset the cached test results because we will be trying them again later.
    ac_milter_tmp=abcdefg
    if unset ac_milter_tmp 2> /dev/null ; then
      unset ac_cv_header_libmilter_mfapi_h
      unset ac_cv_lib_milter_smfi_main
    else
      AC_MSG_WARN(
        [system doesn't have unset so either use --with-sendmail-base
         or set LDFLAGS and CPPFLAGS with the necessary -L and -I options])
    fi
  fi
fi

###############################################################################
#
# If didn't already fine necessary files then search.
#
if test "$ax_path_milter_ok" = "no" ; then
  #############################################################################
  #
  # Determine the sendmail base directory and set SENDMAIL_BASE_DIR.
  #
  if test "x$with_sendmail_base" != "x" ; then
    # set SENDMAIL_BASE_DIR to the one specified by--with-sendmail-base
    SENDMAIL_BASE_DIR="$with_sendmail_base"
  else
    AC_MSG_CHECKING([for sendmail base directory in ../ ])
    #
    # --with-sendmail-base is not used, so we will try to determine it
    #
    # 1) List all directories one level down that look like sendmail.
    # 2) Select ones that are sendmail 8.12 or higher (including 8.13
    #    versions when they come out).
    # 3) Replace any single digit last version numbers with a two digit
    #    version number (ie. 8.12.9 becomes 8.12.09).
    # 4) Sort all of the directories found in reverse order.
    # 5) Take the first one (the highest version).
    # 6) Restore the single digit version numbers.
    #
    ac_milter_tmp=`ls -d ../sendmail-8.1* 2> /dev/null | grep '../sendmail-8.1[[2-9]]' | sed 's,\.\([[0-9]]\)$,.0\1,' | sort -r | sed '1q' | sed 's,\.0\([[0-9]]\)$,.\1,'`

    # Convert found version sections to three digit numbers by adding zeros.
    ac_milter_found_version=`echo "$ac_milter_tmp" | sed 's,.*/sendmail-,,;s,\([[0-9]]*\),x\1x,g;s,x\([[0-9]]\)x,x0\1x,g;s,x\([[0-9]][[0-9]]\)x,x0\1x,g;s,x,,g'`

    # If ac_milter_minimum_version is equal to ac_milter_lower_version, then
    # the found version is greater than or equal to the minimum version.
    # Pick the version string that is the lesser of the two.
    # An empty string would be less than anything.
    # In short, ac_milter_version_ok will equal yes if the version is ok,
    # and no otherwise.
    ac_milter_version_ok=`echo "x$ac_milter_minimum_version
x$ac_milter_found_version" | sort | sed '1q' | sed "s,x${ac_milter_minimum_version},yes,;s,x${ac_milter_found_version},no," `

    # If we have something add the current directory to it.
    if test "x$ac_milter_tmp" != "x" ; then
      ac_milter_tmp="`pwd`/$ac_milter_tmp"
    fi

    if test -r "${ac_milter_tmp}/include/libmilter/mfapi.h" && \
       test "$ac_milter_version_ok" = "yes" ; then
      # The file mfapi.h exists so we will use this as SENDMAIL_BASE_DIR.
      AC_MSG_RESULT([yes])
      SENDMAIL_BASE_DIR="$ac_milter_tmp"
    else
      AC_MSG_RESULT([no])
      AC_MSG_CHECKING([for sendmail base from /etc/mail/sendmail.cf])
      #
      # The previous method to find SENDMAIL_BASE_DIR failed, so we will
      # try this method.
      #
      # 1) Check for a line in /etc/mail/sendmail.cf of the form:
      #         ##### in /some/path/sendmail-8.x.x/more/path
      #    This is the directory that the sendmail.cf file was built in.
      # 2) Take the first occurrence if there are more than one.
      # 3) Remove the leading "##### in ".
      # 4) Remove everything after the sendmail-8.x.x path component.
      #
dnl   # Note that the following expression only should not use double
dnl   # square brackets because for some reason, possibly having to
dnl   # do with the pound sign, m4 doesn't convert them to single brackets.
dnl   #
      ac_milter_tmp=`grep "^##### in /" /etc/mail/sendmail.cf 2> /dev/null | grep "/sendmail-8.1" | sed '1q' | sed 's,^##### in ,,' | sed 's,\(/sendmail-8\.[0-9.]*\).*,\1,'`

      # Convert found version sections to three digit numbers by adding zeros.
      ac_milter_found_version=`echo "$ac_milter_tmp" | sed 's,.*/sendmail-,,;s,\([[0-9]]*\),x\1x,g;s,x\([[0-9]]\)x,x0\1x,g;s,x\([[0-9]][[0-9]]\)x,x0\1x,g;s,x,,g'`

      # ac_milter_version_ok will equal yes if the version is ok, otherwise no.
      ac_milter_version_ok=`echo "x$ac_milter_minimum_version
x$ac_milter_found_version" | sort | sed '1q' | sed "s,x${ac_milter_minimum_version},yes,;s,x${ac_milter_found_version},no," `

      if test -r "${ac_milter_tmp}/include/libmilter/mfapi.h" && \
         test "$ac_milter_version_ok" = "yes" ; then
        # The file mfapi.h exists so we will use this as SENDMAIL_BASE_DIR.
        AC_MSG_RESULT([yes])
        SENDMAIL_BASE_DIR="$ac_milter_tmp"
      else
        AC_MSG_RESULT([no])
      fi
    fi
  fi

  #############################################################################
  #
  # Determine the sendmail obj.* directory and set SENDMAIL_OBJ_DIR.
  # We can only do this if we found SENDMAIL_BASE_DIR.
  #
  if test "x$SENDMAIL_BASE_DIR" != "x" ; then
    if test "x$with_sendmail_obj" != "x" ; then
      # set SENDMAIL_OBJ_DIR to the one specified by--with-sendmail-obj
      SENDMAIL_OBJ_DIR="$with_sendmail_obj"
    else
      AC_MSG_CHECKING([for sendmail obj.* subdirectory using Build -M])
      #
      # --with-sendmail-obj is not used, so we will try to determine it
      #
      # Try to run sendmail's Build program with the -M option which will
      # print out the name of the obj. directory for the tool in the
      # directory where it is run from.
      #
      ac_milter_tmp=`(cd ${SENDMAIL_BASE_DIR}/libmilter 1> /dev/null ; ./Build -M ) 2> /dev/null`

      if test -f "${ac_milter_tmp}/libmilter.a" ; then
        # libmilter.a exists so this is the one we will choose
        AC_MSG_RESULT([yes])
        # Remove beginning and end of path from obj.* directory.
        SENDMAIL_OBJ_DIR=`echo "$ac_milter_tmp" | sed 's,/libmilter$,,;s,.*/,,'`
      else
        AC_MSG_RESULT([no])
        AC_MSG_CHECKING([for sendmail obj.* subdirectory using ls])
        #
        # List all directories of the form "obj." in the sendmail base
        # directory, and choose the one with the latest modification date.
        #
        ac_milter_tmp=`ls -dt ${SENDMAIL_BASE_DIR}/obj.*/libmilter 2> /dev/null | sed '1q'`

        if test -f "${ac_milter_tmp}/libmilter.a" ; then
          # libmilter.a exists so this is the one we will choose
          AC_MSG_RESULT([yes])
          # Remove beginning and end of path from obj.* directory.
          SENDMAIL_OBJ_DIR=`echo "$ac_milter_tmp" | sed 's,/libmilter$,,;s,.*/,,'`
        else
          AC_MSG_RESULT([no])
        fi
      fi
    fi
  fi

  #############################################################################
  #
  # If we have both SENDMAIL_BASE_DIR and SENDMAIL_OBJ_DIR we will check
  # for the necessary files.
  #
  if test "x$SENDMAIL_BASE_DIR" != "x" && \
     test "x$SENDMAIL_OBJ_DIR" != "x" ; then

    # Save and modify CPPFLAGS.
    ac_milter_save_CPPFLAGS="$CPPFLAGS"
    MILTER_CPPFLAGS="-I$SENDMAIL_BASE_DIR/include"
    CPPFLAGS="$CPPFLAGS $MILTER_CPPFLAGS"

    # Save and modify LDFLAGS.
    ac_milter_save_LDFLAGS="$LDLFAGS"
    MILTER_LDFLAGS="-L${SENDMAIL_BASE_DIR}/${SENDMAIL_OBJ_DIR}/libmilter"
    LDFLAGS="$MILTER_LDFLAGS $LDFLAGS"

    AC_CHECK_HEADER([libmilter/mfapi.h],[
      AC_CHECK_LIB([milter],[smfi_main],[
        # both tests succeeded so add -lmilter to the libraries to link
        MILTER_LIBS="-lmilter"

        # indicate success
        ax_path_milter_ok=yes
      ])
    ])

    # Restore the modified environment
    CPPFLAGS="$ac_milter_save_CPPFLAGS"
    LDFLAGS="$ac_milter_save_LDFLAGS"

  fi
fi

# If failure, clear MILTER_LIBS, MILTER_LDFLAGS and MILTER_CPPFLAGS.
if test "$ax_path_milter_ok" = "no" ; then
  MILTER_CPPFLAGS=""
  MILTER_LIBS=""
  MILTER_LDFLAGS=""
fi

# export these to the make environment
AC_SUBST([MILTER_LIBS])
AC_SUBST([MILTER_CPPFLAGS])
AC_SUBST([MILTER_LDFLAGS])
AC_SUBST([SENDMAIL_BASE_DIR])
AC_SUBST([SENDMAIL_OBJ_DIR])

# Indicate status of checking for libmilter stuff.
AC_MSG_CHECKING([if files required by libmilter are present])

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND.
if test "$ax_path_milter_ok" = "yes" ; then
  AC_MSG_RESULT([yes])
  $2
else
  AC_MSG_RESULT([no])
  $3
fi

])dnl
