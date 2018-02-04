# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_sys_perlsharpbang.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_SYS_PERLSHARPBANG
#
# DESCRIPTION
#
#   Determine how the perl interpreter is located by the OS kernel and make
#   substitution variable PERL_SHEBANG available. Does AC_PATH_PROG to find
#   the path to perl. As a side-effect, that sets PERLINTERP and makes it
#   available as a substitution variable.
#
#   Note: The macro allows for the possibility (expected to be seldom used)
#   of an explicit user override (the "user" being the operator executing
#   the final 'configure' script, in this context) by making the option
#   argument like:
#
#     --with-perl-shebang='#! /my/funky/perlpath' # OR
#     --with-perl-shebang='/my/funky/perlpath'  # we just throw away the #! anyway
#                                               # bec it must be absent in Makefile
#
#   Rationale: The are various ways of starting an interpreter on different
#   *nix-like systems. Many use the simple
#
#     #!/usr/bin/perl
#
#   but it could be instead
#
#     #!/usr/local/bin/perl
#
#   and there is even the possibility that the user wants
#
#     #!/usr/bin/env perl
#
#   to find whichever perl comes first in the current $PATH. This is
#   preferred by some of us because we run multiple perl installations on
#   the same box. Adjusting our $PATH then allows us to set precedence over
#   other perls, even whatever the "house" version is.
#
#   Users on very non-unix systems like MS Windows do not have a kernel that
#   does this kind of thing from the first line of script files, but instead
#   the perl on their machine is started and merely notices whatever comes
#   after the interpreter path on this first line of the script (options
#   like "-w").
#
#   Acknowledgement: this macro was in part inspired by Dean Povey's
#   AC_PROG_PERL_VERSION.
#
# LICENSE
#
#   Copyright (c) 2009 Soren Andersen <somian@pobox.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 14

AC_DEFUN([AX_SYS_PERLSHARPBANG],[dnl

   AC_PATH_PROG(PERLINTERP,perl,perl)
   ac_cv_path_perlinterp="$PERLINTERP"
   _sHpB='#!'

   AC_ARG_WITH(perl-shebang,
                AS_HELP_STRING([--with-perl-shebang],
           [override what perl thinks is the way for the kernel to start it (seldom needed)]dnl
			   ),
		[opt_perl_shebang="$withval"]dnl
			    ,dnl
		[opt_perl_shebang="not_set"]dnl
    )dnl

   AC_CACHE_CHECK([whether explicit instead of detected sharpbang is to be used],
		   ax_cv_opt_perl_shebang,
		  [ case "$opt_perl_shebang" in
		      not_set  ) ax_cv_opt_perl_shebang=''
			       ;;
			 *     )
	ax_cv_opt_perl_shebang=`echo "$opt_perl_shebang" | sed -e's|^#!\s*\(.*\)$|\1|'`
		    esac
		  ]dnl
    )dnl

   if test "A$ax_cv_opt_perl_shebang" != "A"
     then
       ac_cv_sys_kernshrpbang_perl="$ax_cv_opt_perl_shebang"
       PERL_SHEBANG="$ac_cv_sys_kernshrpbang_perl"
       AC_SUBST(PERL_SHEBANG)dnl
       AC_MSG_NOTICE([OK - PERL_SHEBANG is $_sHpB$PERL_SHEBANG.])

# Automatic detection of sharpbang formula starts here
     else dnl

   _somian_shbangperl=`$PERLINTERP -V:startperl`
   negclass="[[^']]"; dnl
# must leave this comment:  m4 will remove the outer brackets for us, heheh
   AC_CACHE_CHECK([for kernel sharpbang invocation to start perl],
		  ac_cv_sys_kernshrpbang_perl,
	[_somian_kspb_perl=`echo "$_somian_shbangperl" | sed -ne"s|.*='\($negclass*\)';$|\1|p"`
	if test "x$_somian_kspb_perl" == x
	  then _somian_ksbp_warn_empty='durnit'
	  else
	  case "A$_somian_kspb_perl" in
		 A#!*perl* )
           ac_cv_sys_kernshrpbang_perl=`echo "$_somian_kspb_perl" | sed -e's|#!\(.*\)$|\1|'`
			;;
		     A*    )  _somian_ksbp_warn_defau='trouble'
			      ac_cv_sys_kernshrpbang_perl="$PERLINTERP"
	  esac
	fi
])dnl Done with testing sharpbang

# The above prints Checking ... result message to user.
   PERL_SHEBANG="$ac_cv_sys_kernshrpbang_perl"
   AC_SUBST(PERL_SHEBANG)
    if test A${_somian_ksbp_warn_empty+set} == Aset
      then   AC_MSG_WARN([dnl
In last check, doing $PERLINTERP -V:startperl yielded empty result! That should not happen.])
    fi
# Inform user after printing result value
    if test A${_somian_ksbp_warn_defau+set} == Aset
      then AC_MSG_NOTICE([Maybe Not good -])
	   AC_MSG_WARN([dnl
In last check perl's Config query did not work so we bunted: $_sHpB$PERLINTERP])
      else AC_MSG_NOTICE([OK Good result - ])
	   AC_MSG_NOTICE([dnl
In last check we got a proper-looking answer from perl's Config: $_somian_shbangperl])
dnl Done with user info messages
    fi
dnl Outer loop checked for user override term here
  fi dnl

])dnl EOMACRO DEF
