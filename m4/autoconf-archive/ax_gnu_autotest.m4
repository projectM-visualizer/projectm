# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_gnu_autotest.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_GNU_AUTOTEST([testdir = `tests'], [testsuites = `testsuite'],
#                   [atlocal-sources = `'], [gen-package = `yes'],
#                   [force = `no'])
#
# DESCRIPTION
#
#   Sets up one or multiple GNU Autotest test suites [1].
#
#   TL;DR:
#
#     * Write tests/testsuite.at as normal
#     * Add to configure.ac: AX_GNU_AUTOTEST
#     * Add to Makefile.am or Makefile.in in top_srcdir:
#       @AX_GNU_AUTOTEST_DEFAULT@
#     * autoreconf && ./configure && make check
#
#   GNU Autotest is a very powerful testing framework to script executing
#   binaries, observing their output and logging failures, all in the same
#   portable manner as configure itself. But the only help given the
#   developer in setting up the framework is the AC_CONFIG_TESTDIR()
#   command, which leaves several additional steps up to the developer
#   copying examples from the manual:
#
#     * generating the "package.m4" file used in generating the "testsuite"
#     * generating the "testsuite" executable by calling autom4te
#     * adding Makefile rules to keep both "package.m4" and "testsuite"
#       current
#     * figuring out how to do all the above if the Makefile does not reside
#       in the testdir
#
#   This command takes care of all of the above. It is designed to be called
#   multiple times for different testdir directories, to have multiple
#   testsuites scripts per directory and to handle the optional "package.m4"
#   and "atlocal" (re)generation.
#
#   The various actions taken by this command happen in different phases of
#   the build process:
#
#     1) During the autoconf execution, generate the testsuites and any
#        "package.m4" files. So these are already available before configure
#        ran. The reasoning for deviating from the examples in [1] is that
#        these files are distributed and must be placed in the srcdir: it
#        seems cleaner to not generate anything in srcdir during any of the
#        later phases.
#     2) During the config.status execution (following the configure
#        execution), generate "atconfig" and any "atlocal" files in the
#        buildir. The "atconfig" generation is handled by calling
#        AC_CONFIG_TESTDIR() so the developer does not have to do so
#        themselves.
#     3) During the execution of make, several additional rules and file
#        lists are made available via AC_SUBST(). The rules are intended
#        to be called where appropriate (e.g. make check can depend on
#        check-autotest) and the file lists are intended to be added
#        to the appropriate lists (i.e. to DISTCLEANFILES and EXTRA_DIST).
#
#   Description of AX_GNU_AUTOTEST() arguments:
#
#     * testdir: directory-name containing the testsuites. AX_GNU_AUTOTEST()
#       must be called exactly once for each directory containing testsuites.
#       If empty, defaults to "tests".
#     * testsuites: space-separated list of words, where each word is the
#       name of a test suite script optionally followed by a colon and the
#       name of the scripts source file. If the source file is not given,
#       it defaults to the script name suffixed by ".at". So these words
#       are all equivalent: "foo", "foo:" and "foo:foo.at". If the argument
#       is empty, it defaults to "testsuite". The script filenames must not
#       contain any path, but that is allowed for the source filenames.
#     * atlocal-sources: space- or colon-separated list of filenames, which
#       are registered with AC_CONFIG_FILES() as the sources of atlocal.
#       If empty, no atlocal file is generated.
#     * gen-package: boolean ("yes" or "no") indicating whether "package.m4"
#       should be generated. If empty, defaults to "yes".
#     * force: boolean ("yes" or "no") whether to treat errors in arguments
#       as errors and abort (for "no") or whether to ignore any such errors
#       (for "yes"). If empty, defaults to "no".
#
#   All filenames above must be relative. The testdir name is interpreted
#   relative to top_srcdir. All other names are interpreted relative to the
#   testdir. The boolean values are interpreted as "yes" for any non-empty
#   value except "0", "n", "no" and their mixed-case variants.
#
#   Description of Makefile.am / Makefile.in substitutions:
#
#     * AX_GNU_AUTOTEST_RULES: the make rules provided here. Substitute this
#       in a separate line.
#     * AX_GNU_AUTOTEST_DCLEAN: the list of files to be added to
#       DISTCLEANFILES.
#     * AX_GNU_AUTOTEST_DIST: the list of files to be added to EXTRA_DIST.
#     * AX_GNU_AUTOTEST_DEFAULT: includes all other substitutions and uses
#       them in a "default" way.
#
#   All must be used as a substitution (@...@) instead of as a variable
#   ($(...) or ${...}). These substitutions are generated multiple times,
#   once for each directory where an affected Makefile may be located. All
#   substitutions start with the base as given above but may have suffixes
#   for the directories. Assuming this example in configure.ac:
#
#     AX_GNU_AUTOTEST([foo/bar])
#     AX_GNU_AUTOTEST([baz])
#
#   Then the following substitutions are available (where <base> stands for
#   one of the above prefixes):
#
#     * <base>         : for use in top_srcdir/Makefile
#     * <base>_foo     : for use in top_srcdir/foo/Makefile
#     * <base>_foo_bar : for use in top_srcdir/foo/bar/Makefile
#     * <base>_baz     : for use in top_srcdir/baz/Makefile
#
#   The <base> substitutions cover both foo/bar and baz, so none of the
#   other substitutions should be used. Indeed, no Makefiles are needed in
#   the other directories. But if sub-directory Makefiles are used, then
#   both <base>_baz and either of <base>_foo or <base>_foo_bar must be used
#   in their respective Makefiles.
#
#   Description of Makefile targets defined by AX_GNU_AUTOTEST_RULES*:
#
#     * check-autotest: The equivalent of check.
#     * installcheck-autotest: The equivalent of installcheck.
#     * clean-autotest: The equivalent of clean.
#
#   The developer can either define the above targets as dependencies of
#   their appropriate equivalent rule or of their *-local equivalent rule
#   for automake or they can define a rule with a sub-make call as they
#   wish.
#
#   All rules are dependent on like-named rules for each sub-directory and
#   for each testsuite. Only the testsuite rules actually do any work, the
#   rest are just collectors and convenience names. Assuming this example in
#   configure.ac:
#
#     AX_GNU_AUTOTEST([foo], [testsuite bar])
#     AX_GNU_AUTOTEST([baz])
#
#   Then AX_GNU_AUTOTEST_RULES defines these check rules (likewise for
#   installcheck and clean):
#
#     check-autotest: check-autotest-foo check-autotest-baz
#     check-autotest-foo: check-autotest-foo-testsuite check-autotest-foo-bar
#     check-autotest-baz: check-autotest-baz-testsuite
#     check-autotest-foo-testsuite  # Executes foo/testsuite -C foo
#     check-autotest-foo-bar        # Executes foo/bar -C foo
#     check-autotest-baz-testsuite  # Executes baz/testsuite -C baz
#
#   And AX_GNU_AUTOTEST_RULES_baz defines these check rules:
#
#     check-autotest: check-autotest-testsuite
#     check-autotest-testsuite  # Executes testsuite (which is baz/testsuite)
#
#   Note how the rule names only contain the directory and testsuite paths
#   relative to the Makefile location. Also note how each testsuite is
#   executed in its respective testdir.
#
#   In addition to the above, AX_GNU_AUTOTEST_RULES* also contains the rules
#   to keep the testsuites, "package.m4" and "atconfig" updated. The
#   matching rules to keep "atlocal" updated are generated by automake if
#   that is used or are the responsibility of the developer.
#
#   All testsuite executions (except for clean) use variables
#   AX_GNU_AUTOTEST_FLAGS, AX_GNU_AUTOTEST_CHECK_FLAGS,
#   AX_GNU_AUTOTEST_INSTALLCHECK_FLAGS and more path-and-script-specific
#   variants for additional command line options. These variables can be
#   defined by the developer to pass options to the testsuite. In the
#   example above, the rule check-autotest-foo-bar would look like this:
#
#     check-autotest-foo-bar:
#         foo/bar -C foo $(AX_GNU_AUTOTEST_FLAGS)               \
#                        $(AX_GNU_AUTOTEST_CHECK_FLAGS)         \
#                        $(AX_GNU_AUTOTEST_FLAGS_foo)           \
#                        $(AX_GNU_AUTOTEST_CHECK_FLAGS_foo)     \
#                        $(AX_GNU_AUTOTEST_FLAGS_foo_bar)       \
#                        $(AX_GNU_AUTOTEST_CHECK_FLAGS_foo_bar)
#
#   Description of Makefile file lists:
#
#   These lists are intended to be added to DISTCLEANFILES and EXTRA_DIST.
#   The *_DCLEAN list contains all "atconfig" files and the *_DIST list
#   contains all testsuites and "package.m4" files. The lists are again
#   generated per directory: so AX_GNU_AUTOTEST_DCLEAN contains all
#   "atconfig" files while e.g. AX_GNU_AUTOTEST_DIST_foo contains only files
#   below the "foo" directory. These file lists are prevented from becoming
#   Makefile variables by calling AM_SUBST_NOTMAKE(): that way, only the
#   single version used by the Makefile is substituted, not all lists for
#   all other paths as well. So use either like this:
#
#     DISTCLEANFILES = @AX_GNU_AUTOTEST_DCLEAN@
#     EXTRA_DIST = @AX_GNU_AUTOTEST_DIST_foo@
#
#   Or like this:
#
#     AX_GNU_AUTOTEST_DCLEAN_foo = @AX_GNU_AUTOTEST_DCLEAN_foo@
#     AX_GNU_AUTOTEST_DIST_foo = @AX_GNU_AUTOTEST_DIST_foo@
#     DISTCLEANFILES = ${AX_GNU_AUTOTEST_DCLEAN_foo}
#     EXTRA_DIST = ${AX_GNU_AUTOTEST_DIST_foo}
#
#   Description of shorthand default Makefile contents defined by
#   AX_GNU_AUTOTEST_DEFAULT*:
#
#   This shorthand defines the appropriate rules, adds the file lists to the
#   proper variables and makes the new targets dependencies of the standard
#   "check", "installcheck" and "clean" targets. AX_GNU_AUTOTEST_DEFAULT is
#   for example equivalent to:
#
#     @AX_GNU_AUTOTEST_RULES@
#     check: check-autotest
#     installcheck: installcheck-autotest
#     clean: clean-autotest
#     distclean: distclean-autotest
#     distclean-autotest: clean-autotest
#         -rm -f @AX_GNU_AUTOTEST_DCLEAN@
#     .PHONY: distclean-autotest
#     EXTRA_DIST += @AX_GNU_AUTOTEST_DIST@
#
#   Note that this is copied verbatim into the Makefile (after expansion of
#   the contained @...@ substitutions): it does not shadow the default
#   targets as would happen if the same lines were written in a Makefile.am
#   file. And also note the use of the += operator: this will not be
#   compatible with all versions of Make. Finally, the DISTCLEANFILES list
#   is not used because automake only uses that list if it saw the variable
#   in the Makefile.am file: in a substitution, it gets ignored unless the
#   user already used the list.
#
#   Alternative standard GNU test suites not supported here:
#
#     * Automake test suites configured by the TESTS variable [2]
#     * DejaGnu test suites [3,4]
#
#   [1]:
#   <https://www.gnu.org/software/autoconf/manual/html_node/Using-Autotest.html>
#
#   [2]: <https://www.gnu.org/software/automake/manual/html_node/Tests.html>
#
#   [3]: <https://www.gnu.org/software/dejagnu/>
#
#   [4]:
#   <https://www.gnu.org/software/automake/manual/html_node/DejaGnu-Tests.html>
#
# LICENSE
#
#   Copyright (c) 2015 Olaf Mandel <olaf@mandel.name>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.  This file is offered as-is, without any
#   warranty.

