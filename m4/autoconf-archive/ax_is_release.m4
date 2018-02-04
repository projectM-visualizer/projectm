# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_is_release.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_IS_RELEASE(POLICY)
#
# DESCRIPTION
#
#   Determine whether the code is being configured as a release, or from
#   git. Set the ax_is_release variable to 'yes' or 'no'.
#
#   If building a release version, it is recommended that the configure
#   script disable compiler errors and debug features, by conditionalising
#   them on the ax_is_release variable.  If building from git, these
#   features should be enabled.
#
#   The POLICY parameter specifies how ax_is_release is determined. It can
#   take the following values:
#
#    * git-directory:  ax_is_release will be 'no' if a '.git' directory exists
#    * minor-version:  ax_is_release will be 'no' if the minor version number
#                      in $PACKAGE_VERSION is odd; this assumes
#                      $PACKAGE_VERSION follows the 'major.minor.micro' scheme
#    * micro-version:  ax_is_release will be 'no' if the micro version number
#                      in $PACKAGE_VERSION is odd; this assumes
#                      $PACKAGE_VERSION follows the 'major.minor.micro' scheme
#    * dash-version:   ax_is_release will be 'no' if there is a dash '-'
#                      in $PACKAGE_VERSION, for example 1.2-pre3, 1.2.42-a8b9
#                      or 2.0-dirty (in particular this is suitable for use
#                      with git-version-gen)
#    * always:         ax_is_release will always be 'yes'
#    * never:          ax_is_release will always be 'no'
#
#   Other policies may be added in future.
#
# LICENSE
#
#   Copyright (c) 2015 Philip Withnall <philip@tecnocode.co.uk>
#   Copyright (c) 2016 Collabora Ltd.
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

#serial 7

AC_DEFUN([AX_IS_RELEASE],[
    AC_BEFORE([AC_INIT],[$0])

    m4_case([$1],
      [git-directory],[
        # $is_release = (.git directory does not exist)
        AS_IF([test -d ${srcdir}/.git],[ax_is_release=no],[ax_is_release=yes])
      ],
      [minor-version],[
        # $is_release = ($minor_version is even)
        minor_version=`echo "$PACKAGE_VERSION" | sed 's/[[^.]][[^.]]*.\([[^.]][[^.]]*\).*/\1/'`
        AS_IF([test "$(( $minor_version % 2 ))" -ne 0],
              [ax_is_release=no],[ax_is_release=yes])
      ],
      [micro-version],[
        # $is_release = ($micro_version is even)
        micro_version=`echo "$PACKAGE_VERSION" | sed 's/[[^.]]*\.[[^.]]*\.\([[^.]]*\).*/\1/'`
        AS_IF([test "$(( $micro_version % 2 ))" -ne 0],
              [ax_is_release=no],[ax_is_release=yes])
      ],
      [dash-version],[
        # $is_release = ($PACKAGE_VERSION has a dash)
        AS_CASE([$PACKAGE_VERSION],
                [*-*], [ax_is_release=no],
                [*], [ax_is_release=yes])
      ],
      [always],[ax_is_release=yes],
      [never],[ax_is_release=no],
      [
        AC_MSG_ERROR([Invalid policy. Valid policies: git-directory, minor-version, micro-version, dash-version, always, never.])
      ])
])
