# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_lib_netcdf4.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_NETCDF4([serial/parallel])
#
# DESCRIPTION
#
#   This macro provides tests of the availability of the NetCDF v4 library.
#
#   The optional macro argument should be either 'serial' or 'parallel'. The
#   macro will call nc-config to check the output of the '--has-pnetcdf'
#   option and error out if the requested parallel isn't supported.
#
#   If the optional argument is omitted, no check is made to see if NetCDF
#   has parallel support.
#
#   The macro adds a --with-netcdf4 option accepting one of three values:
#
#     no   - do not check for the NetCDF4 library.
#     yes  - do check for NetCDF4 library in standard locations.
#     path - installation prefix for NetCDF version 4.
#
#   If NetCDF4 is successfully found, this macro calls
#
#     AC_SUBST(NETCDF4_VERSION)
#     AC_SUBST(NETCDF4_CC)
#     AC_SUBST(NETCDF4_CFLAGS)
#     AC_SUBST(NETCDF4_CPPFLAGS)
#     AC_SUBST(NETCDF4_LDFLAGS)
#     AC_SUBST(NETCDF4_LIBS)
#     AC_SUBST(NETCDF4_FC)
#     AC_SUBST(NETCDF4_FFLAGS)
#     AC_SUBST(NETCDF4_FLIBS)
#     AC_DEFINE(HAVE_NETCDF4)
#
#   It also sets
#
#     with_netcdf4="yes"
#     with_netcdf4_fortran="yes"    (if NetCDF has Fortran support)
#     with_netcdf4_parallel="yes"   (if NetCDF has MPI support)
#
#   If NetCDF4 is disabled or not found, this macros sets
#
#     with_netcdf4="no"
#     with_netcdf4_fortran="no"
#
#   Note it does not set with_netcdf4_parallel in this case.
#
#   Your configuration script can test $with_netcdf4 to take any further
#   actions. NETCDF4_{C,CPP,LD}FLAGS may be used when building with C or
#   C++. NETCDF4_F{FLAGS,LIBS} and NETCDF4_LDFLAGS should be used when
#   building Fortran applications.
#
#   To use the macro, one would code one of the following in "configure.ac"
#   before AC_OUTPUT:
#
#     1) dnl Check for NetCDF4 support
#        AX_LIB_NETCDF4()
#
#     2) dnl Check for serial NetCDF4 support
#        AX_LIB_NETCDF4([serial])
#
#     3) dnl Check for parallel NetCDF4 support
#        AX_LIB_NETCDF4([parallel])
#
#   One could test $with_netcdf4 for the outcome or display it as follows
#
#     echo "NetCDF v4 support:  $with_netcdf4"
#
#   One could also for example, override the default CC in "configure.ac" to
#   enforce compilation with the compiler that NetCDF v4 was built with:
#
#     AX_LIB_NETCDF4([parallel])
#     if test "$with_netcdf4" = "yes"; then
#             CC="$NETCDF4_CC"
#     else
#             AC_MSG_ERROR([Unable to find NetCDF4, we need parallel NetCDF4.])
#     fi
#
# LICENSE
#
#   Copyright (c) 2016 Timothy Brown <tbrown@freeshell.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 2

