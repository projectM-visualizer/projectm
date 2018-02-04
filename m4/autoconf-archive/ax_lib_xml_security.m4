# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_lib_xml_security.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_XML_SECURITY([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   This macro provides tests of availability of Apache Xml-Security C++
#   library <http://santuario.apache.org/index.html> of particular version
#   or newer. This macros checks for Apache Xml-Security C++ headers and
#   libraries and defines compilation flags
#
#   Macro supports following options and their values:
#
#   1) Single-option usage:
#
#     --with-xml-security - yes, no or path to Xml-Security installation prefix
#
#   2) Three-options usage (all options are required):
#
#     --with-xml-security=yes
#     --with-xml-security-inc - path to base directory with Xml-Security headers
#     --with-xml-security-lib - linker flags for Xml-Security
#
#   This macro calls:
#
#     AC_SUBST(XML_SECURITY_CFLAGS)
#     AC_SUBST(XML_SECURITY_LDFLAGS)
#     AC_SUBST(XML_SECURITY_VERSION) - only if version requirement is used
#
#   And sets:
#
#     HAVE_XML_SECURITY
#
# LICENSE
#
#   Copyright (c) 2008 Alexander Petry <petry@itwm.fhg.de>
#   Copyright (c) 2008 Mateusz Loskot <mateusz@loskot.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 10

