# =================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_swig_multi_module_support.html
# =================================================================================
#
# SYNOPSIS
#
#   AX_SWIG_MULTI_MODULE_SUPPORT
#
# DESCRIPTION
#
#   Enable support for multiple modules. This effects all invocations of
#   $(SWIG). You have to link all generated modules against the appropriate
#   SWIG runtime library. If you want to build Python modules for example,
#   use the AX_SWIG_PYTHON macro and link the modules against
#   $(AX_SWIG_PYTHON_LIBS).
#
# LICENSE
#
#   Copyright (c) 2008 Sebastian Huber <sebastian-huber@web.de>
#   Copyright (c) 2008 Alan W. Irwin
#   Copyright (c) 2008 Rafael Laboissiere <rafael@laboissiere.net>
#   Copyright (c) 2008 Andrew Collier
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 2 of the License, or (at your
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

#serial 11

AU_ALIAS([SWIG_MULTI_MODULE_SUPPORT], [AX_SWIG_MULTI_MODULE_SUPPORT])
AC_DEFUN([AX_SWIG_MULTI_MODULE_SUPPORT],[
        AC_REQUIRE([AX_PKG_SWIG])
        SWIG="$SWIG -noruntime"
])
