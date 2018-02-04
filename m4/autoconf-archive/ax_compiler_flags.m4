# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_compiler_flags.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_COMPILER_FLAGS([CFLAGS-VARIABLE], [LDFLAGS-VARIABLE], [IS-RELEASE], [EXTRA-BASE-CFLAGS], [EXTRA-YES-CFLAGS], [UNUSED], [UNUSED], [UNUSED], [EXTRA-BASE-LDFLAGS], [EXTRA-YES-LDFLAGS], [UNUSED], [UNUSED], [UNUSED])
#
# DESCRIPTION
#
#   Check for the presence of an --enable-compile-warnings option to
#   configure, defaulting to "error" in normal operation, or "yes" if
#   IS-RELEASE is equal to "yes".  Return the value in the variable
#   $ax_enable_compile_warnings.
#
#   Depending on the value of --enable-compile-warnings, different compiler
#   warnings are checked to see if they work with the current compiler and,
#   if so, are appended to CFLAGS-VARIABLE and LDFLAGS-VARIABLE.  This
#   allows a consistent set of baseline compiler warnings to be used across
#   a code base, irrespective of any warnings enabled locally by individual
#   developers.  By standardising the warnings used by all developers of a
#   project, the project can commit to a zero-warnings policy, using -Werror
#   to prevent compilation if new warnings are introduced.  This makes
#   catching bugs which are flagged by warnings a lot easier.
#
#   By providing a consistent --enable-compile-warnings argument across all
#   projects using this macro, continuous integration systems can easily be
#   configured the same for all projects.  Automated systems or build
#   systems aimed at beginners may want to pass the --disable-Werror
#   argument to unconditionally prevent warnings being fatal.
#
#   --enable-compile-warnings can take the values:
#
#    * no:      Base compiler warnings only; not even -Wall.
#    * yes:     The above, plus a broad range of useful warnings.
#    * error:   The above, plus -Werror so that all warnings are fatal.
#               Use --disable-Werror to override this and disable fatal
#               warnings.
#
#   The set of base and enabled flags can be augmented using the
#   EXTRA-*-CFLAGS and EXTRA-*-LDFLAGS variables, which are tested and
#   appended to the output variable if --enable-compile-warnings is not
#   "no". Flags should not be disabled using these arguments, as the entire
#   point of AX_COMPILER_FLAGS is to enforce a consistent set of useful
#   compiler warnings on code, using warnings which have been chosen for low
#   false positive rates.  If a compiler emits false positives for a
#   warning, a #pragma should be used in the code to disable the warning
#   locally. See:
#
#     https://gcc.gnu.org/onlinedocs/gcc-4.9.2/gcc/Diagnostic-Pragmas.html#Diagnostic-Pragmas
#
#   The EXTRA-* variables should only be used to supply extra warning flags,
#   and not general purpose compiler flags, as they are controlled by
#   configure options such as --disable-Werror.
#
#   IS-RELEASE can be used to disable -Werror when making a release, which
#   is useful for those hairy moments when you just want to get the release
#   done as quickly as possible.  Set it to "yes" to disable -Werror. By
#   default, it uses the value of $ax_is_release, so if you are using the
#   AX_IS_RELEASE macro, there is no need to pass this parameter. For
#   example:
#
#     AX_IS_RELEASE([git-directory])
#     AX_COMPILER_FLAGS()
#
#   CFLAGS-VARIABLE defaults to WARN_CFLAGS, and LDFLAGS-VARIABLE defaults
#   to WARN_LDFLAGS.  Both variables are AC_SUBST-ed by this macro, but must
#   be manually added to the CFLAGS and LDFLAGS variables for each target in
#   the code base.
#
#   If C++ language support is enabled with AC_PROG_CXX, which must occur
#   before this macro in configure.ac, warning flags for the C++ compiler
#   are AC_SUBST-ed as WARN_CXXFLAGS, and must be manually added to the
#   CXXFLAGS variables for each target in the code base.  EXTRA-*-CFLAGS can
#   be used to augment the base and enabled flags.
#
#   Warning flags for g-ir-scanner (from GObject Introspection) are
#   AC_SUBST-ed as WARN_SCANNERFLAGS.  This variable must be manually added
#   to the SCANNERFLAGS variable for each GIR target in the code base.  If
#   extra g-ir-scanner flags need to be enabled, the AX_COMPILER_FLAGS_GIR
#   macro must be invoked manually.
#
#   AX_COMPILER_FLAGS may add support for other tools in future, in addition
#   to the compiler and linker.  No extra EXTRA-* variables will be added
#   for those tools, and all extra support will still use the single
#   --enable-compile-warnings configure option.  For finer grained control
#   over the flags for individual tools, use AX_COMPILER_FLAGS_CFLAGS,
#   AX_COMPILER_FLAGS_LDFLAGS and AX_COMPILER_FLAGS_* for new tools.
#
#   The UNUSED variables date from a previous version of this macro, and are
#   automatically appended to the preceding non-UNUSED variable. They should
#   be left empty in new uses of the macro.
#
# LICENSE
#
#   Copyright (c) 2014, 2015 Philip Withnall <philip@tecnocode.co.uk>
#   Copyright (c) 2015 David King <amigadave@amigadave.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.  This file is offered as-is, without any
#   warranty.