#serial 7

# _AX_GNU_AUTOTEST_remove_parent(filename)
# ----------------------------------------
# Remove all sequences of foo/.. from the filename, recursively (so e.g.
# foo/bar/../.. is empty)
# normal input, quoted output!
# replacement: 1st pattern removes in middle of string
#              2nd pattern removes at beginning: string starts with [[ !
#              3rd pattern removes at end of string: string ends in ]] !
#              4th pattern handles complete collapse: string is surrounded
#                  by [[ ... ]] !
m4_define([_AX_GNU_AUTOTEST_remove_parent],
[m4_bmatch([$1], [./\.\.\(/\|$\)],
       [_AX_GNU_AUTOTEST_remove_parent(m4_bpatsubsts([[$1]],
           [/[^/]*\([^.]\|[^.]\.\|[^/]\.\.\)/\.\./], [/],
           [^\(..\)[^/]*\([^.]\|[^.]\.\|[^/]\.\.\)/\.\./], [\1],
           [/[^/]*\([^.]\|[^.]\.\|[^/]\.\.\)/\.\.\(..\)$], [\2],
           [^\(..\)[^/]*\([^.]\|[^.]\.\|[^/]\.\.\)/\.\.\(..\)$], [\1.\3]))],
       [[$1]])dnl
])# _AX_GNU_AUTOTEST_remove_parent

