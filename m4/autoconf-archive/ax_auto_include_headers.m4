# ============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_auto_include_headers.html
# ============================================================================
#
# SYNOPSIS
#
#   AX_AUTO_INCLUDE_HEADERS(INCLUDE-FILE ...)
#
# DESCRIPTION
#
#   Given a space-separated list of INCLUDE-FILEs, AX_AUTO_INCLUDE_HEADERS
#   will output a conditional #include for each INCLUDE-FILE. The following
#   example demonstrates how AX_AUTO_INCLUDE_HEADERS's might be used in a
#   configure.ac script:
#
#     AH_BOTTOM([
#       AX_AUTO_INCLUDE_HEADERS([sys/resource.h invent.h sys/sysinfo.h])dnl
#     ])
#
#   The preceding invocation instructs autoheader to put the following code
#   at the bottom of the config.h file:
#
#     #ifdef HAVE_SYS_RESOURCE_H
#     # include <sys/resource.h>
#     #endif
#     #ifdef HAVE_INVENT_H
#     # include <invent.h>
#     #endif
#     #ifdef HAVE_SYS_SYSINFO_H
#     # include <sys/sysinfo.h>
#     #endif
#
#   Note that AX_AUTO_INCLUDE_HEADERS merely outputs #ifdef/#include/#endif
#   blocks. The configure.ac script still needs to invoke AC_CHECK_HEADERS
#   to #define the various HAVE_*_H preprocessor macros.
#
#   Here's an easy way to get from config.h a complete list of header files
#   who existence is tested by the configure script:
#
#     cat config.h | perl -ane '/ HAVE_\S+_H / && do {$_=$F[$#F-1]; s/^HAVE_//; s/_H/.h/; s|_|/|g; tr/A-Z/a-z/; print "$_ "}'
#
#   You can then manually edit the resulting list and incorporate it into
#   one or more calls to AX_AUTO_INCLUDE_HEADERS.
#
# LICENSE
#
#   Copyright (c) 2008 Scott Pakin <pakin@uiuc.edu>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 9

AC_DEFUN([AX_AUTO_INCLUDE_HEADERS], [dnl
AC_FOREACH([AX_Header], [$1], [dnl
m4_pushdef([AX_IfDef], AS_TR_CPP(HAVE_[]AX_Header))dnl
[#]ifdef AX_IfDef
[#] include <AX_Header>
[#]endif
m4_popdef([AX_IfDef])dnl
])])
