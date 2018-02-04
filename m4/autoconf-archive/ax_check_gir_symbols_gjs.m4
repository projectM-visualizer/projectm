# =============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_check_gir_symbols_gjs.html
# =============================================================================
#
# SYNOPSIS
#
#   AX_CHECK_GIR_SYMBOLS_GJS(MODULE, APIVERSION, SYMBOLS)
#
# DESCRIPTION
#
#   Check that each symbol from the whitespace-separated list of $SYMBOLS is
#   defined inside the GObject Introspection module $MODULE with API version
#   $APIVERSION, and is importable in GJS, GNOME's JavaScript engine
#   (https://wiki.gnome.org/Projects/Gjs).
#
#   GObject Introspection
#   (https://wiki.gnome.org/Projects/GObjectIntrospection) is a tool for
#   generating bindings from C libraries to higher-level languages. The
#   bindings live in a GObject Introspection repository (GIR) file, which is
#   what this macro checks.
#
#   Note that for the purposes of GObject Introspection, the API version is
#   different from the release version. For example, GTK currently has API
#   version 3.0, but that could mean any release from the 3.0, 3.2, 3.4,...
#   series.
#
#   Example:
#
#     AX_CHECK_GIR_SYMBOLS_GJS([Gtk], [3.0], [ListBox FlowBox
#                                             Widget.get_action_group])
#
#   NOTE: This macro depends on AX_PROG_GJS.
#
# LICENSE
#
#   Copyright (c) 2013, 2016 Endless Mobile, Inc.; contributed by Philip Chimento <philip@endlessm.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 2

AC_DEFUN([AX_CHECK_GIR_SYMBOLS_GJS], [
  AC_REQUIRE([AX_PROG_GJS])
  m4_foreach_w([SYMBOL], [$3], [
    AC_MSG_CHECKING([for $1.SYMBOL in $1-$2])
    _AX_GJS_IFELSE([
      imports.gi.versions@<:@\"$1\"@:>@ = \"$2\";
      const Library = imports.gi.$1;
      let symbols = \"SYMBOL\".split('.');
      function check_symbols(symbols_list) {
        printerr('gjs: checking', symbols_list.join('.'));
        try {
          symbols_list.reduce(function (prev, curr) {
            if (typeof prev@<:@curr@:>@ === 'undefined')
              throw 1;
            return prev@<:@curr@:>@;
          }, Library);
          return true;
        } catch (e) {
          if (e === 1)
            return false;
          throw e;
        }
      }
      if (!check_symbols(symbols)) {
        dnl For methods, we need to check the class's prototype
        symbols.splice(-1, 0, 'prototype');
        if (!check_symbols(symbols))
          throw 1;
      }
    ],
    [AC_MSG_RESULT([yes])],
    [
      AC_MSG_RESULT([no])
      AC_MSG_ERROR([The symbol $1.SYMBOL was not importable
in GJS, although the $1 library was present.
Perhaps you need a newer version of the library?])
    ])
  ])
])
