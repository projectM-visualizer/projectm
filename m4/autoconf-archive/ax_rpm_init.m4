# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_rpm_init.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_RPM_INIT
#
# DESCRIPTION
#
#   Setup variables for creation of rpms. It will define several variables
#   useful for creating rpms on a system where rpms are supported.
#   Currently, I requires changes to Makefile.am to function properly (see
#   the example below).
#
#   Also note that I do not use any non-UNIX OSs (and for the most part, I
#   only use RedHat), so this is probably generally not useful for other
#   systems.
#
#   Required setup:
#
#   In configure.in:
#
#     dnl For my rpm.m4 macros
#     RPM_RELEASE=1
#     AC_SUBST(RPM_RELEASE)
#
#     AX_RPM_INIT
#     dnl Enable or disable the rpm making rules in Makefile.am
#     AM_CONDITIONAL(MAKE_RPMS, test x$make_rpms = xtrue)
#
#   Furthermore, the %GNUconfigure rpm macro has a problem in that it does
#   not define CXXFLAGS for the target system correctly, so for compiling
#   C++ code, add the following line _before_ calling AC_PROG_CXX:
#
#     dnl This is a little hack to make this work with rpm better (see mysql++.spec.in)
#     test -z "$CXXFLAGS" && CXXFLAGS="${CFLAGS}"
#
#   Changes to Makefile.am (I am trying to get rid of this step; suggestions
#   invited):
#
#     if MAKE_RPMS
#     rpm: @RPM_TARGET@
#
#     .PHONY: rpm
#
#     $(RPM_TARGET): $(DISTFILES)
#       ${MAKE} dist
#       -mkdir -p $(RPM_DIR)/SRPMS
#       -mkdir -p `dirname $(RPM_TARGET)`
#       $(RPM_PROG) $(RPM_ARGS) $(RPM_TARBALL)
#       @echo Congratulations, $(RPM_TARGET) "(and friends)" should now exist.
#     else
#     endif
#
#   Also, it works best with a XXXX.spec.in file like the following (this is
#   way down on the wishlist, but a program to generate the skeleton spec.in
#   much like autoscan would just kick butt!):
#
#     ---------- 8< ----------
#     # -*- Mode:rpm-spec -*-
#     # mysql++.spec.in
#     Summary: Your package description goes here
#     %define rel @RPM_RELEASE@
#
#     %define version @VERSION@
#     %define pkgname @PACKAGE@
#     %define prefix /usr
#
#     %define lt_release @LT_RELEASE@
#     %define lt_version @LT_CURRENT@.@LT_REVISION@.@LT_AGE@
#
#     # This is a hack until I can figure out how to better handle replacing
#     # autoconf macros... (gotta love autoconf...)
#     %define __aclocal   aclocal || aclocal -I ./macros
#     %define configure_args  @RPM_CONFIGURE_ARGS@
#
#     Name: %{pkgname}
#     Version: %{version}
#     Release: %{rel}
#
#     Copyright: LGPL
#     Group: # your group name goes here
#     Source: %{pkgname}-%{version}.tar.gz
#     Requires: # additional requirements
#     Buildroot: /tmp/%{pkgname}-root
#     URL: http://yoururl.go.here
#     Prefix: %{prefix}
#     BuildArchitectures: # Target platforms, i.e., i586
#     Packager: Your Name <youremail@your.address>
#
#     %description
#     Your package description
#
#     %changelog
#
#     %prep
#     %setup
#     #%patch
#
#     %build
#     %GNUconfigure %{configure_args}
#     # This is why we copy the CFLAGS to the CXXFLAGS in configure.in
#     # CFLAGS="%{optflags}" CXXFLAGS="%{optflags}" ./configure %{_target_platform} --prefix=%{prefix}
#     make
#
#     %install
#     # To make things work with BUILDROOT
#     if [ "$RPM_BUILD_ROOT" != "/tmp/%{pkgname}-root" ]
#     then
#       echo
#       echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#       echo @                                                                    @
#       echo @  RPM_BUILD_ROOT is not what I expected.  Please clean it yourself. @
#       echo @                                                                    @
#       echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#       echo
#     else
#       echo Cleaning RPM_BUILD_ROOT: "$RPM_BUILD_ROOT"
#       rm -rf "$RPM_BUILD_ROOT"
#     fi
#     make DESTDIR="$RPM_BUILD_ROOT" install
#
#     %clean
#     # Call me paranoid, but I do not want to be responsible for nuking
#     # someone's harddrive!
#     if [ "$RPM_BUILD_ROOT" != "/tmp/%{pkgname}-root" ]
#     then
#       echo
#       echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#       echo @                                                                    @
#       echo @  RPM_BUILD_ROOT is not what I expected.  Please clean it yourself. @
#       echo @                                                                    @
#       echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#       echo
#     else
#       echo Cleaning RPM_BUILD_ROOT: "$RPM_BUILD_ROOT"
#       rm -rf "$RPM_BUILD_ROOT"
#     fi
#
#     %files
#     %defattr(-, root, root)
#     # Your application file list goes here
#     # %{prefix}/lib/lib*.so*
#     %doc COPYRIGHT ChangeLog README AUTHORS NEWS
#     %doc doc/*
#
#     # If you install a library
#     %post -p /sbin/ldconfig
#
#     # If you install a library
#     %postun -p /sbin/ldconfig
#
#     %package devel
#     Summary: Development files for %{pkgname}
#     Group: Applications/Databases
#     %description devel
#     Development files for %{pkgname}.
#
#     %files devel
#     %defattr(-, root, root)
#     # Your development files go here
#     # Programmers documentation goes here
#     %doc doc
#
#     # end of file
#     ---------- >8 ----------
#
# LICENSE
#
#   Copyright (c) 2008 Dale K. Hawkins <dhawkins@cdrgts.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 6

