# ===========================================================================
#        https://www.gnu.org/software/autoconf-archive/ax_xercesc.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_XERCESC
#
# DESCRIPTION
#
#   Define $HAVE_LIBXERCES_C, $LIBXERCES_C, and $LTLIBXERCES_C to signify
#   whether Xerces is available.
#
# LICENSE
#
#   Copyright (c) 2008 Ronald Landheer-Cieslak <blytkerchan@users.sourceforge.net>
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

#serial 8

AU_ALIAS([RLC_XERCESC], [AX_XERCESC])
AC_DEFUN([AX_XERCESC],
[
  dnl Prerequisites of AC_LIB_LINKFLAGS_BODY.
  AC_REQUIRE([AC_LIB_PREPARE_PREFIX])
  AC_REQUIRE([AC_LIB_RPATH])

  AC_LANG_PUSH(C++)
  AX_XERCESC_BODY()

  ac_save_CPPFLAGS="$CPPFLAGS"
  AC_LIB_APPENDTOVAR([CPPFLAGS], [$INCXERCES_C])

  AC_CACHE_CHECK([for libxerces-c], [ac_cv_libxerces_c], [
    ac_save_LIBS="$LIBS"
    LIBS="$LIBS $LIBXERCES_C $LIBICONV"
    AC_TRY_LINK([
#include <xercesc/util/PlatformUtils.hpp>
#ifdef XERCES_CPP_NAMESPACE_USE
XERCES_CPP_NAMESPACE_USE
#endif
         ], [
try
{
   XMLPlatformUtils::Initialize();
}
catch (...)
{
   // ...
}
XMLPlatformUtils::Terminate();
         ], [ac_cv_libxerces_c=yes], [ac_cv_libxerces_c=no])
    LIBS="$ac_save_LIBS"
  ])
  if test "$ac_cv_libxerces_c" = yes; then
    HAVE_LIBXERCES_C=yes
    AC_DEFINE(HAVE_LIBXERCES_C, 1, [Define if you have the xerces-c library.])
    AC_MSG_CHECKING([how to link with libxerces-c])
    AC_MSG_RESULT([$LIBXERCES_C])
  else
    HAVE_LIBXERCES_C=no
    CPPFLAGS="$ac_save_CPPFLAGS"
    LIBXERCES_C=
    LTLIBXERCES_C=
  fi

  AC_LANG_POP(C++)

  AC_SUBST(HAVE_LIBXERCES_C)
  AC_SUBST(LIBXERCES_C)
  AC_SUBST(LTLIBXERCES_C)
])

