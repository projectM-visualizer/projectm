# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_check_struct_for.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_STRUCT_FOR(INCLUDES,STRUCT,MEMBER,DEFINE,[no])
#
# DESCRIPTION
#
#   Checks STRUCT for MEMBER and defines DEFINE if found.
#
# LICENSE
#
#   Copyright (c) 2008 Wes Hardaker <wjhardaker@ucdavis.edu>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 6

AU_ALIAS([AC_CHECK_STRUCT_FOR], [AX_CHECK_STRUCT_FOR])
AC_DEFUN([AX_CHECK_STRUCT_FOR],[
ac_safe_struct=`echo "$2" | sed 'y%./+-%__p_%'`
ac_safe_member=`echo "$3" | sed 'y%./+-%__p_%'`
ac_safe_all="ac_cv_struct_${ac_safe_struct}_has_${ac_safe_member}"
changequote(, )dnl
  ac_uc_define=STRUCT_`echo "${ac_safe_struct}_HAS_${ac_safe_member}" | sed 'y%abcdefghijklmnopqrstuvwxyz./-%ABCDEFGHIJKLMNOPQRSTUVWXYZ___%'`
changequote([, ])dnl

AC_MSG_CHECKING([for $2.$3])
AC_CACHE_VAL($ac_safe_all,
[
if test "x$4" = "x"; then
  defineit="= 0"
elif test "x$4" = "xno"; then
  defineit=""
else
  defineit="$4"
fi
AC_TRY_COMPILE([
$1
],[
struct $2 testit;
testit.$3 $defineit;
], eval "${ac_safe_all}=yes", eval "${ac_safe_all}=no" )
])

if eval "test \"x$`echo ${ac_safe_all}`\" = \"xyes\""; then
  AC_MSG_RESULT(yes)
  AC_DEFINE_UNQUOTED($ac_uc_define)
else
  AC_MSG_RESULT(no)
fi
])
