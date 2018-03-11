# ===========================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_perl_module_version.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PERL_MODULE_VERSION([MODULE VERSION], [ACTION-IF-TRUE], [ACTION-IF-FALSE])
#
# DESCRIPTION
#
#   Checks to see if the list of 'Module Version' are available in the
#   system. If all the modules in the list are available ACTION-IF-TRUE is
#   executed. Case one module is not available ACTION-IF-FALSE is executed
#   and the macro execution is aborted. NOTE: Perl is needed.
#
#   Example:
#
#     AX_PERL_MODULE_VERSION(CGI::Test 0.104 CGI::Ajax 0.694, ,
#        AC_MSG_ERROR(Need some Perl modules))
#
# LICENSE
#
#   Copyright (c) 2009 Marco Gomes <mpglesi@gmail.com>
#   Copyright (c) 2009 Ruben Fonseca <fonseka@gmail.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 10

AU_ALIAS([AC_PERL_MODULE_VERSION], [AX_PERL_MODULE_VERSION])
AC_DEFUN([AX_PERL_MODULE_VERSION],[dnl
ac_perl_list_modules="$1"
# Make sure we have perl
if test -z "$PERL"; then
AC_CHECK_PROG(PERL,perl,perl)
fi

# Check the number of arguments
args_num=`echo $ac_perl_list_modules | wc -w`
check_args=$(( $args_num % 2 ))
if test "$check_args" = "1" ; then
  AC_MSG_ERROR(syntax error)
else
  eval
fi

if test "x$PERL" != x; then
  ac_failed=0
  while test ${#ac_perl_list_modules} -gt 2 ; do
	module_name=`echo $ac_perl_list_modules | cut -d " " -f 1`
	module_version=`echo $ac_perl_list_modules | cut -d " " -f 2`
	ac_perl_list_modules=`echo $ac_perl_list_modules | cut -d " " -f 3-`
	AC_MSG_CHECKING(for perl module $module_name version $module_version)

	$PERL "-M$module_name" -e exit > /dev/null 2>&1
	if test $? -ne 0; then
	  AC_MSG_RESULT(no);
	  ac_failed=1
	  ac_perl_list_modules=""
	else
	  version=`$PERL "-M$module_name" -e 'print $'"$module_name::VERSION" 2>&1`
	  $PERL -e 'exit(shift cmp shift)' "$version" "$module_version"
	  if test $? -eq 0 -o $? -eq 1 ; then
	    AC_MSG_RESULT(ok);
	  else
	    AC_MSG_RESULT(no)
	    ac_failed=1
	    ac_perl_list_modules=""
	  fi
	fi;
  done

  if test "$ac_failed" = 0; then
    :
    $2
  else
    :
    $3
  fi
else
  AC_MSG_ERROR(could not find perl)
fi])dnl
