# ===========================================================================
#         https://www.gnu.org/software/autoconf-archive/ax_llvm.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LLVM([llvm-libs])
#
# DESCRIPTION
#
#   Test for the existence of llvm, and make sure that it can be linked with
#   the llvm-libs argument that is passed on to llvm-config i.e.:
#
#     llvm --libs <llvm-libs>
#
#   llvm-config will also include any libraries that are depended upon.
#
# LICENSE
#
#   Copyright (c) 2008 Andy Kitchen <agimbleinthewabe@gmail.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 16

AC_DEFUN([AX_LLVM],
[
AC_ARG_WITH([llvm],
	AS_HELP_STRING([--with-llvm@<:@=DIR@:>@], [use llvm (default is yes) - it is possible to specify the root directory for llvm (optional)]),
	[
    if test "$withval" = "no"; then
		want_llvm="no"
    elif test "$withval" = "yes"; then
        want_llvm="yes"
        ac_llvm_config_path=`which llvm-config`
    else
	    want_llvm="yes"
        ac_llvm_config_path="$withval"
	fi
    ],
    [want_llvm="yes"])

	succeeded=no
	if test -z "$ac_llvm_config_path"; then
		ac_llvm_config_path=`which llvm-config`
	fi

	if test "x$want_llvm" = "xyes"; then
		if test -e "$ac_llvm_config_path"; then
			LLVM_CPPFLAGS=`$ac_llvm_config_path --cxxflags`
			LLVM_LDFLAGS="$($ac_llvm_config_path --ldflags) $($ac_llvm_config_path --libs $1)"

			AC_REQUIRE([AC_PROG_CXX])
			CPPFLAGS_SAVED="$CPPFLAGS"
			CPPFLAGS="$CPPFLAGS $LLVM_CPPFLAGS"
			export CPPFLAGS

			LDFLAGS_SAVED="$LDFLAGS"
			LDFLAGS="$LDFLAGS $LLVM_LDFLAGS"
			export LDFLAGS

			AC_CACHE_CHECK(can compile with and link with llvm([$1]),
						   ax_cv_llvm,
		[AC_LANG_PUSH([C++])
				 AC_LINK_IFELSE([AC_LANG_PROGRAM([[@%:@include <llvm/Module.h>
													]],
					   [[llvm::Module *M = new llvm::Module("test"); return 0;]])],
			   ax_cv_llvm=yes, ax_cv_llvm=no)
		 AC_LANG_POP([C++])
			])

			if test "x$ax_cv_llvm" = "xyes"; then
				succeeded=yes
			fi

			CPPFLAGS="$CPPFLAGS_SAVED"
		LDFLAGS="$LDFLAGS_SAVED"
		else
			succeeded=no
		fi
	fi

		if test "$succeeded" != "yes" ; then
			AC_MSG_ERROR([[We could not detect the llvm libraries make sure that llvm-config is on your path or specified by --with-llvm.]])
		else
			AC_SUBST(LLVM_CPPFLAGS)
			AC_SUBST(LLVM_LDFLAGS)
			AC_DEFINE(HAVE_LLVM,,[define if the llvm library is available])
		fi
])