AC_DEFUN([AX_LIB_XML_SECURITY],
[
    AC_ARG_WITH([xml-security],
	 AS_HELP_STRING([--with-xml-security=@<:@ARG@:>@],
	     [use Xml-Security C++ library from given prefix (ARG=path); check standard prefixes (ARG=yes); disable (ARG=no)]
	 ),
	 [
	 if test "$withval" = "yes"; then
	     if test -d /usr/local/include/xsec ; then
		 xml_security_prefix=/usr/local
	     elif test -d /usr/include/xsec ; then
		 xml_security_prefix=/usr
	     else
		 xml_security_prefix=""
	     fi
	     xml_security_requested="yes"
	 elif test -d "$withval"; then
	     xml_security_prefix="$withval"
	     xml_security_requested="yes"
	 else
	     xml_security_prefix=""
	     xml_security_requested="no"
	 fi
	 ],
	 [
	 dnl Default behavior is implicit yes
	 if test -d /usr/local/include/xsec ; then
	     xml_security_prefix=/usr/local
	 elif test -d /usr/include/xsec ; then
	     xml_security_prefix=/usr
	 else
	     xml-security_prefix=""
	 fi
	 ]
    )

    AC_ARG_WITH([xml-security-inc],
	  AS_HELP_STRING([--with-xml-security-inc=@<:@DIR@:>@],
	      [path to Xml-Security C++ headers]
	  ),
	  [xml_security_include_dir="$withval"],
	  [xml_security_include_dir=""]
     )
     AC_ARG_WITH([xml-security-lib],
	  AS_HELP_STRING([--with-xml-security-lib=@<:@ARG@:>@],
	      [link options for Xml-Security C++ Parser libraries]
	  ),
	  [xml_security_lib_flags="$withval"],
	  [xml_security_lib_flags=""]
     )

     XML_SECURITY_CFLAGS=""
     XML_SECURITY_LDFLAGS=""
     XML_SECURITY_VERSION=""

     dnl
     dnl Collect include/lib paths and flags
     dnl
     run_xml_security_test="no"

     if test -n "$xml_security_prefix"; then
	  xml_security_include_dir="$xml_security_prefix/include"
	  xml_security_include_dir2="$xml_security_prefix/include/xsec"
	  xml_security_lib_flags="-L$xml_security_prefix/lib -lxml-security-c -lpthread"
	  run_xml_security_test="yes"
     elif test "$xml_security_requested" = "yes"; then
	  if test -n "$xml_security_include_dir" -a -n "$xml_security_lib_flags"; then
	      xml_security_include_dir2="$xml_security_include_dir/xenc"
	      run_xml_security_test="yes"
	  fi
     else
	  run_xml_security_test="no"
     fi

     dnl
     dnl Check Xml-Security C++ files
     dnl
     if test "$run_xml_security_test" = "yes"; then

	  saved_CPPFLAGS="$CPPFLAGS"
	  CPPFLAGS="$CPPFLAGS -I$xml_security_include_dir -I$xml_security_include_dir2"

	  saved_LDFLAGS="$LDFLAGS"
	  LDFLAGS="$LDFLAGS $xml_security_lib_flags"

	  dnl
	  dnl Check Xml-Security headers
	  dnl
	  AC_MSG_CHECKING([for Xml-Security C++ headers in $xml_security_include_dir and $xml_security_include_dir2])

	  AC_LANG_PUSH([C++])
	  AC_COMPILE_IFELSE([
	      AC_LANG_PROGRAM(
		  [[
 @%:@include <xsec/utils/XSECPlatformUtils.hpp>
		  ]],
		  [[
		  ]]
	      )],
	      [
	      XML_SECURITY_CFLAGS="-I$xml_security_include_dir -I$xml_security_include_dir2"
	      xml_security_header_found="yes"
	      AC_MSG_RESULT([found])
	      ],
	      [
	      xml_security_header_found="no"
	      AC_MSG_RESULT([not found])
	      ]
	  )
	  AC_LANG_POP([C++])

	  dnl
	  dnl Check Xml-Security libraries
	  dnl
	  if test "$xml_security_header_found" = "yes"; then

	      AC_MSG_CHECKING([for Xml-Security C++ libraries])

	      AC_LANG_PUSH([C++])
	      AC_LINK_IFELSE([
		  AC_LANG_PROGRAM(
		      [[
@%:@include <xsec/utils/XSECPlatformUtils.hpp>
#ifdef XERCES_CPP_NAMESPACE_USE
XERCES_CPP_NAMESPACE_USE
#endif
		      ]],
		      [[
XSECPlatformUtils::Initialise();
		      ]]
		  )],
		  [
		  XML_SECURITY_LDFLAGS="$xml_security_lib_flags"
		  xml_security_lib_found="yes"
		  AC_MSG_RESULT([found])
		  ],
		  [
		  xml_security_lib_found="no"
		  AC_MSG_RESULT([not found])
		  ]
	      )
	      AC_LANG_POP([C++])
	  fi

	  CPPFLAGS="$saved_CPPFLAGS"
	  LDFLAGS="$saved_LDFLAGS"
    fi

    AC_MSG_CHECKING([for Xml-Security C++])

    if test "$run_xml_security_test" = "yes"; then
	 if test x"$xml_security_header_found" = x"yes" -a x"$xml_security_lib_found" = x"yes"; then

	     AC_SUBST([XML_SECURITY_CFLAGS])
	     AC_SUBST([XML_SECURITY_LDFLAGS])

	     HAVE_XML_SECURITY="yes"
	 else
	     HAVE_XML_SECURITY="no"
	 fi

	 AC_MSG_RESULT([$HAVE_XML_SECURITY])

	 dnl
	 dnl Check Xml-Security version
	 dnl
	 if test "$HAVE_XML_SECURITY" = "yes"; then

	     xml_security_version_req=ifelse([$1], [], [], [$1])

	     if test  -n "$xml_security_version_req"; then

		 AC_MSG_CHECKING([if Xml-Security C++ version is >= $xml_security_version_req])

		 if test -f "$xml_security_include_dir2/framework/XSECConfig.hpp"; then

		     xml_security_major=`cat $xml_security_include_dir2/framework/XSECConfig.hpp | \
				     grep '^#define.*XSEC_VERSION_MAJOR.*[0-9]$' | \
				     sed -e 's/#define XSEC_VERSION_MAJOR.//'`

		     xml_security_medium=`cat $xml_security_include_dir2/framework/XSECConfig.hpp | \
				     grep '^#define.*XSEC_VERSION_MEDIUM.*[0-9]$' | \
				     sed -e 's/#define XSEC_VERSION_MEDIUM.//'`

		     xml_security_minor=`cat $xml_security_include_dir2/framework/XSECConfig.hpp | \
				     grep '^#define.*XSEC_VERSION_MINOR.*[0-9]$' | \
				     sed -e 's/#define XSEC_VERSION_MINOR.//'`

		     XML_SECURITY_VERSION="$xml_security_major.$xml_security_medium.$xml_security_minor"
		     AC_SUBST([XML_SECURITY_VERSION])

		     dnl Decompose required version string and calculate numerical representation
		     xml_security_version_req_major=`expr $xml_security_version_req : '\([[0-9]]*\)'`
		     xml_security_version_req_medium=`expr $xml_security_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
		     xml_security_version_req_minor=`expr $xml_security_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`

		     xml_security_version_req_number=`expr $xml_security_version_req_major \* 10000 \
						\+ $xml_security_version_req_medium \* 100 \
						\+ $xml_security_version_req_minor`

		     dnl Calculate numerical representation of detected version
		     xml_security_version_number=`expr $xml_security_major \* 10000 \
					   \+ $xml_security_medium \* 100 \
					    \+ $xml_security_minor`

		     xml_security_version_check=`expr $xml_security_version_number \>\= $xml_security_version_req_number`
		     if test "$xml_security_version_check" = "1"; then
			 AC_MSG_RESULT([yes])
		     else
			 AC_MSG_RESULT([no])
			 AC_MSG_WARN([Found Xml-Security C++ $XML_SECURITY_VERSION, which is older than required. Possible compilation failure.])
		     fi
		 else
		     AC_MSG_RESULT([no])
		     AC_MSG_WARN([Missing header XSECConfig.hpp. Unable to determine Xml-Security version.])
		 fi
	     fi
	 fi

    else
	 HAVE_XML_SECURITY="no"
	 AC_MSG_RESULT([$HAVE_XML_SECURITY])

	 if test "$xml_security_requested" = "yes"; then
	     AC_MSG_ERROR([Xml-Security C++ support requested but headers or library not found. Specify valid prefix of Xml-Security C++ using --with-xml-security=@<:@DIR@:>@ or provide headers and linker flags using --with-xml-security-inc and --with-xml-security-lib])
	 fi
    fi
])