# _AX_GNU_AUTOTEST_canonicalize(filename)
# ---------------------------------------
# Canonicalize filename: see below for replacements
# normal input, quoted output!
# replacement: 1st pattern removes runs of /
#              2nd pattern removes runs of ./ from middle
#              3rd pattern removes ./ from beginning: string starts with [[ !
#              4th pattern removes / and /. from end: string ends in ]] !
m4_define([_AX_GNU_AUTOTEST_canonicalize],
[_AX_GNU_AUTOTEST_remove_parent(dnl
m4_bpatsubsts([[$1]],
              [//+], [/],
              [/\(\./\)+], [/],
              [^\(..\)\./], [\1],
              [/\.?\(..\)$], [\1]))dnl
])# _AX_GNU_AUTOTEST_canonicalize

# _AX_GNU_AUTOTEST_check_filename(filename)
# -----------------------------------------
# Checks and warns if filename contains invalid characters
m4_define([_AX_GNU_AUTOTEST_check_filename],
[m4_bmatch([$1],
           [\[\|\]], [m4_fatal([Overquoted file name '$1'])],
           [,],      [m4_fatal([Comma in file name '$1'])],
           [:],      [m4_fatal([Colon in file name '$1'])],
           ['],      [m4_fatal([Quote in file name '$1'])],
           [ ],      [m4_fatal([Whitespace in file name '$1'])])dnl
])# _AX_GNU_AUTOTEST_check_filename

# _AX_GNU_AUTOTEST_suite_split(suite)
# -----------------------------------
# Convert string [foo:bar] into list [[foo],[bar]] and string [foo] to
# list [[foo],[]]
# Output is quoted
# For the m4_bpatsubst, the string starts and ends with [ ]
m4_define([_AX_GNU_AUTOTEST_suite_split],
[m4_bmatch([$1],
           [:], [m4_dquote(m4_bpatsubst([[$1]], [:.*\(.\)$], [\1]),
                           m4_bpatsubst([[$1]], [^\(.\)[^:]*:], [\1]))],
                [m4_dquote([$1], [])])dnl
])# _AX_GNU_AUTOTEST_suite_split

# _AX_GNU_AUTOTEST_check_suite1(suite, source)
# --------------------------------------------
# Checks both suite and source with _AX_GNU_AUTOTEST_check_filename
m4_define([_AX_GNU_AUTOTEST_check_suite1],
[_AX_GNU_AUTOTEST_check_filename([$1])dnl
_AX_GNU_AUTOTEST_check_filename([$2])dnl
])# _AX_GNU_AUTOTEST_suite_canon1

# _AX_GNU_AUTOTEST_suite_canon(suite, source)
# -------------------------------------------
# Returns a quoted list of canonicalized suite and source
m4_define([_AX_GNU_AUTOTEST_suite_canon],
[m4_dquote(_AX_GNU_AUTOTEST_canonicalize([$1]),
           _AX_GNU_AUTOTEST_canonicalize([$2]))dnl
])# _AX_GNU_AUTOTEST_suite_canon

# _AX_GNU_AUTOTEST_check_suite2(suite)
# ------------------------------------
# Check for and warn about the presence of a slash (path-separator)
m4_define([_AX_GNU_AUTOTEST_check_suite2],
[m4_bmatch([$1],
           [/], [m4_fatal([Path in test suite file name '$1'])])dnl
])# _AX_GNU_AUTOTEST_check_suite2

# _AX_GNU_AUTOTEST_suite_expand(suite, source)
# --------------------------------------------
# Returns a quoted list of default values: suite defaults to [testsuite] and
# source defaults to suite[.at]
m4_define([_AX_GNU_AUTOTEST_suite_expand],
[m4_dquote(m4_ifblank([$1], [[testsuite]], [[$1]]),
           m4_ifblank([$2], [m4_ifblank([$1], [[testsuite.at]], [[$1.at]])],
                            [[$2]]))dnl
])# _AX_GNU_AUTOTEST_suite_expand

# _AX_GNU_AUTOTEST_add_suite(dir, [suite, source], gen_package, force)
# --------------------------------------------------------------------
# Add dir/suite to the _AX_GNU_AUTOTEST_suites variable, warning about
# duplicates unless force is non-blank. And add the source and if
# gen_package is true also package.m4 to _AX_GNU_AUTOTEST_suite_srcs
# Global variables used:
# - _AX_GNU_AUTOTEST_suites
# - _AX_GNU_AUTOTEST_suite_srcs
m4_define([_AX_GNU_AUTOTEST_add_suite],
[m4_set_add([_AX_GNU_AUTOTEST_suites], [$1/]m4_car($2), [],
            [m4_ifblank([$4], [m4_fatal([Multiple mentions of testsuite ']dnl
[$1/]m4_car($2)['])])])dnl
m4_define([_AX_GNU_AUTOTEST_suite_srcs]m4_dquote([$1/]m4_car($2)),
          m4_dquote(m4_unquote(m4_cdr($2))dnl
m4_ifnblank($3, [,[package.m4]])))dnl
])# _AX_GNU_AUTOTEST_add_suite

# _AX_GNU_AUTOTEST_gen_package(file)
# ----------------------------------
# Generate a package.m4 file if out of date wrt/ configure.am
# TODO: what other dependencies to check?
m4_define([_AX_GNU_AUTOTEST_gen_package],
[m4_if(m4_esyscmd_s([test -f '$1' -a ! '$1' -ot ']__file__['; echo $?]), 1,
[m4_errprintn(m4_location[: generating `$1' ]dnl
m4_esyscmd_s([mkdir -p "`dirname '$1'`" && cat >'$1' <<"EOFpackage.m4"
# Signature of the current package.
m4_define([AT_PACKAGE_NAME],
  ]m4_dquote(m4_defn([AC_PACKAGE_NAME]))[)
m4_define([AT_PACKAGE_TARNAME],
  ]m4_dquote(m4_defn([AC_PACKAGE_TARNAME]))[)
m4_define([AT_PACKAGE_VERSION],
  ]m4_dquote(m4_defn([AC_PACKAGE_VERSION]))[)
m4_define([AT_PACKAGE_STRING],
  ]m4_dquote(m4_defn([AC_PACKAGE_STRING]))[)
m4_define([AT_PACKAGE_BUGREPORT],
  ]m4_dquote(m4_defn([AC_PACKAGE_BUGREPORT]))[)
m4_define([AT_PACKAGE_URL],
  ]m4_dquote(m4_defn([AC_PACKAGE_URL]))[)
EOFpackage.m4]))])dnl
])# _AX_GNU_AUTOTEST_gen_package

# _AX_GNU_AUTOTEST_gen_suite(dir, [suite, source], package)
# ---------------------------------------------------------
# Generate a testscript file dir/suite if out of date wrt/ dir/source (or
# dir/package.m4 if package-var is non-zero)
m4_define([_AX_GNU_AUTOTEST_gen_suite],
[m4_pushdef([suite], [$1/]m4_car($2))dnl
m4_pushdef([source], [$1/]m4_argn(2, $2))dnl
m4_if(m4_esyscmd_s([test -f ']m4_defn([suite])[' -a ! ']dnl
m4_defn([suite])[' -ot ']m4_defn([source])[']m4_ifnblank($3,
m4_dquote([ -a ! ']m4_defn([suite])[' -ot '$1/package.m4']))dnl
[; echo $?]), 1,
[m4_errprintn(m4_location[: generating `]m4_defn([suite])[' ]dnl
m4_esyscmd_s([mkdir -p '$1' && autom4te --language=autotest -I '$1' -o ']dnl
m4_defn([suite])[' ']m4_defn([source])[']))])dnl
m4_popdef([suite], [source])dnl
])# _AX_GNU_AUTOTEST_gen_suite

# _AX_GNU_AUTOTEST_dist_suite(dir, [suite, source])
# -------------------------------------------------
# Add dir/suite and dir/source to _AX_GNU_AUTOTEST_dist
# Global variables used:
# - _AX_GNU_AUTOTEST_dist
m4_define([_AX_GNU_AUTOTEST_dist_suite],
[m4_set_add([_AX_GNU_AUTOTEST_dist], [$1/]m4_car($2))dnl
m4_set_add([_AX_GNU_AUTOTEST_dist], [$1/]m4_argn(2, $2))dnl
])# _AX_GNU_AUTOTEST_dist_suite

# _AX_GNU_AUTOTEST_acsubst_shared()
# ---------------------------------
# Generates rules and Makefile snippets shared between all directories
# shared1 is used for silencing builds and included in all RULES,
# shared2 is used in the DEFAULT substitution
m4_define([_AX_GNU_AUTOTEST_acsubst_shared],
[AC_SUBST([_AX_GNU_AUTOTEST_shared1],
[['AX_GNU_AUTOTEST_V_RUN = $(_AX_GNU_AUTOTEST_v_RUN_$(V))
_AX_GNU_AUTOTEST_v_RUN_ = $(_AX_GNU_AUTOTEST_v_RUN_$(AM_DEFAULT_VERBOSITY))
_AX_GNU_AUTOTEST_v_RUN_0 = @echo "  RUN   " $<;
AX_GNU_AUTOTEST_V_quiet_flag = $(_AX_GNU_AUTOTEST_v_quiet_flag_$(V))
_AX_GNU_AUTOTEST_v_quiet_flag_ = ]dnl
[$(_AX_GNU_AUTOTEST_v_quiet_flag_$(AM_DEFAULT_VERBOSITY))
_AX_GNU_AUTOTEST_v_quiet_flag_0 = -q # <- Note: trailing whitespace']])dnl
AM_SUBST_NOTMAKE([_AX_GNU_AUTOTEST_shared1])dnl
AC_SUBST([_AX_GNU_AUTOTEST_shared2],
[['check: check-autotest
installcheck: installcheck-autotest
clean: clean-autotest
distclean: distclean-autotest
.PHONY: distclean-autotest
distclean-autotest: clean-autotest']])dnl
AM_SUBST_NOTMAKE([_AX_GNU_AUTOTEST_shared2])dnl
])# _AX_GNU_AUTOTEST_acsubst_shared

# _AX_GNU_AUTOTEST_cleanname(char, name)
# --------------------------------------
# Output name with all non-alphanumeric characters replaced with char
# Output is quoted
# For the m4_bpatsubsts the string starts and ends in [[ ]] and
#   the repetition catches all possible sequences of non-alnum chars
m4_define([_AX_GNU_AUTOTEST_cleanname],
[m4_bpatsubsts([[$2]], [\(..\)[^0-9A-Za-z$1]\(..\)], [\1$1\2],
                       [\(..\)[^0-9A-Za-z$1]\(..\)], [\1$1\2],
                       [\(..\)[^0-9A-Za-z$1]\(..\)], [\1$1\2],
                       [\(..\)[^0-9A-Za-z$1]\(..\)], [\1$1\2],
                       [\(..\)[^0-9A-Za-z$1]\(..\)], [\1$1\2])dnl
])# _AX_GNU_AUTOTEST_cleanname

# _AX_GNU_AUTOTEST_substr(str, start, [len])
# ------------------------------------------
# Like m4_substr(), but with quoted output
# For m4_bpatsubst the string starts and ends in [ ]
# TODO: RE repetition does not work, so need to use loops... Why?
# TODO: What about out-of-range start or len
m4_define([_AX_GNU_AUTOTEST_substr],
[m4_pushdef([i])dnl
m4_if($#, 2, [m4_bpatsubst([[$1]], [^\(.\)]m4_if(m4_eval([($2) > 0]), [0],,
                                   [m4_for([i], 0, m4_eval([($2)-1]),,
                                   [[.]])])[\(.*\)], [\1\2])],
             [m4_bpatsubst([[$1]], [^\(.\)]m4_if(m4_eval([($2) > 0]), [0],,
                                   [m4_for([i], 0, m4_eval([($2)-1]),,
                                   [[.]])])[\(]m4_if(m4_eval([($3) > 0]), [0],,
                                   [m4_for([i], 0, m4_eval([($3)-1]),,
                                   [[.]])])[\).*\(.\)], [\1\2\3])])dnl
m4_popdef([i])dnl
])# _AX_GNU_AUTOTEST_substr

# _AX_GNU_AUTOTEST_list_prependc(prefix, arg1, ...)
# -------------------------------------------------
# Returns unquoted list of ,prefixarg1,prefixarg2, ...
# Output starts with a comma if at least one arg
m4_define([_AX_GNU_AUTOTEST_list_prependc],
[m4_pushdef([i])dnl
m4_foreach([i], m4_cdr($@),
           [,[$1]m4_defn([i])])dnl
m4_popdef([i])dnl
])# _AX_GNU_AUTOTEST_list_prependc

# _AX_GNU_AUTOTEST_list_appendc(postfix, arg1, ...)
# -------------------------------------------------
# Returns unquoted list of ,arg1postfix,arg2postfix, ...
# Output starts with a comma if at least one arg
m4_define([_AX_GNU_AUTOTEST_list_appendc],
[m4_pushdef([i])dnl
m4_foreach([i], m4_cdr($@),
           [,m4_defn([i])[$1]])dnl
m4_popdef([i])dnl
])# _AX_GNU_AUTOTEST_list_appendc

# _AX_GNU_AUTOTEST_filter_listc(prefix, arg1, ...)
# ------------------------------------------------
# Returns unquoted list of all args starting with prefix, but the prefix is
# stripped in the output. Output starts with a comma if at least one arg
m4_define([_AX_GNU_AUTOTEST_filter_listc],
[m4_pushdef([n], m4_len([$1]))dnl
m4_pushdef([i])dnl
m4_foreach([i], m4_cdr($@),
           [m4_if([$1], _AX_GNU_AUTOTEST_substr(m4_defn([i]), 0, n),
                  [,_AX_GNU_AUTOTEST_substr(m4_defn([i]), n)])])dnl
m4_popdef([n], [i])dnl
])# _AX_GNU_AUTOTEST_filter_listc

# _AX_GNU_AUTOTEST_filterout_listc(rx, arg1, ...)
# -----------------------------------------------
# Returns unquoted list of all args not matching regular expression rx
# The arguments are not modified (unlike filter_listc). Output starts with
# a comma if at least one arg
m4_define([_AX_GNU_AUTOTEST_filterout_listc],
[m4_pushdef([i])dnl
m4_foreach([i], m4_cdr($@),
           [m4_bmatch(m4_defn([i]), [$1], [], [,m4_defn([i])])])dnl
m4_popdef([i])dnl
])# _AX_GNU_AUTOTEST_filterout_listc

# _AX_GNU_AUTOTEST_filterpref_listc(prefix, arg1, ...)
# ----------------------------------------------------
# Returns unquoted list of all args that do not start with any of the other
# args followed by the prefix. Output starts with a comma if at least one
# arg. Example:
# filterpref([/], [foo], [foo/bar], [baz/bar])
# => ,[foo],[baz/baz]
# Uses a set variable _AX_GNU_AUTOTEST_filterpref_var, which interferes with
# other variables of the same name elsewhere (pushdef / popdef do not work
# fully for sets).
m4_define([_AX_GNU_AUTOTEST_filterpref_listc],
[m4_pushdef([_AX_GNU_AUTOTEST_filterpref_var])dnl
m4_set_delete([_AX_GNU_AUTOTEST_filterpref_var])dnl
m4_if([$#], [1], [], [m4_set_add_all([_AX_GNU_AUTOTEST_filterpref_var],
                                     m4_unquote(m4_cdr($@)))])dnl
m4_pushdef([i])dnl
m4_pushdef([j])dnl
m4_pushdef([m], m4_len([$1]))dnl
m4_pushdef([n])dnl
m4_foreach([i], m4_cdr($@),
           [m4_define([n], m4_eval(m4_defn([m]) + m4_len(m4_defn([i]))))dnl
m4_set_foreach([_AX_GNU_AUTOTEST_filterpref_var], [j],
               [m4_if(m4_defn([i])[$1],
                      _AX_GNU_AUTOTEST_substr(m4_defn([j]), 0, n),
                      [m4_set_remove([_AX_GNU_AUTOTEST_filterpref_var],
                                     m4_defn([j]))])])])dnl
m4_set_listc([_AX_GNU_AUTOTEST_filterpref_var])dnl
m4_popdef([_AX_GNU_AUTOTEST_filterpref_var], [i], [j], [m], [n])dnl
])# _AX_GNU_AUTOTEST_filterpref_listc

# _AX_GNU_AUTOTEST_rules_flags(dirsuite, target)
# ----------------------------------------------
# Outputs the list of flag variables for the Makefile rules, specific to
# the suite / dir and target
# Recurses into all parent-directories (parent-first)
# For m4_bpatsubsts the string starts and ends in [[ ]]
m4_define([_AX_GNU_AUTOTEST_rules_flags],
[m4_pushdef([name], _AX_GNU_AUTOTEST_cleanname([_], [$1]))dnl
m4_bmatch([$1], [^\.?$],
                 [[ $(AX_GNU_AUTOTEST_FLAGS) $(AX_GNU_AUTOTEST_$2_FLAGS)]],
                 [_AX_GNU_AUTOTEST_rules_flags(m4_bpatsubsts([[$1]],
                                               [^\(..\)[^/]*\(..\)$], [\1\2],
                                               [/[^/]*\(..\)$], [\1]),
                                               [$2])dnl
[ $(AX_GNU_AUTOTEST_FLAGS_]m4_defn([name])[) $(AX_GNU_AUTOTEST_$2_FLAGS_]dnl
m4_defn([name])[)]])dnl
m4_popdef([name])dnl
])# _AX_GNU_AUTOTEST_rules_flags

# _AX_GNU_AUTOTEST_acsubst_rules_suite_deps(suite, dep1, ...)
# -----------------------------------------------------------
# Generates the three rules for one suite, output as string
# The suite and dependencies are already relative to the source-dir.
# For the m4_bpatsubst, the string starts and ends with [ ]
# The two quotes ('') in AM_V_GEN obviate the need for m4_pattern_allow
m4_define([_AX_GNU_AUTOTEST_acsubst_rules_suite_deps],
[m4_pushdef([name], _AX_GNU_AUTOTEST_cleanname([-], [$1]))dnl
m4_pushdef([atconf], [atconfig])dnl
m4_bmatch([$1],
          [/], [m4_define([atconf], m4_bpatsubst([[$1]], [/[^/]*\(.\)$],
                                                         [/atconfig\1]))])dnl
m4_pushdef([cdir])dnl
m4_bmatch([$1],
          [/], [m4_define([cdir], [ -C ]m4_bpatsubst([[$1]], [/[^/]*\(.\)$],
                                                             [\1]))])dnl
m4_dquote([$(srcdir)/$1: $(top_srcdir)/]__file__[]dnl
m4_mapall([[ $(srcdir)/]m4_quote], m4_cdr($@))[
	$(A''M_V_GEN)cd $(top_srcdir) && $(AUTOCONF) -f
]m4_defn([atconf])[:
	$(A''M_V_GEN)cd $(top_builddir) && $(SHELL) ./config.status ]dnl
$(AX_GNU_AUTOTEST_V_quiet_flag)[$][@
check-autotest-]m4_defn([name])[: $(srcdir)/$1 ]m4_defn([atconf])[
	$(AX_GNU_AUTOTEST_V_RUN)$(SHELL) $(srcdir)/$1]m4_defn([cdir])dnl
_AX_GNU_AUTOTEST_rules_flags([$1], [CHECK])[
installcheck-autotest-]m4_defn([name])[: $(srcdir)/$1 ]m4_defn([atconf])[
	$(AX_GNU_AUTOTEST_V_RUN)$(SHELL) $(srcdir)/$1]m4_defn([cdir])dnl
[ AUTOTEST_PATH=$(DESTDIR)$(bindir)]dnl
_AX_GNU_AUTOTEST_rules_flags([$1], [INSTALLCHECK])[
clean-autotest-]m4_defn([name])[: $(srcdir)/$1
	-$(SHELL) $(srcdir)/$1]m4_defn([cdir])[ -c
])dnl
m4_popdef([name], [atconf], [cdir])dnl
])# _AX_GNU_AUTOTEST_acsubst_rules_suite_deps

# _AX_GNU_AUTOTEST_acsubst_rules_suite(dir, suite)
# ------------------------------------------------
# Generates the three rules for one suite, output as string
# The suite is supplied still containing the dir prefix.
# Global variables used:
# - _AX_GNU_AUTOTEST_suite_srcs
m4_define([_AX_GNU_AUTOTEST_acsubst_rules_suite],
[m4_pushdef([suite], [$2])dnl
m4_ifnblank([$1], [m4_define([suite], m4_unquote(m4_cdr(dnl
_AX_GNU_AUTOTEST_filter_listc([$1/], [$2]))))])dnl
m4_pushdef([sdir])dnl
m4_bmatch([$2], [/], [m4_define([sdir],
                      m4_bpatsubst([[$2]], [/[^/]*\(.\)$], [\1]))])dnl
m4_pushdef([deps], m4_dquote(,
                   m4_unquote(m4_defn([_AX_GNU_AUTOTEST_suite_srcs[$2]]))))dnl
m4_ifnblank(m4_defn([sdir]), [m4_define([deps], m4_dquote(dnl
_AX_GNU_AUTOTEST_list_prependc(m4_defn([sdir])[/]deps)))])dnl
m4_ifnblank([$1], [m4_define([deps],
                   m4_dquote(_AX_GNU_AUTOTEST_filter_listc([$1/]deps)))])dnl
_AX_GNU_AUTOTEST_acsubst_rules_suite_deps(m4_defn([suite])deps)dnl
m4_popdef([deps], [sdir], [suite])dnl
])# _AX_GNU_AUTOTEST_acsubst_rules_suite

# _AX_GNU_AUTOTEST_acsubst_rules_dir(base, dir)
# ---------------------------------------------
# Generates the dependency rules for one directory relative to base, output
# as string
# For the m4_bpatsubst, the string starts and ends with [ ]
# Global variables used:
# - _AX_GNU_AUTOTEST_suites
# - _AX_GNU_AUTOTEST_testdirs
m4_define([_AX_GNU_AUTOTEST_acsubst_rules_dir],
[m4_pushdef([name])dnl
m4_ifnblank([$2],
            [m4_define([name], [-]_AX_GNU_AUTOTEST_cleanname([-], [$2]))])dnl
m4_pushdef([suites], m4_dquote(m4_set_listc([_AX_GNU_AUTOTEST_suites])))dnl
m4_ifnblank([$1], [m4_define([suites], m4_dquote(dnl
_AX_GNU_AUTOTEST_filter_listc([$1/]suites)))])dnl
m4_ifblank([$2], [m4_define([suites], m4_dquote(dnl
_AX_GNU_AUTOTEST_filterout_listc([/]suites)))],
                 [m4_define([suites], m4_dquote(dnl
_AX_GNU_AUTOTEST_list_prependc([$2/]dnl
_AX_GNU_AUTOTEST_filterout_listc([/]dnl
_AX_GNU_AUTOTEST_filter_listc([$2/]suites)))))])dnl
m4_pushdef([testdirs], m4_dquote(m4_set_listc(dnl
[_AX_GNU_AUTOTEST_testdirs])))dnl
m4_ifnblank([$1], [m4_define([testdirs], m4_dquote(dnl
_AX_GNU_AUTOTEST_filter_listc([$1/]testdirs)))])dnl
m4_ifblank([$2], [m4_define([testdirs], m4_dquote(dnl
_AX_GNU_AUTOTEST_filterpref_listc([/]testdirs)))],
                 [m4_define([testdirs], m4_dquote(dnl
_AX_GNU_AUTOTEST_list_prependc([$2/]dnl
_AX_GNU_AUTOTEST_filterpref_listc([/]dnl
_AX_GNU_AUTOTEST_filter_listc([$2/]testdirs)))))])dnl
m4_pushdef([deps], m4_quote(m4_cdr(testdirs[]suites)))dnl
m4_define([deps], m4_dquote(m4_mapall_sep(dnl
[[-autotest-]m4_curry([_AX_GNU_AUTOTEST_cleanname], [-])], [,],
m4_defn([deps]))))dnl
m4_dquote([check-autotest]m4_defn([name])[:]m4_mapall([[ check]m4_quote],
                                                      m4_defn([deps]))[
installcheck-autotest]m4_defn([name])[:]m4_mapall([[ installcheck]m4_quote],
                                                  m4_defn([deps]))[
clean-autotest]m4_defn([name])[:]m4_mapall([[ clean]m4_quote],
                                           m4_defn([deps]))[
])dnl
m4_popdef([deps], [name], [suites], [testdirs])dnl
])# _AX_GNU_AUTOTEST_acsubst_rules_dir

# _AX_GNU_AUTOTEST_acsubst_rules_phony(element)
# ---------------------------------------------
# Generates the names of the three targets for one element as string
m4_define([_AX_GNU_AUTOTEST_acsubst_rules_phony],
[m4_pushdef([name])dnl
m4_ifnblank([$1], [m4_define([name],
                             _AX_GNU_AUTOTEST_cleanname([-], [-$1]))])dnl
m4_dquote([ check-autotest]m4_defn([name])dnl
[ installcheck-autotest]m4_defn([name])dnl
[ clean-autotest]m4_defn([name]))dnl
m4_popdef([name])dnl
])# _AX_GNU_AUTOTEST_acsubst_rules_phony

# _AX_GNU_AUTOTEST_acsubst_rules_one(name, dir)
# ---------------------------------------------
# AC_SUBSTs one variable called "name" to contain the rules for all suites
# in or below dir
# Global variables used:
# - _AX_GNU_AUTOTEST_suites
# - _AX_GNU_AUTOTEST_testdirs
m4_define([_AX_GNU_AUTOTEST_acsubst_rules_one],
[m4_pushdef([suites])dnl
m4_ifblank([$2], [m4_define([suites], m4_dquote(dnl
m4_set_listc([_AX_GNU_AUTOTEST_suites])))],
                 [m4_define([suites], m4_dquote(dnl
_AX_GNU_AUTOTEST_list_prependc([$2/]dnl
_AX_GNU_AUTOTEST_filter_listc([$2/]dnl
m4_set_listc([_AX_GNU_AUTOTEST_suites])))))])dnl
m4_pushdef([testdirs])dnl
m4_ifblank([$2], [m4_define([testdirs], m4_dquote([],
                            m4_set_listc([_AX_GNU_AUTOTEST_testdirs])))],
                 [m4_define([testdirs], m4_dquote([],
                            _AX_GNU_AUTOTEST_filter_listc([$2/]dnl
m4_set_listc([_AX_GNU_AUTOTEST_testdirs]))))])dnl
AC_SUBST([$1],
[["${_AX_GNU_AUTOTEST_shared1}"'
]]m4_map_args([m4_curry([_AX_GNU_AUTOTEST_acsubst_rules_suite],
                       [$2])]suites)dnl
m4_map_args([m4_curry([_AX_GNU_AUTOTEST_acsubst_rules_dir], [$2])]testdirs)dnl
[[.PHONY:]]m4_map_args_sep([_AX_GNU_AUTOTEST_acsubst_rules_phony(], [)], [\
]testdirs[]suites)[[']])dnl
AM_SUBST_NOTMAKE([$1])dnl
m4_popdef([suites], [testdirs])dnl
])# _AX_GNU_AUTOTEST_acsubst_rules_one

# _AX_GNU_AUTOTEST_acsubst_rules(dir)
# -----------------------------------
# AC_SUBSTs the rules variable for dir and recurses for all parent-dirs
# Recurses into all parent-directories
# For m4_bpatsubsts the string starts and ends in [[ ]]
m4_define([_AX_GNU_AUTOTEST_acsubst_rules],
[m4_bmatch([$1], [^\.?$],
                 [_AX_GNU_AUTOTEST_acsubst_rules_one([AX_GNU_AUTOTEST_RULES],
                                                     [])],
                 [_AX_GNU_AUTOTEST_acsubst_rules_one(dnl
[AX_GNU_AUTOTEST_RULES_]_AX_GNU_AUTOTEST_cleanname([_], [$1]), [$1])dnl
_AX_GNU_AUTOTEST_acsubst_rules(m4_bpatsubsts([[$1]], [^\(..\)[^/]*\(..\)$],
                               [\1\2], [/[^/]*\(..\)$], [\1]))])dnl
])# _AX_GNU_AUTOTEST_acsubst_rules

# _AX_GNU_AUTOTEST_acsubst_list(name, arg1, ...)
# ----------------------------------------------
# AC_SUBSTs the name variable to contain all arg1, ...
m4_define([_AX_GNU_AUTOTEST_acsubst_list],
[AC_SUBST([$1], [[']]m4_dquote(m4_mapall_sep([m4_quote], [[\
 ]], m4_dquote(m4_dquote_elt(m4_unquote(m4_cdr($@))))))[[']])dnl
AM_SUBST_NOTMAKE([$1])dnl
])# _AX_GNU_AUTOTEST_acsubst_list

# _AX_GNU_AUTOTEST_acsubst_dclean(dir)
# ------------------------------------
# AC_SUBSTs the dclean variable for dir and recurses for all parent-dirs
# Global variables used:
# - _AX_GNU_AUTOTEST_testdirs
# Recurses into all parent-directories
# For m4_bpatsubsts the string starts and ends in [[ ]]
m4_define([_AX_GNU_AUTOTEST_acsubst_dclean],
[m4_bmatch([$1], [^\.?$],
                 [_AX_GNU_AUTOTEST_acsubst_list([AX_GNU_AUTOTEST_DCLEAN]dnl
_AX_GNU_AUTOTEST_list_appendc([/atconfig]dnl
m4_set_listc([_AX_GNU_AUTOTEST_testdirs])))],
                 [_AX_GNU_AUTOTEST_acsubst_list(dnl
[AX_GNU_AUTOTEST_DCLEAN_]_AX_GNU_AUTOTEST_cleanname([_], [$1])dnl
_AX_GNU_AUTOTEST_filter_listc([$1/]dnl
_AX_GNU_AUTOTEST_list_appendc([/atconfig]dnl
m4_set_listc([_AX_GNU_AUTOTEST_testdirs]))))dnl
_AX_GNU_AUTOTEST_acsubst_dclean(m4_bpatsubsts([[$1]], [^\(..\)[^/]*\(..\)$],
                                [\1\2], [/[^/]*\(..\)$], [\1]))])dnl
])# _AX_GNU_AUTOTEST_acsubst_dclean

# _AX_GNU_AUTOTEST_acsubst_dist(dir)
# ----------------------------------
# AC_SUBSTs the dist variable for dir and recurses for all parent-dirs
# Global variables used:
# - _AX_GNU_AUTOTEST_dist
# Recurses into all parent-directories
# For m4_bpatsubsts the string starts and ends in [[ ]]
m4_define([_AX_GNU_AUTOTEST_acsubst_dist],
[m4_bmatch([$1], [^\.?$],
                 [_AX_GNU_AUTOTEST_acsubst_list([AX_GNU_AUTOTEST_DIST]dnl
m4_set_listc([_AX_GNU_AUTOTEST_dist]))],
                 [_AX_GNU_AUTOTEST_acsubst_list(dnl
[AX_GNU_AUTOTEST_DIST_]_AX_GNU_AUTOTEST_cleanname([_], [$1])dnl
_AX_GNU_AUTOTEST_filter_listc([$1/]m4_set_listc([_AX_GNU_AUTOTEST_dist])))dnl
_AX_GNU_AUTOTEST_acsubst_dist(m4_bpatsubsts([[$1]], [^\(..\)[^/]*\(..\)$],
                              [\1\2], [/[^/]*\(..\)$], [\1]))])dnl
])# _AX_GNU_AUTOTEST_acsubst_dist

# _AX_GNU_AUTOTEST_acsubst_default(dir)
# -------------------------------------
# AC_SUBSTs the rules of the "default" use-case  for dir and recurses for
# all parent-dirs
# Recurses into all parent-directories
# For m4_bpatsubsts the string starts and ends in [[ ]]
m4_define([_AX_GNU_AUTOTEST_acsubst_default],
[m4_pushdef([name])dnl
m4_bmatch([$1], [^\.?$], [],
          [m4_define([name], [_]_AX_GNU_AUTOTEST_cleanname([_], [$1]))])dnl
AC_SUBST([AX_GNU_AUTOTEST_DEFAULT]m4_defn([name]),
[["${AX_GNU_AUTOTEST_RULES]m4_defn([name])[}
${_AX_GNU_AUTOTEST_shared2}"'
	-rm -f '"${AX_GNU_AUTOTEST_DCLEAN]m4_defn([name])[}"'
EXTRA_DIST += '"${AX_GNU_AUTOTEST_DIST]m4_defn([name])[}"]])dnl
AM_SUBST_NOTMAKE([AX_GNU_AUTOTEST_DEFAULT]m4_defn([name]))dnl
m4_popdef([name])dnl
m4_bmatch([$1], [^\.?$], [],
          [_AX_GNU_AUTOTEST_acsubst_default(m4_bpatsubsts([[$1]],
               [^\(..\)[^/]*\(..\)$], [\1\2], [/[^/]*\(..\)$], [\1]))])dnl
])# _AX_GNU_AUTOTEST_acsubst_default

# AX_GNU_AUTOTEST([testdir = `tests'], [testsuites = `testsuite'],
#                 [atlocal-sources = `'], [gen-package = `yes'],
#                 [force = `no'])
# ----------------------------------------------------------------
# Global variables used:
# - _AX_GNU_AUTOTEST_testdirs
# - _AX_GNU_AUTOTEST_suites
# - _AX_GNU_AUTOTEST_suite_srcs
# - _AX_GNU_AUTOTEST_dist
AC_DEFUN([AX_GNU_AUTOTEST],
[dnl Check command arguments
dnl Check $5: force
m4_pushdef([force], m4_normalize([$5]))dnl
m4_bmatch(m4_defn([force]), [^\([Nn][Oo]?\|0\)$], [m4_define([force], [])])dnl
m4_ifnblank(m4_defn([force]), [m4_define([force], 1)])dnl
dnl
dnl Check $1: testdir
m4_pushdef([testdir], m4_normalize([$1]))dnl
m4_ifblank(m4_defn([testdir]), [m4_define([testdir], [tests])])dnl
m4_ifblank(force, [_AX_GNU_AUTOTEST_check_filename(m4_defn([testdir]))])dnl
m4_define([testdir], _AX_GNU_AUTOTEST_canonicalize(m4_defn([testdir])))dnl
m4_set_add([_AX_GNU_AUTOTEST_testdirs], m4_defn([testdir]), [],
           [m4_ifblank(force,
    [m4_fatal([Already configured directory ']m4_defn([testdir])['])])])dnl
dnl
dnl Check $4: gen-package
m4_pushdef([gen_package], m4_normalize([$4]))dnl
m4_ifblank(m4_defn([gen_package]), [m4_define([gen_package], [yes])])dnl
m4_bmatch(m4_defn([gen_package]),
          [^\([Nn][Oo]?\|0\)$], [m4_define([gen_package], [])])dnl
m4_ifnblank(m4_defn([gen_package]), [m4_define([gen_package], 1)])dnl
dnl
dnl Check $2: testsuites
m4_pushdef([testsuites], m4_normalize([$2]))dnl
m4_ifblank(m4_defn([testsuites]), [m4_define([testsuites], [testsuite])])dnl
m4_define([testsuites], m4_dquote(m4_map_args_w(m4_defn([testsuites]),
                        [_AX_GNU_AUTOTEST_suite_split(], [)], [,])))dnl
m4_ifblank(force, [m4_map([_AX_GNU_AUTOTEST_check_suite1],
                          m4_defn([testsuites]))])dnl
m4_define([testsuites], m4_dquote(m4_map_sep([_AX_GNU_AUTOTEST_suite_canon],
                                             [,], m4_defn([testsuites]))))dnl
m4_ifblank(force, [m4_map([_AX_GNU_AUTOTEST_check_suite2],
                          m4_defn([testsuites]))])dnl
m4_define([testsuites], m4_dquote(m4_map_sep([_AX_GNU_AUTOTEST_suite_expand],
                                             [,], m4_defn([testsuites]))))dnl
m4_map_args_sep([_AX_GNU_AUTOTEST_add_suite(]m4_quote(m4_defn([testdir]))[,],
                [,]m4_defn([gen_package])[,]m4_defn([force])[)], [],
                testsuites)dnl
dnl
dnl Check $3: atlocal-sources
m4_pushdef([atlocal_sources], m4_normalize(m4_bpatsubst([[$3]], [:], [ ])))dnl
m4_define([atlocal_sources], m4_split(m4_defn([atlocal_sources]), [ ]))dnl
m4_ifnblank(m4_defn([atlocal_sources]), [m4_define([atlocal_sources],
    m4_dquote(m4_map_args_sep([_AX_GNU_AUTOTEST_canonicalize(],
                              [)], [,], atlocal_sources)))])dnl
m4_ifblank(force, [m4_map_args([_AX_GNU_AUTOTEST_check_filename],
                               atlocal_sources)])dnl
dnl
dnl Phase 1
m4_ifnblank(gen_package,
[_AX_GNU_AUTOTEST_gen_package(m4_defn([testdir])[/package.m4])dnl
m4_set_add([_AX_GNU_AUTOTEST_dist], m4_defn([testdir])[/package.m4])dnl
])dnl
m4_map_args_sep([_AX_GNU_AUTOTEST_gen_suite([]m4_defn([testdir])[],],
                [,]gen_package[)], [], testsuites)dnl
m4_map_args_sep([_AX_GNU_AUTOTEST_dist_suite([]m4_defn([testdir])[],],
                [)], [], testsuites)dnl
dnl
dnl Phase 2
dnl The following commands fail for quotes in testdir
AC_CONFIG_TESTDIR(m4_dquote(m4_defn([testdir])))dnl
m4_ifnblank(m4_defn([atlocal_sources]), [AC_CONFIG_FILES(dnl
m4_defn([testdir])[/atlocal:]m4_map_args_sep(m4_defn([testdir])[/],
                                             [], [:], atlocal_sources))])dnl
dnl
dnl Phase 3
_AX_GNU_AUTOTEST_acsubst_shared()dnl
_AX_GNU_AUTOTEST_acsubst_rules(m4_defn([testdir]))dnl
_AX_GNU_AUTOTEST_acsubst_dclean(m4_defn([testdir]))dnl
_AX_GNU_AUTOTEST_acsubst_dist(m4_defn([testdir]))dnl
_AX_GNU_AUTOTEST_acsubst_default(m4_defn([testdir]))dnl
dnl
m4_popdef([testdir], [testsuites], [atlocal_sources], [gen_package],
          [force])dnl
])# AX_GNU_AUTOTEST
