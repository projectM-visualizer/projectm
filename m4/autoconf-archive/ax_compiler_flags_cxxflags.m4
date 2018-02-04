# ===============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_compiler_flags_cxxflags.html
# ===============================================================================
#
# SYNOPSIS
#
#   AX_COMPILER_FLAGS_CXXFLAGS([VARIABLE], [IS-RELEASE], [EXTRA-BASE-FLAGS], [EXTRA-YES-FLAGS])
#
# DESCRIPTION
#
#   Add warning flags for the C++ compiler to VARIABLE, which defaults to
#   WARN_CXXFLAGS.  VARIABLE is AC_SUBST-ed by this macro, but must be
#   manually added to the CXXFLAGS variable for each target in the code
#   base.
#
#   This macro depends on the environment set up by AX_COMPILER_FLAGS.
#   Specifically, it uses the value of $ax_enable_compile_warnings to decide
#   which flags to enable.
#
# LICENSE
#
#   Copyright (c) 2015 David King <amigadave@amigadave.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.  This file is offered as-is, without any
#   warranty.

#serial 10

AC_DEFUN([AX_COMPILER_FLAGS_CXXFLAGS],[
    AC_REQUIRE([AC_PROG_SED])
    AX_REQUIRE_DEFINED([AX_APPEND_COMPILE_FLAGS])
    AX_REQUIRE_DEFINED([AX_APPEND_FLAG])
    AX_REQUIRE_DEFINED([AX_CHECK_COMPILE_FLAG])

    # Variable names
    m4_define([ax_warn_cxxflags_variable],
              [m4_normalize(ifelse([$1],,[WARN_CXXFLAGS],[$1]))])

    AC_LANG_PUSH([C++])

    # Always pass -Werror=unknown-warning-option to get Clang to fail on bad
    # flags, otherwise they are always appended to the warn_cxxflags variable,
    # and Clang warns on them for every compilation unit.
    # If this is passed to GCC, it will explode, so the flag must be enabled
    # conditionally.
    AX_CHECK_COMPILE_FLAG([-Werror=unknown-warning-option],[
        ax_compiler_flags_test="-Werror=unknown-warning-option"
    ],[
        ax_compiler_flags_test=""
    ])

    # Check that -Wno-suggest-attribute=format is supported
    AX_CHECK_COMPILE_FLAG([-Wno-suggest-attribute=format],[
        ax_compiler_no_suggest_attribute_flags="-Wno-suggest-attribute=format"
    ],[
        ax_compiler_no_suggest_attribute_flags=""
    ])

    # Base flags
    AX_APPEND_COMPILE_FLAGS([ dnl
        -fno-strict-aliasing dnl
        $3 dnl
    ],ax_warn_cxxflags_variable,[$ax_compiler_flags_test])

    AS_IF([test "$ax_enable_compile_warnings" != "no"],[
        # "yes" flags
        AX_APPEND_COMPILE_FLAGS([ dnl
            -Wall dnl
            -Wextra dnl
            -Wundef dnl
            -Wwrite-strings dnl
            -Wpointer-arith dnl
            -Wmissing-declarations dnl
            -Wredundant-decls dnl
            -Wno-unused-parameter dnl
            -Wno-missing-field-initializers dnl
            -Wformat=2 dnl
            -Wcast-align dnl
            -Wformat-nonliteral dnl
            -Wformat-security dnl
            -Wsign-compare dnl
            -Wstrict-aliasing dnl
            -Wshadow dnl
            -Winline dnl
            -Wpacked dnl
            -Wmissing-format-attribute dnl
            -Wmissing-noreturn dnl
            -Winit-self dnl
            -Wredundant-decls dnl
            -Wmissing-include-dirs dnl
            -Wunused-but-set-variable dnl
            -Warray-bounds dnl
            -Wreturn-type dnl
            -Wno-overloaded-virtual dnl
            -Wswitch-enum dnl
            -Wswitch-default dnl
            $4 dnl
            $5 dnl
            $6 dnl
            $7 dnl
        ],ax_warn_cxxflags_variable,[$ax_compiler_flags_test])
    ])
    AS_IF([test "$ax_enable_compile_warnings" = "error"],[
        # "error" flags; -Werror has to be appended unconditionally because
        # it's not possible to test for
        #
        # suggest-attribute=format is disabled because it gives too many false
        # positives
        AX_APPEND_FLAG([-Werror],ax_warn_cxxflags_variable)

        AX_APPEND_COMPILE_FLAGS([ dnl
            [$ax_compiler_no_suggest_attribute_flags] dnl
        ],ax_warn_cxxflags_variable,[$ax_compiler_flags_test])
    ])

    # In the flags below, when disabling specific flags, always add *both*
    # -Wno-foo and -Wno-error=foo. This fixes the situation where (for example)
    # we enable -Werror, disable a flag, and a build bot passes CXXFLAGS=-Wall,
    # which effectively turns that flag back on again as an error.
    for flag in $ax_warn_cxxflags_variable; do
        AS_CASE([$flag],
                [-Wno-*=*],[],
                [-Wno-*],[
                    AX_APPEND_COMPILE_FLAGS([-Wno-error=$(AS_ECHO([$flag]) | $SED 's/^-Wno-//')],
                                            ax_warn_cxxflags_variable,
                                            [$ax_compiler_flags_test])
                ])
    done

    AC_LANG_POP([C++])

    # Substitute the variables
    AC_SUBST(ax_warn_cxxflags_variable)
])dnl AX_COMPILER_FLAGS_CXXFLAGS
