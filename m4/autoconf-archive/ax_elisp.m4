# ===========================================================================
#         https://www.gnu.org/software/autoconf-archive/ax_elisp.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_ELISP_CONFIG_FILE(FILENAME)
#   AX_ELISP_CHECK(SYMBOL,DESCRIPTION,BODY,SUCCESS-EXPR)
#   AX_ELISP_CHECK_FEATURE(SYMBOL)
#   AX_ELISP_CHECK_FBOUNDP(SYMBOL,[FEATURE [...]])
#   AX_ELISP_CHECK_BOUNDP(SYMBOL,[FEATURE [...]])
#
# DESCRIPTION
#
#   This is a simple library to check the Emacs reality by way of Emacs Lisp
#   forms evaluated under $EMACS --batch -Q. This means you MUST have the
#   shell variable EMACS set to a valid Emacs executable prior to the first
#   call to any of the AX_ELISP_CHECK et al macros. Those work by saving
#   their results to the file defined by calling AX_ELISP_CONFIG_FILE so you
#   MUST call that prior, too. For example:
#
#     dnl Arrange to save config answers in $top_builddir/lisp/config.el.
#     AX_ELISP_CONFIG_FILE([lisp/config.el])
#
#     dnl Set shell variable EMACS and AC_SUBST it, too.
#     dnl (NB: This is a separate Autoconf Archive macro.)
#     AX_PROG_EMACS
#
#   In the following detailed descriptions, SYMBOL stands for an Emacs Lisp
#   symbol, which may contain hyphens, e.g., 'define-error' or 'org-src'.
#   Likewise, FEATURE is an Emacs Lisp symbol (naming a feature). BODY and
#   SUCCESS-EXPR are Emacs Lisp forms, zero or more for BODY and exactly one
#   for SUCCESS-EXPR. In these forms you must take care to avoid apostrophe
#   (U+27). Instead of 'foo, write (quote foo).
#
#   * AX_ELISP_CONFIG_FILE(FILENAME)
#
#   This arranges for future AX_ELISP_CHECK (et al) calls to save their
#   results in FILENAME. May be called multiple times. FILENAME should be
#   relative to the top build dir.
#
#   * AX_ELISP_CHECK(SYMBOL,DESCRIPTION,BODY,SUCCESS-EXPR)
#
#   This is the general macro that the other AX_ELISP_CHECK* macros use. It
#   constructs a short Emacs Lisp file comprising BODY and evaluates it via
#   $EMACS --batch -Q. The exit value of this script depends on the result
#   of evaluating SUCCESS-EXPR: non-nil is success and nil is failure. On
#   success, append SYMBOL on a line of its own to the config file. This
#   macro uses AC_CACHE_CHECK and passes DESCRIPTION to it.
#
#   * AX_ELISP_CHECK_FEATURE(FEATURE)
#
#   This checks if (require (quote FEATURE)) is successful. If so, add
#   featurep-FEATURE to the config file (NB the "featurep-" prefix).
#
#   * AX_ELISP_CHECK_FBOUNDP(SYMBOL,[FEATURE [...]])
#
#   This checks if (fboundp (quote SYMBOL)) is successful. If so, append
#   SYMBOL to the config file. Optional 2nd arg is a space-separated list of
#   features to require prior to the fboundp check.
#
#   * AX_ELISP_CHECK_BOUNDP(SYMBOL,[FEATURE [...]])
#
#   This checks if (boundp (quote SYMBOL)) is successful. If so, append
#   SYMBOL to the config file. Optional 2nd arg is a space-separated list of
#   features to require prior to the boundp check.
#
# LICENSE
#
#   Copyright (c) 2016-2017 Thien-Thi Nguyen
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

#serial 2

AC_DEFUN([AX_ELISP_CONFIG_FILE],[dnl
dnl 1 -- relative filename (from 'top_srcdir')
m4_define([AX_ELISP__CONFIG_FILENAME],[$1])dnl
])dnl


AC_DEFUN([AX_ELISP__PREP],[dnl
AS_IF([AS_VAR_TEST_SET([EMACS])],,dnl
  [AC_MSG_ERROR([No value for EMACS])])
dnl FIXME: Check at autoconf-time that AX_ELISP__CONFIG_FILENAME is set.
dnl        (If not, or if the value is unsuitable, throw fatal error.)
rm -f AX_ELISP__CONFIG_FILENAME
touch AX_ELISP__CONFIG_FILENAME
])dnl


AC_DEFUN([AX_ELISP_CHECK],[dnl
dnl 1 -- Emacs Lisp symbol to add to config file if "success"
dnl 2 -- description
dnl 3 -- Emacs Lisp body (zero or more forms)
dnl 4 -- Emacs Lisp expression for "success"
AC_REQUIRE([AX_ELISP__PREP])dnl
AS_VAR_PUSHDEF([CV],[elisp_cv_$1])dnl
AC_CACHE_CHECK([$2],[CV],[dnl
cat >conftest.el <<AX_ELISP__EOF
$3
(kill-emacs (if $4 0 1))
AX_ELISP__EOF
AS_IF([$EMACS --batch -Q -l conftest.el 1>&5 2>&5],[CV=yes],[CV=no])])
AS_IF([test yes = $[]CV],[echo "$1" >> AX_ELISP__CONFIG_FILENAME])
AS_VAR_POPDEF([CV])dnl
])dnl


AC_DEFUN([AX_ELISP_CHECK_FEATURE],[dnl
dnl 1 -- Emacs Lisp symbol (a feature name)
AC_REQUIRE([AX_ELISP__PREP])dnl
AX_ELISP_CHECK([featurep-$1],[if $EMACS supports feature '$1'],[dnl
(require (quote $1))
],[dnl
(featurep (quote $1))
])])


AC_DEFUN([AX_ELISP_CHECK_FBOUNDP],[dnl
dnl 1 -- Emacs Lisp symbol
dnl 2 -- (optional) space-separated list of features to 'require'
AX_ELISP_CHECK([$1],[if '$1' is defined],[dnl
m4_foreach([FEATURE],m4_split(m4_normalize($2)),[dnl
(require 'FEATURE)
])],[(fboundp '$1)])])dnl


AC_DEFUN([AX_ELISP_CHECK_BOUNDP],[dnl
dnl 1 -- Emacs Lisp symbol
dnl 2 -- (optional) space-separated list of features to 'require'
AX_ELISP_CHECK([$1],[if '$1' is defined],[dnl
m4_foreach([FEATURE],m4_split(m4_normalize($2)),[dnl
(require 'FEATURE)
])],[(boundp '$1)])])dnl


dnl TODO:
dnl - Add error checking (see FIXME in code).
dnl - Validate m4 mumblings -- is this the right crazy?!
dnl - Add customization of $EMACS invocation.
dnl
dnl Local variables:
dnl mode: autoconf
dnl End:
dnl ax_elisp.m4 ends here
