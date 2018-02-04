# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_prog_help2man.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PROG_HELP2MAN([program list], [man directory prefix], [binary prefix])
#
# DESCRIPTION
#
#   AX_PROG_HELP2MAN is a macro designed to facilitate usage of help2man. It
#   may take three optional arguments:
#
#   1. <program list>: comma-separated m4 list of input programs
#
#   Specifies the list of input programs which shall be used to produce man
#   pages using help2man. If no value is specified, the macro assumes that
#   the sole target will be named $(PACKAGE_NAME). Thus, the two following
#   lines are equivalent:
#
#     AX_PROG_HELP2MAN
#     AX_PROG_HELP2MAN([\$(PACKAGE_NAME)])
#
#   2. <man directory prefix>: directory prefix of the man pages directory
#
#   Sets AX_HELP2MAN_PREFIX_MANS to <man directory prefix>. Defaults to
#   'doc/man/man1' if no value is provided. Note that
#   AX_HELP2MAN_PREFIX_MANS will always be a subdirectory of the build
#   directory.
#
#   3. <binary prefix>:
#
#   Sets AX_HELP2MAN_PREFIX_BIN to <binary prefix>. Defaults to
#   '$(top_builddir)' if no value is provided. The two following lines are
#   equivalent:
#
#     AX_PROG_HELP2MAN([subdir/program1, subdir/program2])
#     AX_PROG_HELP2MAN([program1, program2], [], [\$(top_builddir)/subdir])
#
#   The macro:
#
#   - checks that help2man is available on the system.
#
#   - provides the configure option --disable-help2man.
#
#   - sets the output variable ax_help2man_MANS to the list of man pages
#   that shall be generated.
#
#   - sets the automake conditional HAVE_HELP2MAN
#
#   - sets the substitution variable AX_HELP2MAN_RULES to the required make
#   rules, targets and recipes.
#
#   Further detail on the macro can be found after the minimal working
#   example. Here is a minimal working example:
#
#     # configure.ac:
#     AX_PROG_HELP2MAN
#
#     # Makefile.am:
#     if HAVE_HELP2MAN
#     man1_MANS = $(ax_help2man_MANS)
#     @AX_HELP2MAN_RULES@
#     endif
#
#   This minimal working example shall work under the following assumptions:
#
#   1. the aforementioned binary has the same name as the project
#
#   2. the project produces a single binary at the root of the build
#   directory
#
#   3. the man page shall be generated into the doc/man/man1 directory
#
#   Note that adding ax_help2man_MANS to man1_MANS is not needed if the man
#   page generation is not mandatory.
#
#   The AX_HELP2MAN_RULES substitution variable shall contain:
#
#   - a recipe to create, if necessary, the destination directory.
#
#   - a generic rule to produce the manpages.
#
#   This rule targets the contents of the variable ax_help2man_MANS and its
#   recipe shall be akin to the following:
#
#     $(HELP2MAN) -l -N --output="$@" --name="$(shell basename $<)" "$(shell dirname $<)/$(shell basename $<)"
#
#   Errors from this rule are ignored.
#
#   - rules for each input program and target man page.
#
#   For each input program, a rule akin to the following shall be available:
#
#     $(AX_HELP2MAN_PREFIX_MANS)/program.1: $(AX_HELP2MAN_PREFIX_BIN)/path/to/program
#
#   The macro assumes that all man pages shall be generated into the same
#   directory (AX_HELP2MAN_PREFIX_MANS) and that all input programs can be
#   found in the same directory (AX_HELP2MAN_PREFIX_BIN). If a subset of the
#   inputs or outputs have different paths (for instance one of the inputs
#   is a script available in $(top_srcdir) whereas the other inputs are
#   programs compiled at build time), it can be customized in the
#   Makefile.am: the target man page must be added to ax_help2man_MANS, so
#   that it becomes a target of the generic rule for man pages and a rule
#   must be added to specify its prerequisite:
#
#     ax_help2man_MANS += path/to/output/man/page.1
#     path/to/output/man/page.1: path/to/input/program.1
#
#   Here is a full example for a project where binaries 'program1' and
#   'program2' are compiled at build time whereas 'script' is available in
#   the source tree:
#
#     # configure.ac:
#       AX_PROG_HELP2MAN([program1, program2])
#
#     # Makefile.am:
#       if HAVE_HELP2MAN
#       man1_MANS = $(ax_help2man_MANS)
#       @AX_HELP2MAN_RULES@
#       ax_help2man_MANS += $(AX_HELP2MAN_PREFIX_MANS)/script.1
#       $(AX_HELP2MAN_PREFIX_MANS)/script.1: $(top_srcdir)/script
#       endif
#
#   Note that this macro should not be called more than once.
#
# LICENSE
#
#   Copyright (c) 2017 Harenome Ranaivoarivony-Razanajato <ranaivoarivony-razanajato@hareno.me>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 3 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   Under Section 7 of GPL version 3, you are granted additional permissions
#   described in the Autoconf Configure Script Exception, version 3.0, as
#   published by the Free Software Foundation.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <https://www.gnu.org/licenses/>.

