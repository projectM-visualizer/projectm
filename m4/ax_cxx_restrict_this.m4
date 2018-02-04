# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_cxx_restrict_this.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_RESTRICT_THIS
#
# DESCRIPTION
#
#   Determine whether the C++ compiler supports qualifying a member function
#   with a restricted "this" pointer.  Define "restrict_this" to the correct
#   spelling; use like this:
#
#     T::fn() restrict_this { /* code */ }
#
#   Otherwise, define "restrict_this" to be empty.
#
#   Note: the syntax above is a GCC extension. If your C++ compiler has a
#   different way of applying the 'restricted' qualifier to the "this"
#   pointer, please consider reporting it.
#
# LICENSE
#
#   Copyright (c) 2010 Riccardo Murri <riccardo.murri@gmail.com>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation, either version 3 of the License, or (at your
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
#   You should have received a copy of the GNU General Public License and a
#   copy of the Autoconf Configure Script Exception along with this program;
#   see the files COPYINGv3 and COPYING.EXCEPTION respectively. If not, see
#   <https://www.gnu.org/licenses/>.

#serial 2

AC_DEFUN([AX_CXX_RESTRICT_THIS],
[AC_CACHE_CHECK([whether C++ supports GCC's restrict "this" syntax], ax_cv_cxx_restrict_this,
  [ax_cv_cxx_restrict_this=no
   AC_LANG_PUSH([C++])
   # The order here caters to the fact that C++ does not require restrict.
   for ac_kw in __restrict __restrict__ _Restrict restrict; do
     AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
      [[#define restrict_this $ac_kw
class Foo {
  private:
      int x_;
  public:
      Foo(int x) : x_(x) { };
      int bar(Foo* other);
};
int Foo::bar(Foo* other) restrict_this { return this->x_ + other->x_; }
      ]],
      [[
Foo a(3), b(5);
return (8 == a.bar(&b));
      ]])],
      [ax_cv_cxx_restrict_this=$ac_kw])
     test "$ax_cv_cxx_restrict_this" != no && break
   done
   AC_LANG_POP([C++])
  ])
 AH_VERBATIM([restrict_this],
[/* Define to the keyword(s) used to specify that a member function's
    "this" pointer is unaliased.  Define to nothing if this is not
    supported. */
#undef restrict_this])
 case $ax_cv_cxx_restrict_this in
   no) AC_DEFINE([restrict_this], []) ;;
   *)  AC_DEFINE_UNQUOTED([restrict_this], [$ax_cv_cxx_restrict_this]) ;;
 esac
])# AC_C_RESTRICT
