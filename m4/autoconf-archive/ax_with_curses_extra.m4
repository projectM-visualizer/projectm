# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_with_curses_extra.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_WITH_CURSES_PANEL
#   AX_WITH_CURSES_MENU
#   AX_WITH_CURSES_FORM
#
# DESCRIPTION
#
#   These macros try to find additional libraries that often come with
#   SysV-compatible Curses.  In particular, the Panel, Menu and Form
#   libraries are searched, along with their header files.  These macros
#   depend on AX_WITH_CURSES.
#
#   The following preprocessor symbols may be defined by these macros:
#
#     By AX_WITH_CURSES_PANEL:
#
#     HAVE_PANEL              - if the Panel library is present
#     HAVE_PANEL_H            - if <panel.h> is present and should be used
#     HAVE_NCURSES_PANEL_H    - if <ncurses/panel.h> should be used
#     HAVE_NCURSESW_PANEL_H   - if <ncursesw/panel.h> should be used
#
#     By AX_WITH_CURSES_MENU:
#
#     HAVE_MENU               - if the Menu library is present
#     HAVE_MENU_H             - if <menu.h> is present and should be used
#     HAVE_NCURSES_MENU_H     - if <ncurses/menu.h> should be used
#     HAVE_NCURSESW_MENU_H    - if <ncursesw/menu.h> should be used
#
#     By AX_WITH_CURSES_FORM:
#
#     HAVE_FORM               - if the Form library is present
#     HAVE_FORM_H             - if <form.h> is present and should be used
#     HAVE_NCURSES_FORM_H     - if <ncurses/form.h> should be used
#     HAVE_NCURSESW_FORM_H    - if <ncursesw/form.h> should be used
#
#   The following output variables may be defined by these macros; these are
#   precious and may be overridden on the ./configure command line:
#
#     PANEL_LIBS   - library to add to xxx_LDADD before CURSES_LIBS
#     MENU_LIBS    - library to add to xxx_LDADD before CURSES_LIBS
#     FORM_LIBS    - library to add to xxx_LDADD before CURSES_LIBS
#
#   In previous versions of this macro, the flags PANEL_LIB, MENU_LIB and
#   FORM_LIB were defined. These have been renamed, in keeping with the
#   variable scheme of PKG_CHECK_MODULES, which should eventually supersede
#   the use of AX_WITH_CURSES and AX_WITH_CURSES_* macros. These libraries
#   are NOT added to LIBS by default.  You need to add them to the
#   appropriate xxx_LDADD line in your Makefile.am in front of the
#   equivalent CURSES_LIBS incantation.  For example:
#
#     prog_LDADD = @PANEL_LIBS@ @CURSES_LIBS@
#
#   If one of the xxx_LIBS variables is set on the configure command line
#   (such as by running "./configure PANEL_LIBS=-lmypanel"), then the header
#   file searched must NOT contain a subpath.  In this case, in other words,
#   only <panel.h> would be searched for.  The user may use the CPPFLAGS
#   precious variable to override the standard #include search path.
#
#   The following shell variables may be defined by these macros:
#
#     ax_cv_panel   - set to "yes" if Panels library is present
#     ax_cv_menu    - set to "yes" if Menu library is present
#     ax_cv_form    - set to "yes" if Form library is present
#
#   These variables can be used in your configure.ac to determine whether a
#   library you require is actually present.  For example:
#
#     AX_WITH_CURSES
#     if test "x$ax_cv_curses" != xyes; then
#         AC_MSG_ERROR([requires a SysV or X/Open-compatible Curses library])
#     fi
#     AX_WITH_CURSES_PANEL
#     if test "x$ax_cv_panel" != xyes; then
#         AC_MSG_ERROR([requires the Curses Panel library])
#     fi
#
#   To use the HAVE_xxx_H preprocessor symbols, insert the following into
#   your system.h (or equivalent) header file:
#
#     For AX_WITH_CURSES_PANEL:
#
#     #if defined HAVE_NCURSESW_PANEL_H
#     #  include <ncursesw/panel.h>
#     #elif defined HAVE_NCURSES_PANEL_H
#     #  include <ncurses/panel.h>
#     #elif defined HAVE_PANEL_H
#     #  include <panel.h>
#     #else
#     #  error "SysV-compatible Curses Panel header file required"
#     #endif
#
#     For AX_WITH_CURSES_MENU:
#
#     #if defined HAVE_NCURSESW_MENU_H
#     #  include <ncursesw/menu.h>
#     #elif defined HAVE_NCURSES_MENU_H
#     #  include <ncurses/menu.h>
#     #elif defined HAVE_MENU_H
#     #  include <menu.h>
#     #else
#     #  error "SysV-compatible Curses Menu header file required"
#     #endif
#
#     For AX_WITH_CURSES_FORM:
#
#     #if defined HAVE_NCURSESW_FORM_H
#     #  include <ncursesw/form.h>
#     #elif defined HAVE_NCURSES_FORM_H
#     #  include <ncurses/form.h>
#     #elif defined HAVE_FORM_H
#     #  include <form.h>
#     #else
#     #  error "SysV-compatible Curses Form header file required"
#     #endif
#
# LICENSE
#
#   Copyright (c) 2011 John Zaitseff <J.Zaitseff@zap.org.au>
#
#   This program is free software: you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation, either version 3 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <https://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Archive. When you make and distribute a
#   modified version of the Autoconf Macro, you may extend this special
#   exception to the GPL to apply to your modified version as well.

#serial 5