AC_DEFUN([AX_LIB_NETCDF4], [

AC_REQUIRE([AC_PROG_SED])
AC_REQUIRE([AC_PROG_AWK])
AC_REQUIRE([AC_PROG_GREP])

dnl Check first argument is one of the recognized values.
dnl Fail eagerly if is incorrect as this simplifies case statements below.
if   test "m4_normalize(m4_default([$1],[]))" = ""        ; then
    netcdf4_requested_mode="serial"
elif test "m4_normalize(m4_default([$1],[]))" = "serial"  ; then
    netcdf4_requested_mode="serial"
elif test "m4_normalize(m4_default([$1],[]))" = "parallel"; then
    netcdf4_requested_mode="parallel"
else
    AC_MSG_ERROR([
Unrecognized value for AX[]_LIB_NETCDF4 within configure.ac.
If supplied, argument 1 must be either 'serial' or 'parallel'.
])
fi

dnl Add a default --with-netcdf4 configuration option.
AC_ARG_WITH([netcdf4],
  AS_HELP_STRING(
    [--with-netcdf4=[yes/no/PATH]],
    m4_case(m4_normalize([$1]),
            [serial],   [base directory of serial NetCDF4 installation],
            [parallel], [base directory of parallel NetCDF4 installation],
            [base directory of NetCDF4 installation])
  ),
  [if test "$withval" = "no"; then
     with_netcdf4="no"
   elif test "$withval" = "yes"; then
     with_netcdf4="yes"
   else
     with_netcdf4="yes"
     NETCDF4_PREFIX="${withval}"
     NC_CONFIG="${withval}/bin/nc-config"
   fi],
   [with_netcdf4="yes"]
)

dnl Set defaults to blank
NETCDF4_CC=""
NETCDF4_VERSION=""
NETCDF4_CFLAGS=""
NETCDF4_CPPFLAGS=""
NETCDF4_LDFLAGS=""
NETCDF4_LIBS=""
NETCDF4_FC=""
NETCDF4_FFLAGS=""
NETCDF4_FLIBS=""

dnl Try and find NetCDF4 tools and options.
if test "$with_netcdf4" = "yes"; then
    if test -z "$NC_CONFIG"; then
        dnl Check to see if NC_CONFIG is in the path.
        AC_PATH_PROGS([NC_CONFIG], [nc-config], [])
        NETCDF4_PREFIX=$(AS_DIRNAME([$(AS_DIRNAME(["$NC_CONFIG"]))]))
    else
        AC_MSG_CHECKING([Using provided NetCDF4 prefix])
        AC_MSG_RESULT([$NC_CONFIG])
    fi

    AC_MSG_CHECKING([for NetCDF4 libraries])

    if test ! -f "$NC_CONFIG" || test ! -x "$NC_CONFIG"; then
        AC_MSG_RESULT([no])
        AC_MSG_WARN([

Unable to locate NetCDF4 compilation helper script 'nc-config'.
Please specify --with-netcdf4=<LOCATION> as the full path prefix
where NetCDF4 has been installed.
NetCDF4 support is being disabled (equivalent to --with-netcdf4=no).
])
        with_netcdf4="no"
        with_netcdf4_fortran="no"
    else
        dnl Get the actual compiler used
        NETCDF4_CC=$(eval $NC_CONFIG --cc | $AWK '{print $[]1}')
        if test "$NETCDF4_CC" = "ccache"; then
            NETCDF4_CC=$(eval $NC_CONFIG --cc | $AWK '{print $[]2}')
        fi

        dnl Look for version
        NETCDF4_VERSION=$(eval $NC_CONFIG --version | $AWK '{print $[]2}')

        dnl Look for the CFLAGS
        NETCDF4_CFLAGS=$(eval $NC_CONFIG --cflags)

        dnl Look for the LIBS and LDFLAGS
        NETCDF4_tmp_clibs=$(eval $NC_CONFIG --libs)

        dnl Sort out the tmp libs based on their prefixes
        for arg in $NETCDF4_tmp_clibs ; do
          case "$arg" in
            -L*) echo $NETCDF4_LDFLAGS | $GREP -e "$arg" 2>&1 >/dev/null \
                   || NETCDF4_LDFLAGS="$arg $NETCDF4_LDFLAGS"
              ;;
            -l*) echo $NETCDF4_LIBS | $GREP -e "$arg" 2>&1 >/dev/null \
                   || NETCDF4_LIBS="$arg $NETCDF4_LIBS"
              ;;
          esac
        done

        AC_MSG_RESULT([yes (version $[NETCDF4_VERSION])])

        dnl See if we need (and have) parallel support
        if test "$netcdf4_requested_mode" = "parallel" ; then
            with_netcdf4_parallel=$(eval $NC_CONFIG --has-pnetcdf)
            if test "$with_netcdf4_parallel" = "no" ; then
                AC_MSG_ERROR([
parallel NetCDF4 is not supported (while it was requested)
])
            fi
        fi

        dnl See if we can compile
        ax_lib_netcdf4_save_CC=$CC
        ax_lib_netcdf4_save_CPPFLAGS=$CPPFLAGS
        ax_lib_netcdf4_save_LIBS=$LIBS
        ax_lib_netcdf4_save_LDFLAGS=$LDFLAGS
        CC=$NETCDF4_CC
        CFLAGS=$NETCDF4_CFLAGS
        LIBS=$NETCDF4_LIBS
        LDFLAGS=$NETCDF4_LDFLAGS
        AC_CHECK_HEADER([netcdf.h], [ac_cv_netcdf4_h=yes], [ac_cv_netcdf4_h=no])
        AC_CHECK_LIB([netcdf], [nc_create], [ac_cv_libnetcdf4=yes],
                     [ac_cv_libnetcdf4=no])
        if test "$ac_cv_netcdf4_h" = "no" && \
           test "$ac_cv_libnetcdf4" = "no" ; then
            AC_MSG_WARN([Unable to compile NetCDF4 test program])
        fi

        CC=$ax_lib_netcdf4_save_CC
        CFLAGS=$ax_lib_netcdf4_save_CFLAGS
        LIBS=$ax_lib_netcdf4_save_LIBS
        LDFLAGS=$ax_lib_hdf5_save_LDFLAGS


        AC_MSG_CHECKING([for matching NetCDF4 Fortran libraries])
        NF_CONFIG="${NETCDF4_PREFIX}/bin/nf-config"
        if test ! -f "$NF_CONFIG" || test ! -x "$NF_CONFIG"; then
            AC_MSG_RESULT([no])
            with_netcdf4_fortran="no"
        else
            NETCDF_FVERSION=$(eval $NF_CONFIG --version | $AWK '{print $[]2}')
            AC_MSG_RESULT([yes (version $[NETCDF_FVERSION])])
            NETCDF4_FC=$(eval $NF_CONFIG --fc | $AWK '{print $[]1}')
            if test "$NETCDF4_FC" = "ccache"; then
                NETCDF4_FC=$(eval $NF_CONFIG --fc | $AWK '{print $[]2}')
            fi
            dnl Look for the FFLAGS
            NETCDF4_FFLAGS=$(eval $NC_CONFIG --fflags)

            dnl Look for the FLIBS and LDFLAGS
            NETCDF4_tmp_flibs=$(eval $NC_CONFIG --flibs)

            dnl Sort out the tmp libs based on their prefixes
            for arg in $NETCDF4_tmp_flibs ; do
              case "$arg" in
                -L*) echo $NETCDF4_LDFLAGS | $GREP -e "$arg" 2>&1 >/dev/null \
                       || NETCDF4_LDFLAGS="$arg $NETCDF4_LDFLAGS"
                  ;;
                -l*) echo $NETCDF4_FLIBS | $GREP -e "$arg" 2>&1 >/dev/null \
                       || NETCDF4_FLIBS="$arg $NETCDF4_FLIBS"
                  ;;
               esac
            done
            with_netcdf4_fortran="yes"
        fi

        AC_SUBST([NETCDF4_VERSION])
        AC_SUBST([NETCDF4_CC])
        AC_SUBST([NETCDF4_CFLAGS])
        AC_SUBST([NETCDF4_LDFLAGS])
        AC_SUBST([NETCDF4_LIBS])
        AC_SUBST([NETCDF4_FC])
        AC_SUBST([NETCDF4_FFLAGS])
        AC_SUBST([NETCDF4_FLIBS])
        AC_DEFINE([HAVE_NETCDF4], [1], [Defined if you have NETCDF4 support])
    fi
fi
])