dnl AX_RPM_INIT
dnl Figure out how to create rpms for this system and setup for an
dnl automake target

AU_ALIAS([AM_RPM_INIT], [AX_RPM_INIT])
AC_DEFUN([AX_RPM_INIT],
[dnl
AC_REQUIRE([AC_CANONICAL_HOST])
dnl Find the RPM program
AC_ARG_WITH(rpm-prog,[  --with-rpm-prog=PROG   Which rpm to use (optional)],
            rpm_prog="$withval", rpm_prog="")

AC_ARG_ENABLE(rpm-rules, [  --enable-rpm-rules       Try to create rpm make rules (defaults to yes for Linux)],
                enable_rpm_rules="$withval",enable_rpm_rules=no)

AC_ARG_WITH(rpm-extra-args, [  --with-rpm-extra-args=ARGS       Run rpm with extra arguments (defaults to none)],
                rpm_extra_args="$withval", rpm_extra_args="")

dnl AC_ARG_ENABLE(rpm-topdir, [  --enable-rpm       Try to create rpm make rules (defaults to yes for Linux)],
dnl             enable_rpm_rules="$withval",no)

dnl echo enable_rpm_rules is $enable_rpm_rules
dnl echo rpm_prog is $rpm_prog

  RPM_TARGET=""

  if test x$enable_rpm_rules = xno ; then
     echo "Not trying to build rpms for your system (use --enable-rpm-rules to override) "
     no_rpm=yes
  else
    if test x$rpm_prog != x ; then
       if test x${RPM_PROG+set} != xset ; then
          RPM_PROG=$rpm_prog
       fi
    fi

    AC_PATH_PROG(RPM_PROG, rpm, no)
    no_rpm=no
    if test "$RPM_PROG" = "no" ; then
echo *** RPM Configuration Failed
echo *** Failed to find the rpm program.  If you want to build rpm packages
echo *** indicate the path to the rpm program using  --with-rpm-prog=PROG
      no_rpm=yes
      RPM_MAKE_RULES=""
    else
      AC_MSG_CHECKING(how rpm sets %{_rpmdir})
      rpmdir=`rpm --eval %{_rpmdir}`
      if test x$rpmdir = x"%{_rpmdir}" ; then
        AC_MSG_RESULT([not set (cannot build rpms?)])
        echo *** Could not determine the value of %{_rpmdir}
        echo *** This could be because it is not set, or your version of rpm does not set it
        echo *** It must be set in order to generate the correct rpm generation commands
        echo ***
        echo *** You might still be able to create rpms, but I could not automate it for you
        echo *** BTW, if you know this is wrong, please help to improve the rpm.m4 module
        echo *** Send corrections, updates and fixes to dhawkins@cdrgts.com.  Thanks.
      else
        AC_MSG_RESULT([$rpmdir])
      fi
      AC_MSG_CHECKING(how rpm sets %{_rpmfilename})
      rpmfilename=$rpmdir/`rpm --eval %{_rpmfilename} | sed "s/%{ARCH}/${host_cpu}/g" | sed "s/%{NAME}/$PACKAGE/g" | sed "s/%{VERSION}/${VERSION}/g" | sed "s/%{RELEASE}/${RPM_RELEASE}/g"`
      AC_MSG_RESULT([$rpmfilename])

      RPM_DIR=${rpmdir}
      RPM_TARGET=$rpmfilename
      RPM_ARGS="-ta $rpm_extra_args"
      RPM_TARBALL=${PACKAGE}-${VERSION}.tar.gz
    fi
  fi

  case "${no_rpm}" in
    yes) make_rpms=false;;
    no) make_rpms=true;;
    *) AC_MSG_WARN([bad value ${no_rpm} for no_rpm (not making rpms)])
       make_rpms=false;;
  esac
  AC_SUBST(RPM_DIR)
  AC_SUBST(RPM_TARGET)
  AC_SUBST(RPM_ARGS)
  AC_SUBST(RPM_TARBALL)

  RPM_CONFIGURE_ARGS=${ac_configure_args}
  AC_SUBST(RPM_CONFIGURE_ARGS)
])