AC_DEFUN([AX_XERCESC_BODY],
[
  dnl By default, look in $includedir and $libdir.
  use_additional=yes
  AC_LIB_WITH_FINAL_PREFIX([
    eval additional_includedir=\"$includedir\"
    eval additional_libdir=\"$libdir\"
  ])
  AC_ARG_WITH([libxerces-c-prefix],
[  --with-libxerces-c-prefix[=DIR]  search for libxerces-c in DIR/include and DIR/lib
  --without-libxerces-c-prefix     don't search for libxerces-c in includedir and libdir],
[
    if test "X$withval" = "Xno"; then
      use_additional=no
    else
      if test "X$withval" = "X"; then
        AC_LIB_WITH_FINAL_PREFIX([
          eval additional_includedir=\"$includedir\"
          eval additional_libdir=\"$libdir\"
        ])
      else
        additional_includedir="$withval/include"
        additional_libdir="$withval/lib"
      fi
    fi
])
  dnl Search the library and its dependencies in $additional_libdir and
  dnl $LDFLAGS. Using breadth-first-search.
  LIBXERCES_C=
  LTLIBXERCES_C=
  INCXERCES_C=
  rpathdirs=
  ltrpathdirs=
  names_already_handled=
  names_next_round='xerces-c'
  while test -n "$names_next_round"; do
    names_this_round="$names_next_round"
    names_next_round=
    for name in $names_this_round; do
      already_handled=
      for n in $names_already_handled; do
        if test "$n" = "$name"; then
          already_handled=yes
          break
        fi
      done
      if test -z "$already_handled"; then
        names_already_handled="$names_already_handled $name"
        dnl See if it was already located by an earlier AC_LIB_LINKFLAGS
        dnl or AC_LIB_HAVE_LINKFLAGS call.
        uppername=`echo "$name" | sed -e 'y|abcdefghijklmnopqrstuvwxyz./-|ABCDEFGHIJKLMNOPQRSTUVWXYZ___|'`
        eval value=\"\$HAVE_LIB$uppername\"
        if test -n "$value"; then
          if test "$value" = yes; then
            eval value=\"\$LIB$uppername\"
            test -z "$value" || LIBXERCES_C="${LIBXERCES_C}${LIBXERCES_C:+ }$value"
            eval value=\"\$LTLIB$uppername\"
            test -z "$value" || LTLIBXERCES_C="${LTLIBXERCES_C}${LTLIBXERCES_C:+ }$value"
          else
            dnl An earlier call to AC_LIB_HAVE_LINKFLAGS has determined
            dnl that this library doesn't exist. So just drop it.
            :
          fi
        else
          dnl Search the library lib$name in $additional_libdir and $LDFLAGS
          found_dir=
          found_la=
          found_so=
          found_a=
          if test $use_additional = yes; then
            found_so="$additional_libdir/lib$name.$shlibext*"
            for i in $found_so; do
              if test -f $i && test -x $i; then
                found_so="$i"
                break
              fi
            done
            if test -n "$shlibext" && test -f $found_so; then
              found_dir="$additional_libdir"
              if test -f "$additional_libdir/lib$name.la"; then
                found_la="$additional_libdir/lib$name.la"
              fi
            else
              if test -f "$additional_libdir/lib$name.$libext"; then
                found_dir="$additional_libdir"
                found_a="$additional_libdir/lib$name.$libext"
                if test -f "$additional_libdir/lib$name.la"; then
                  found_la="$additional_libdir/lib$name.la"
                fi
              fi
            fi
          fi
          if test "X$found_dir" = "X"; then
            for x in $LDFLAGS $LTLIBXERCES_C; do
              AC_LIB_WITH_FINAL_PREFIX([eval x=\"$x\"])
              case "$x" in
                -L*)
                  dir=`echo "X$x" | sed -e 's/^X-L//'`
                  if test -n "$shlibext"; then
                    found_dir="$dir"
                    found_so="$dir/lib$name.$shlibext*"
                         for i in $found_so; do
                                                    if test -f $i && test -x $i; then
                                                      found_so="$i"
                                                      break
                                                    fi
                                                  done

                    if test -f "$dir/lib$name.la"; then
                      found_la="$dir/lib$name.la"
                    fi
                  else
                    if test -f "$dir/lib$name.$libext"; then
                      found_dir="$dir"
                      found_a="$dir/lib$name.$libext"
                      if test -f "$dir/lib$name.la"; then
                        found_la="$dir/lib$name.la"
                      fi
                    fi
                  fi
                  ;;
              esac
              if test "X$found_dir" != "X"; then
                break
              fi
            done
          fi
          if test "X$found_dir" != "X"; then
            dnl Found the library.
            LTLIBXERCES_C="${LTLIBXERCES_C}${LTLIBXERCES_C:+ }-L$found_dir -l$name"
            if test "X$found_so" != "X"; then
              dnl Linking with a shared library. We attempt to hardcode its
              dnl directory into the executable's runpath, unless it's the
              dnl standard /usr/lib.
              if test "$enable_rpath" = no || test "X$found_dir" = "X/usr/lib"; then
                dnl No hardcoding is needed.
                LIBXERCES_C="${LIBXERCES_C}${LIBXERCES_C:+ }$found_so"
              else
                dnl Use an explicit option to hardcode DIR into the resulting
                dnl binary.
                dnl Potentially add DIR to ltrpathdirs.
                haveit=
                for x in $ltrpathdirs; do
                  if test "X$x" = "X$found_dir"; then
                    haveit=yes
                    break
                  fi
                done
                if test -z "$haveit"; then
                  ltrpathdirs="$ltrpathdirs $found_dir"
                fi
                if test "$hardcode_direct" = yes; then
                  dnl resulting binary.
                  LIBXERCES_C="${LIBXERCES_C}${LIBXERCES_C:+ }$found_so"
                else
                  if test -n "$hardcode_libdir_flag_spec" && test "$hardcode_minus_L" = no; then
                    dnl Use an explicit option to hardcode DIR into the resulting
                    dnl binary.
                    LIBXERCES_C="${LIBXERCES_C}${LIBXERCES_C:+ }$found_so"
                    dnl Potentially add DIR to rpathdirs.
                    haveit=
                    for x in $rpathdirs; do
                      if test "X$x" = "X$found_dir"; then
                        haveit=yes
                        break
                      fi
                    done
                    if test -z "$haveit"; then
                      rpathdirs="$rpathdirs $found_dir"
                    fi
                  else
                    dnl Rely on "-L$found_dir".
                    dnl But don't add it if it's already contained in the LDFLAGS
                    haveit=
                    for x in $LDFLAGS $LIBXERCES_C; do
                      AC_LIB_WITH_FINAL_PREFIX([eval x=\"$x\"])
                      if test "X$x" = "X-L$found_dir"; then
                        haveit=yes
                        break
                      fi
                    done
                    if test -z "$haveit"; then
                      LIBXERCES_C="${LIBXERCES_C}${LIBXERCES_C:+ }-L$found_dir"
                    fi
                    if test "$hardcode_minus_L" != no; then
                      dnl FIXME: Not sure whether we should use
                      dnl "-L$found_dir -l$name" or "-L$found_dir $found_so"
                      dnl here.
                      LIBXERCES_C="${LIBXERCES_C}${LIBXERCES_C:+ }$found_so"
                    else
                      dnl We cannot use $hardcode_runpath_var and LD_RUN_PATH
                      dnl here, because this doesn't fit in flags passed to the
                      dnl compiler. So give up. No hardcoding. This affects only
                      dnl very old systems.
                      dnl FIXME: Not sure whether we should use
                      dnl "-L$found_dir -l$name" or "-L$found_dir $found_so"
                      dnl here.
                      LIBXERCES_C="${LIBXERCES_C}${LIBXERCES_C:+ }-l$name"
                    fi
                  fi
                fi
              fi
            else
              if test "X$found_a" != "X"; then
                dnl Linking with a static library.
                LIBXERCES_C="${LIBXERCES_C}${LIBXERCES_C:+ }$found_a"
              else
                dnl We shouldn't come here, but anyway it's good to have a
                dnl fallback.
                LIBXERCES_C="${LIBXERCES_C}${LIBXERCES_C:+ }-L$found_dir -l$name"
              fi
            fi
            dnl Assume the include files are nearby.
            additional_includedir=
            case "$found_dir" in
              */lib | */lib/)
                basedir=`echo "X$found_dir" | sed -e 's,^X,,' -e 's,/lib/*$,,'`
                additional_includedir="$basedir/include"
                ;;
            esac
            if test "X$additional_includedir" != "X"; then
              dnl But don't add it
              dnl   1. if it's the standard /usr/include,
              dnl   2. if it's /usr/local/include and we are using GCC on Linux,
              dnl   3. if it's already present in $CPPFLAGS or the already
              dnl   4. if it doesn't exist as a directory.
              if test "X$additional_includedir" != "X/usr/include"; then
                haveit=
                if test "X$additional_includedir" = "X/usr/local/include"; then
                  if test -n "$GCC"; then
                    case $host_os in
                      linux*) haveit=yes;;
                    esac
                  fi
                fi
                if test -z "$haveit"; then
                  for x in $CPPFLAGS $INCXERCES_C; do
                    AC_LIB_WITH_FINAL_PREFIX([eval x=\"$x\"])
                    if test "X$x" = "X-I$additional_includedir"; then
                      haveit=yes
                      break
                    fi
                  done
                  if test -z "$haveit"; then
                    if test -d "$additional_includedir"; then
                      INCXERCES_C="${INCXERCES_C}${INCXERCES_C:+ }-I$additional_includedir"
                    fi
                  fi
                fi
              fi
            fi
            dnl Look for dependencies.
            if test -n "$found_la"; then
              dnl Read the .la file. It defines the variables
              dnl dlname, library_names, old_library, dependency_libs, current,
              dnl age, revision, installed, dlopen, dlpreopen, libdir.
              save_libdir="$libdir"
              case "$found_la" in
                */* | *\\*) . "$found_la" ;;
                *) . "./$found_la" ;;
              esac
              libdir="$save_libdir"
              dnl We use only dependency_libs.
              for dep in $dependency_libs; do
                case "$dep" in
                  -L*)
                    additional_libdir=`echo "X$dep" | sed -e 's/^X-L//'`
                    dnl But don't add it
                    dnl   1. if it's the standard /usr/lib,
                    dnl   2. if it's /usr/local/lib and we are using GCC on Linux,
                    dnl   3. if it's already present in $LDFLAGS or the already
                    dnl   4. if it doesn't exist as a directory.
                    if test "X$additional_libdir" != "X/usr/lib"; then
                      haveit=
                      if test "X$additional_libdir" = "X/usr/local/lib"; then
                        if test -n "$GCC"; then
                          case $host_os in
                            linux*) haveit=yes;;
                          esac
                        fi
                      fi
                      if test -z "$haveit"; then
                        haveit=
                        for x in $LDFLAGS $LIBXERCES_C; do
                          AC_LIB_WITH_FINAL_PREFIX([eval x=\"$x\"])
                          if test "X$x" = "X-L$additional_libdir"; then
                            haveit=yes
                            break
                          fi
                        done
                        if test -z "$haveit"; then
                          if test -d "$additional_libdir"; then
                            LIBXERCES_C="${LIBXERCES_C}${LIBXERCES_C:+ }-L$additional_libdir"
                          fi
                        fi
                        haveit=
                        for x in $LDFLAGS $LTLIBXERCES_C; do
                          AC_LIB_WITH_FINAL_PREFIX([eval x=\"$x\"])
                          if test "X$x" = "X-L$additional_libdir"; then
                            haveit=yes
                            break
                          fi
                        done
                        if test -z "$haveit"; then
                          if test -d "$additional_libdir"; then
                            LTLIBXERCES_C="${LTLIBXERCES_C}${LTLIBXERCES_C:+ }-L$additional_libdir"
                          fi
                        fi
                      fi
                    fi
                    ;;
                  -R*)
                    dir=`echo "X$dep" | sed -e 's/^X-R//'`
                    if test "$enable_rpath" != no; then
                    dnl Potentially add DIR to rpathdirs.
                    haveit=
                    for x in $rpathdirs; do
                      if test "X$x" = "X$dir"; then
                        haveit=yes
                        break
                      fi
                    done
                    if test -z "$haveit"; then
                      rpathdirs="$rpathdirs $dir"
                    fi
                    dnl Potentially add DIR to ltrpathdirs.
                    haveit=
                    for x in $ltrpathdirs; do
                      if test "X$x" = "X$dir"; then
                        haveit=yes
                        break
                      fi
                    done
                    if test -z "$haveit"; then
                      ltrpathdirs="$ltrpathdirs $dir"
                    fi
                    fi
                    ;;
                  -l*)
                    dnl Handle this in the next round.
                    names_next_round="$names_next_round "`echo "X$dep" | sed -e 's/^X-l//'`
                    ;;
                  *.la)
                    dnl Handle this in the next round. Throw away the .la's
                    dnl directory; it is already contained in a preceding -L
                    dnl option.
                    names_next_round="$names_next_round "`echo "X$dep" | sed -e 's,^X.*/,,' -e 's,^lib,,' -e 's,\.la$,,'`
                    ;;
                  *)
                    dnl Most likely an immediate library name.
                    LIBXERCES_C="${LIBXERCES_C}${LIBXERCES_C:+ }$dep"
                    LTLIBXERCES_C="${LTLIBXERCES_C}${LTLIBXERCES_C:+ }$dep"
                    ;;
                esac
              done
            fi
          else
            dnl Didn't find the library; assume it is in the system directories
            dnl known to the linker and runtime loader. (All the system
            dnl directories known to the linker should also be known to the
            dnl runtime loader, otherwise the system is severely misconfigured.)
            LIBXERCES_C="${LIBXERCES_C}${LIBXERCES_C:+ }-l$name"
            LTLIBXERCES_C="${LTLIBXERCES_C}${LTLIBXERCES_C:+ }-l$name"
          fi
        fi
      fi
    done
  done
  if test "X$rpathdirs" != "X"; then
    if test -n "$hardcode_libdir_separator"; then
      dnl Weird platform: only the last -rpath option counts, the user must
      dnl pass all path elements in one option. We can arrange that for a
      alldirs=
      for found_dir in $rpathdirs; do
        alldirs="${alldirs}${alldirs:+$hardcode_libdir_separator}$found_dir"
      done
      dnl Note: hardcode_libdir_flag_spec uses $libdir and $wl.
      acl_save_libdir="$libdir"
      libdir="$alldirs"
      eval flag=\"$hardcode_libdir_flag_spec\"
      libdir="$acl_save_libdir"
      LIBXERCES_C="${LIBXERCES_C}${LIBXERCES_C:+ }$flag"
    else
      dnl The -rpath options are cumulative.
      for found_dir in $rpathdirs; do
        acl_save_libdir="$libdir"
        libdir="$found_dir"
        eval flag=\"$hardcode_libdir_flag_spec\"
        libdir="$acl_save_libdir"
        LIBXERCES_C="${LIBXERCES_C}${LIBXERCES_C:+ }$flag"
      done
    fi
  fi
  if test "X$ltrpathdirs" != "X"; then
    dnl When using libtool, the option that works for both libraries and
    dnl executables is -R. The -R options are cumulative.
    for found_dir in $ltrpathdirs; do
      LTLIBXERCES_C="${LTLIBXERCES_C}${LTLIBXERCES_C:+ }-R$found_dir"
    done
  fi
])
