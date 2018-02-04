# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_open62541_check_h.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_OPEN62541_CHECK_H([HEADERS = `...see_below...'], [ACTION-IF-FOUND],
#                        [ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   Searches for the header file(s) of the open62541 library [1].
#
#   The open62541 library is a cmake-based project, that provides its header
#   files in one of two layouts:
#
#     1) As individual files, e.g. ua_config.h, ua_server.h, ua_types.h, ...
#     2) As a single amalgamation file open62541.h
#
#   The second case is enabled when configuring open62541 with the options
#   "-D UA_ENABLE_AMALGAMATION=true to" cmake, which seems to be preferred.
#   Code using the library can distinguish which layout is used by checking
#   for the macro "UA_NO_AMALGAMATION": if it is defined, the first layout
#   is used.
#
#   The AX_OPEN62541_CHECK_H macro checks first for the amalgamation and, if
#   that is not found, for the individual headers. It defines
#   "UA_NO_AMALGAMATION" if necessary.
#
#   The individual headers to check for if no amalgamation is found can be
#   provided as a space-separated list in the first argument. If that is
#   empty, it defaults to all files known to be contained in the
#   amalgamation as of v0.2.0 of the library:
#
#     * ms_stdint.h
#     * ua_client.h
#     * ua_client_highlevel.h
#     * ua_config.h
#     * ua_config_standard.h
#     * ua_connection.h
#     * ua_constants.h
#     * ua_job.h
#     * ua_log.h
#     * ua_log_stdout.h
#     * ua_network_tcp.h
#     * ua_nodeids.h
#     * ua_server.h
#     * ua_server_external_ns.h
#     * ua_types.h
#     * ua_types_generated.h
#     * ua_types_generated_handling.h
#
#   If the with_open62541 shell variable is set to "no" (e.g. from running
#   the AX_OPEN62541_PATH macro and the user giving configure the option
#   "--without-open62541"), then expands ACTION-IF-NOT-FOUND without any
#   checks.
#
#   [1]: <http://open62541.org/>
#
# LICENSE
#
#   Copyright (c) 2016,2017 Olaf Mandel <olaf@mandel.name>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.  This file is offered as-is, without any
#   warranty.

#serial 3

# AX_OPEN62541_CHECK_H([HEADERS], [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# -------------------------------------------------------------------------
AC_DEFUN([AX_OPEN62541_CHECK_H],
[m4_pushdef([headers], m4_normalize([$1]))dnl
m4_ifblank(m4_defn([headers]), [m4_define([headers],
 [ms_stdint.h]dnl
 [ua_client.h]dnl
 [ua_client_highlevel.h]dnl
 [ua_config.h]dnl
 [ua_config_standard.h]dnl
 [ua_connection.h]dnl
 [ua_constants.h]dnl
 [ua_job.h]dnl
 [ua_log.h]dnl
 [ua_log_stdout.h]dnl
 [ua_network_tcp.h]dnl
 [ua_nodeids.h]dnl
 [ua_server.h]dnl
 [ua_server_external_ns.h]dnl
 [ua_types.h]dnl
 [ua_types_generated.h]dnl
 [ua_types_generated_handling.h])])dnl
dnl ua_server_external_ns.h depends on ua_server.h but fails to include it:
dnl so specify the includes:
pushdef([includes],
[#ifdef HAVE_UA_SERVER_H
#    include <ua_server.h>
#endif])dnl
dnl
AS_IF([test x${with_open62541:+set} != xset -o "x$with_open62541" != xno],
      [AC_CHECK_HEADERS([open62541.h], [$2],
          [AC_CHECK_HEADERS(m4_defn([headers]), [$2]dnl
[AC_DEFINE([UA_NO_AMALGAMATION], [1],
           [Use individual open62541 headers instead of the amalgamation.])],
                            [$3],
                            [m4_defn([includes])])],
                        [AC_INCLUDES_DEFAULT])],
      [$3])
m4_popdef([headers], [includes])dnl
])# AX_OPEN62541_CHECK_H