#serial 14

# _AX_COMPILER_FLAGS_LANG([LANGNAME])
m4_defun([_AX_COMPILER_FLAGS_LANG],
[m4_ifdef([_AX_COMPILER_FLAGS_LANG_]$1[_enabled], [],
          [m4_define([_AX_COMPILER_FLAGS_LANG_]$1[_enabled], [])dnl
           AX_REQUIRE_DEFINED([AX_COMPILER_FLAGS_]$1[FLAGS])])dnl
])

AC_DEFUN([AX_COMPILER_FLAGS],[
    # C support is enabled by default.
    _AX_COMPILER_FLAGS_LANG([C])
    # Only enable C++ support if AC_PROG_CXX is called. The redefinition of
    # AC_PROG_CXX is so that a fatal error is emitted if this macro is called
    # before AC_PROG_CXX, which would otherwise cause no C++ warnings to be
    # checked.
    AC_PROVIDE_IFELSE([AC_PROG_CXX],
                      [_AX_COMPILER_FLAGS_LANG([CXX])],
                      [m4_define([AC_PROG_CXX], defn([AC_PROG_CXX])[_AX_COMPILER_FLAGS_LANG([CXX])])])
    AX_REQUIRE_DEFINED([AX_COMPILER_FLAGS_LDFLAGS])

    # Default value for IS-RELEASE is $ax_is_release
    ax_compiler_flags_is_release=m4_tolower(m4_normalize(ifelse([$3],,
                                                                [$ax_is_release],
                                                                [$3])))

    AC_ARG_ENABLE([compile-warnings],
                  AS_HELP_STRING([--enable-compile-warnings=@<:@no/yes/error@:>@],
                                 [Enable compiler warnings and errors]),,
                  [AS_IF([test "$ax_compiler_flags_is_release" = "yes"],
                         [enable_compile_warnings="yes"],
                         [enable_compile_warnings="error"])])
    AC_ARG_ENABLE([Werror],
                  AS_HELP_STRING([--disable-Werror],
                                 [Unconditionally make all compiler warnings non-fatal]),,
                  [enable_Werror=maybe])

    # Return the user's chosen warning level
    AS_IF([test "$enable_Werror" = "no" -a \
                "$enable_compile_warnings" = "error"],[
        enable_compile_warnings="yes"
    ])

    ax_enable_compile_warnings=$enable_compile_warnings

    AX_COMPILER_FLAGS_CFLAGS([$1],[$ax_compiler_flags_is_release],
                             [$4],[$5 $6 $7 $8])
    m4_ifdef([_AX_COMPILER_FLAGS_LANG_CXX_enabled],
             [AX_COMPILER_FLAGS_CXXFLAGS([WARN_CXXFLAGS],
                                         [$ax_compiler_flags_is_release],
                                         [$4],[$5 $6 $7 $8])])
    AX_COMPILER_FLAGS_LDFLAGS([$2],[$ax_compiler_flags_is_release],
                              [$9],[$10 $11 $12 $13])
    AX_COMPILER_FLAGS_GIR([WARN_SCANNERFLAGS],[$ax_compiler_flags_is_release])
])dnl AX_COMPILER_FLAGS