AC_DEFUN([_AX_WITH_CURSES_CHECKEXTRA], [
    dnl Parameter 1 is the variable name component, using uppercase letters only
    dnl Parameter 2 is the printable library name
    dnl Parameter 3 is the C code to try compiling and linking
    dnl Parameter 4 is the header filename
    dnl Parameter 5 is the library command line

    AS_VAR_PUSHDEF([_AX_WITH_CURSES_CHECKEXTRA_have_var],        [HAVE_$1])dnl
    AS_VAR_PUSHDEF([_AX_WITH_CURSES_CHECKEXTRA_cv_var],          [ax_cv_[]m4_tolower($1)])dnl
    AS_VAR_PUSHDEF([_AX_WITH_CURSES_CHECKEXTRA_header_var],      [ax_cv_header_$4])dnl
    AS_VAR_PUSHDEF([_AX_WITH_CURSES_CHECKEXTRA_have_header_var], [HAVE_[]m4_toupper($4)])dnl

    ax_saved_LIBS=$LIBS
    ax_saved_CPPFLAGS=$CPPFLAGS

    AC_CACHE_CHECK([for Curses $2 library with $4], [_AX_WITH_CURSES_CHECKEXTRA_header_var], [
        LIBS="$ax_saved_LIBS $5 $CURSES_LIBS"
        CPPFLAGS="$ax_saved_CPPFLAGS $CURSES_CFLAGS"
        AC_LINK_IFELSE([AC_LANG_PROGRAM([[
                @%:@include <$4>
            ]], [$3])],
            [_AX_WITH_CURSES_CHECKEXTRA_header_var=yes],
            [_AX_WITH_CURSES_CHECKEXTRA_header_var=no])
    ])
    AS_IF([test "x$[]_AX_WITH_CURSES_CHECKEXTRA_header_var" = xyes], [
        _AX_WITH_CURSES_CHECKEXTRA_cv_var=yes
        AS_LITERAL_IF([$5], [$1_LIBS="$5"])
        AC_DEFINE([_AX_WITH_CURSES_CHECKEXTRA_have_var],        [1], [Define to 1 if the Curses $2 library is present])
        AC_DEFINE([_AX_WITH_CURSES_CHECKEXTRA_have_header_var], [1], [Define to 1 if <$4> is present])
    ], [
        AS_IF([test "x$[]_AX_WITH_CURSES_CHECKEXTRA_cv_var" = xyes], [],
            [_AX_WITH_CURSES_CHECKEXTRA_cv_var=no])
    ])

    LIBS=$ax_saved_LIBS
    CPPFLAGS=$ax_saved_CPPFLAGS
    unset ax_saved_LIBS
    unset ax_saved_CPPFLAGS

    AS_VAR_POPDEF([_AX_WITH_CURSES_CHECKEXTRA_have_header_var])dnl
    AS_VAR_POPDEF([_AX_WITH_CURSES_CHECKEXTRA_header_var])dnl
    AS_VAR_POPDEF([_AX_WITH_CURSES_CHECKEXTRA_cv_var])dnl
    AS_VAR_POPDEF([_AX_WITH_CURSES_CHECKEXTRA_have_var])dnl
])dnl

AC_DEFUN([_AX_WITH_CURSES_EXTRA], [
    dnl Parameter 1 is the variable name component, using uppercase letters only
    dnl Parameter 2 is the printable library name
    dnl Parameter 3 is the C code to try compiling and linking
    dnl Parameter 4 is the header filename component
    dnl Parameter 5 is the NCursesW library command line
    dnl Parameter 6 is the NCurses library command line
    dnl Parameter 7 is the plain Curses library command line

    AC_REQUIRE([AX_WITH_CURSES])
    AC_ARG_VAR([$1_LIBS], [linker library for Curses $2, e.g. $7])

    AS_IF([test "x$[]$1_LIBS" != x], [
        _AX_WITH_CURSES_CHECKEXTRA([$1], [$2], [$3], [$4], [$[]$1_LIBS])
    ], [
        AS_IF([test "x$ax_cv_curses_which" = xncursesw], [
            _AX_WITH_CURSES_CHECKEXTRA([$1], [$2], [$3], [ncursesw/$4], [$5])
        ], [test "x$ax_cv_curses_which" = xncurses], [
            _AX_WITH_CURSES_CHECKEXTRA([$1], [$2], [$3], [$4], [$6])
            AS_IF([test x$[]ax_cv_[]m4_tolower($1) != "xyes"], [
                _AX_WITH_CURSES_CHECKEXTRA([$1], [$2], [$3], [ncurses/$4], [$6])
            ])
        ], [test "x$ax_cv_curses_which" = xplaincurses], [
            _AX_WITH_CURSES_CHECKEXTRA([$1], [$2], [$3], [$4], [$7])
        ])
    ])
])dnl

AC_DEFUN([AX_WITH_CURSES_PANEL], [
    _AX_WITH_CURSES_EXTRA([PANEL], [Panel], [[
            WINDOW *win = newwin(0, 0, 0, 0);
            PANEL *pan = new_panel(win);
        ]], [panel.h], [-lpanelw], [-lpanel], [-lpanel])
])dnl

AC_DEFUN([AX_WITH_CURSES_MENU], [
    _AX_WITH_CURSES_EXTRA([MENU], [Menu], [[
            ITEM **mi;
            MENU *m = new_menu(mi);
        ]], [menu.h], [-lmenuw], [-lmenu], [-lmenu])
])dnl

AC_DEFUN([AX_WITH_CURSES_FORM], [
    _AX_WITH_CURSES_EXTRA([FORM], [Form], [[
            FIELD **ff;
            FORM *f = new_form(ff);
        ]], [form.h], [-lformw], [-lform], [-lform])
])dnl
