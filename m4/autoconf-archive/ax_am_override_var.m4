# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_am_override_var.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_AM_OVERRIDE_VAR([varname1 varname ... ])
#   AX_AM_OVERRIDE_FINALIZE
#
# DESCRIPTION
#
#   This autoconf macro generalizes the approach given in
#   <http://lists.gnu.org/archive/html/automake/2005-09/msg00108.html> which
#   moves user specified values for variable 'varname' given at configure
#   time into the corresponding AM_${varname} variable and clears out
#   'varname', allowing further manipulation by the configure script so that
#   target specific variables can be given specialized versions.  'varname
#   may still be specified on the make command line and will be appended as
#   usual.
#
#   As an example usage, consider a project which might benefit from
#   different compiler flags for different components. Typically this is
#   done via target specific flags, e.g.
#
#    libgtest_la_CXXFLAGS    =                        \
#                     -I $(top_srcdir)/tests          \
#                     -I $(top_builddir)/tests        \
#                     $(GTEST_CXXFLAGS)
#
#   automake will automatically append $(CXXFLAGS) -- provided by the user
#   -- to the build rule for libgtest_la.  That might be problematic, as
#   CXXFLAGS may contain compiler options which are inappropriate for
#   libgtest_la.
#
#   The approach laid out in the referenced mailing list message is to
#   supply a base value for a variable during _configure_ time, during which
#   it is possible to amend it for specific targets. The user may
#   subsequently specify a value for the variable during _build_ time, which
#   make will apply (via the standard automake rules) to all appropriate
#   targets.
#
#   For example,
#
#    AX_AM_OVERRIDE_VAR([CXXFLAGS])
#
#   will store the value of CXXFLAGS specified at configure time into the
#   AM_CXXFLAGS variable, AC_SUBST it, and clear CXXFLAGS. configure may
#   then create a target specific set of flags based upon AM_CXXFLAGS, e.g.
#
#    # googletest uses variadic macros, which g++ -pedantic-errors
#    # is very unhappy about
#    AC_SUBST([GTEST_CXXFLAGS],
#       [`AS_ECHO_N(["$AM_CXXFLAGS"]) \
#             | sed s/-pedantic-errors/-pedantic/`
#        ]
#     )
#
#   which would be used in a Makefile.am as above.  Since CXXFLAGS is
#   cleared, the configure time value will not affect the build for
#   libgtest_la.
#
#   Prior to _any other command_ which may set ${varname}, call
#
#    AX_AM_OVERRIDE_VAR([varname])
#
#   This will preserve the value (if any) passed to configure in
#   AM_${varname} and AC_SUBST([AM_${varname}).  You may pass a space
#   separated list of variable names, or may call AX_AM_OVERRIDE_VAR
#   multiple times for the same effect.
#
#   If any subsequent configure commands set ${varname} and you wish to
#   capture the resultant value into AM_${varname} in the case where
#   ${varname} was _not_ provided at configure time,  call
#
#    AX_AM_OVERRIDE_FINALIZE
#
#   after _all_ commands which might affect any of the variables specified
#   in calls to AX_AM_OVERRIDE_VAR.  This need be done only once, but
#   repeated calls will not cause harm.
#
#   There is a bit of trickery required to allow further manipulation of the
#   AM_${varname} in a Makefile.am file.  If AM_CFLAGS is used as is in a
#   Makefile.am, e.g.
#
#    libfoo_la_CFLAGS = $(AM_CFLAGS)
#
#   then automake will emit code in Makefile.in which sets AM_CFLAGS from
#   the configure'd value.
#
#   If however, AM_CFLAGS is manipulated (i.e. appended to), you will have
#   to explicitly arrange for the configure'd value to be substituted:
#
#    AM_CFLAGS = @AM_CFLAGS@
#    AM_CFLAGS += -lfoo
#
#   or else automake will complain about using += before =.
#
# LICENSE
#
#   Copyright (c) 2013 Smithsonian Astrophysical Observatory
#   Copyright (c) 2013 Diab Jerius <djerius@cfa.harvard.edu>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 2

AC_DEFUN([_AX_AM_OVERRIDE_INITIALIZE],
[
        m4_define([_mst_am_override_vars],[])
])


# _AX_AM_OVERRIDE_VAR(varname)
AC_DEFUN([_AX_AM_OVERRIDE_VAR],
[
  m4_define([_mst_am_override_vars], m4_defn([_mst_am_override_vars]) $1 )
  _mst_am_override_$1_set=false

  AS_IF( [test "${$1+set}" = set],
         [AC_SUBST([AM_$1],["$$1"])
          $1=
          _mst_am_override_$1_set=:
         ]
  )
]) # _AX_AM_OVERRIDE_VAR

# _AX_AM_OVERRIDE_FINALIZE(varname)
AC_DEFUN([_AX_AM_OVERRIDE_FINALIZE],
[
  AS_IF([$_mst_am_override_$1_set = :],
        [],
        [AC_SUBST([AM_$1],["$$1"])
         $1=
         _mst_am_override_$1_set=
        ]
  )
  AC_SUBST($1)
]) # _AX_AM_OVERRIDE_FINALIZE

AC_DEFUN([AX_AM_OVERRIDE_VAR],
[
  AC_REQUIRE([_AX_AM_OVERRIDE_INITIALIZE])
  m4_map_args_w([$1],[_AX_AM_OVERRIDE_VAR(],[)])
])# AX_OVERRIDE_VAR


# AX_AM_OVERRIDE_FINALIZE
AC_DEFUN([AX_AM_OVERRIDE_FINALIZE],
[
  AC_REQUIRE([_AX_AM_OVERRIDE_INITIALIZE])
  m4_map_args_w(_mst_am_override_vars,[_AX_AM_OVERRIDE_FINALIZE(],[)])
]) # AX_AM_OVERRIDE_FINALIZE
