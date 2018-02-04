# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_check_girs_gjs.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_GIRS_GJS(MODULE, API_VERSION [, MODULE, API_VERSION...])
#
# DESCRIPTION
#
#   Check that the GObject Introspection module $MODULE is importable in GJS
#   (GNOME's JavaScript engine, https://wiki.gnome.org/Projects/Gjs). The
#   API version must be $API_VERSION.
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
#   series. To check for specific API that was added in a later version, use
#   AX_CHECK_GIR_SYMBOLS_GJS.
#
#   Example:
#
#     AX_CHECK_GIRS_GJS([Gdk], [3.0],
#                       [Gtk], [3.0])
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

#serial 3

AC_DEFUN([_AX_CHECK_GIR_GJS], [
  AC_MSG_CHECKING([for version $2 of $1])
  _AX_GJS_IFELSE([
      imports.gi.versions@<:@\"$1\"@:>@ = \"$2\";
      const Library = imports.gi.$1;
    ],
    [AC_MSG_RESULT([yes])],
    [
      AC_MSG_RESULT([no])
      AC_MSG_ERROR([You do not have API version $2 of the GObject
Introspection bindings for the $1 library.
Build a version from source, or find out what package to
install with one of these commands, depending on your system:
  apt-file search $1-$2.typelib
  dnf provides \*/$1-$2.typelib
])
    ]
  )
])

AC_DEFUN([AX_CHECK_GIRS_GJS], [
  AC_REQUIRE([AX_PROG_GJS])
  m4_map_args_pair([_AX_CHECK_GIR_GJS], [AC_MSG_ERROR], $@)
])