#serial 2

AC_DEFUN_ONCE([AX_PROG_HELP2MAN],
[
  dnl The make rules use 'AM_V_P' and 'AM_V_at' which may not be known.
  m4_pattern_allow(AM_V_P)
  m4_pattern_allow(AM_V_at)

  dnl Provide an option for the configure script, and check whether help2man
  dnl is available and whether the user wants to use it.
  AC_ARG_ENABLE(help2man,
      [AS_HELP_STRING([--disable-help2man], [Disable help2man man generation])])
  AC_PATH_PROG(HELP2MAN, help2man, false)
  AS_IF([test "x$enable_help2man" = "xno"], [HELP2MAN="false"])
  AM_CONDITIONAL([HAVE_HELP2MAN], [test "x$HELP2MAN" != "xfalse"])

  dnl Default to 'doc' and $(top_builddir) if no prefixes were provided
  AC_SUBST(AX_HELP2MAN_PREFIX_MANS, "\$(top_builddir)/m4_ifblank([$2], [doc/man/man1], [$2])")
  AC_SUBST(AX_HELP2MAN_PREFIX_BIN, "m4_ifblank([$3], [\$(top_builddir)], [$3])")

  ax_help2man_targets_text=""
  m4_ifblank([$1],
  dnl if no executable path was specified, default to $(PACKAGE_NAME)
  [
    ax_help2man_mans_list='$(AX_HELP2MAN_PREFIX_MANS)/$(PACKAGE_NAME).1'
    ax_help2man_targets_text='$(AX_HELP2MAN_PREFIX_MANS)/$(PACKAGE_NAME).1: $(AX_HELP2MAN_PREFIX_BIN)/$(PACKAGE_NAME)'
  ],
  dnl else, (if executable path(s) were specified), create rules for each target
  [
    m4_foreach(target, [$1],
    [
      ax_help2man_target="target"
      ax_help2man_target_name=$(basename "${ax_help2man_target}")
      ax_help2man_mans_list="${ax_help2man_mans_list} "'$(AX_HELP2MAN_PREFIX_MANS)/'"${ax_help2man_target_name}"'.1'
      ax_help2man_targets_text="${ax_help2man_targets_text}
"'$(AX_HELP2MAN_PREFIX_MANS)/'"${ax_help2man_target_name}"'.1: $(AX_HELP2MAN_PREFIX_BIN)/'"${ax_help2man_target}"
    ])
  ])

  dnl Reminder for $ax_help2man_rules_text:
  dnl   Do not insert spaces here, make rules require tabs for indentation!
  dnl
  dnl $(shell dirname $<)/$(shell basename $<) instead of merely $<:
  dnl   if the make rule prerequisite
  dnl   (i.e the input program for help2man) is at the root of the directory, the
  dnl   prerequisite text will only contain the name of the file. However,
  dnl   help2man needs the leading './'... Fortunately, dirname returns '.' if its
  dnl   input does not contain slashes!
  ax_help2man_rules_text='
# Rule to create the directory for man 1 pages
$(AX_HELP2MAN_PREFIX_MANS):
	$(AM_V_at)$(MKDIR_P) @S|@@

# Generic rule to create manpages using help2man
$(ax_help2man_MANS): | $(AX_HELP2MAN_PREFIX_MANS)
	@if ! $(AM_V_P); then printf "  %-8s %s\n" "HELP2MAN" "@S|@@"; fi
	$(AM_V_at)-$(HELP2MAN) -l -N --output="@S|@@" --name="$(shell basename $<)" "$(shell dirname $<)/$(shell basename $<)"

# Targets
'"${ax_help2man_targets_text}"

  AC_SUBST([ax_help2man_MANS], ["${ax_help2man_mans_list}"])
  AC_SUBST([AX_HELP2MAN_RULES], ["${ax_help2man_rules_text}"])
  AM_SUBST_NOTMAKE([AX_HELP2MAN_RULES])
])
