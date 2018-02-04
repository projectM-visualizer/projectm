# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_zoneinfo.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_ZONEINFO([options...])
#
# DESCRIPTION
#
#   This macro finds compiled zoneinfo files.  If successful it will define
#   HAVE_ZONEINFO per:
#
#     AC_DEFINE([HAVE_ZONEINFO], [1], [...])
#
#   and have the variable TZDIR point to the zoneinfo directory as per
#
#     AC_SUBST([TZDIR])
#     AC_DEFINE_UNQUOTED([TZDIR], [/path/to/zic/files], [...])
#
#   Optionally, OPTIONS can be `right' to trigger further tests that will
#   determine if leap second fix-ups are available.  If so the variables
#   HAVE_ZONEINFO_RIGHT, ZONEINFO_UTC_RIGHT and TZDIR_RIGHT will be
#   populated:
#
#     AC_DEFINE([HAVE_ZONEINFO_RIGHT], [1], [...])
#     AC_SUBST([TZDIR_RIGHT])
#     AC_DEFINE_UNQUOTED([TZDIR_RIGHT], [/path/to/right/zic/files], [...])
#     AC_SUBST([ZONEINFO_UTC_RIGHT])
#     AC_DEFINE_UNQUOTED([ZONEINFO_UTC_RIGHT], [$ZONEINFO_UTC_RIGHT], [...])
#
# LICENSE
#
#   Copyright (c) 2012 Sebastian Freundt <freundt@fresse.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 4

AC_DEFUN([AX_ZONEINFO_TZFILE_H], [dnl
	dnl not totally necessary (yet), as we can simply inspect the tzfiles
	dnl ourselves, but it certainly helps
	AC_CHECK_HEADER([tzfile.h])
])dnl AX_ZONEINFO_TZFILE_H

AC_DEFUN([AX_ZONEINFO_CHECK_TZFILE], [dnl
	dnl AX_ZONEINFO_CHECK_TZFILE([FILE], [ACTION-IF-VALID], [ACTION-IF-NOT])
	dnl secret switch is the 4th argument, which determines the ret code
	dnl of the leapcnt check
	pushdef([probe], [$1])
	pushdef([if_found], [$2])
	pushdef([if_not_found], [$3])

	AC_REQUIRE([AX_ZONEINFO_TZFILE_H])

	if test -z "${ax_tmp_zoneinfo_nested}"; then
		AC_MSG_CHECKING([zoneinfo file ]probe[])
	fi

	AC_LANG_PUSH([C])
	AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

]]ifelse([$4], [], [], [[
#define CHECK_LEAPCNT	]]$4[[
]])[[

/* simplified struct */
struct tzhead {
	char	tzh_magic[4];		/* TZ_MAGIC */
	char	tzh_version[1];		/* '\0' or '2' as of 2005 */
	char	tzh_reserved[15];	/* reserved--must be zero */
	char	tzh_ttisgmtcnt[4];	/* coded number of trans. time flags */
	char	tzh_ttisstdcnt[4];	/* coded number of trans. time flags */
	char	tzh_leapcnt[4];		/* coded number of leap seconds */
	char	tzh_timecnt[4];		/* coded number of transition times */
	char	tzh_typecnt[4];		/* coded number of local time types */
	char	tzh_charcnt[4];		/* coded number of abbr. chars */
};

int
main(int argc, char *argv[])
{
	struct tzhead foo;
	int f;

	if (argc <= 1) {
		return 0;
	} else if ((f = open(argv[1], O_RDONLY, 0644)) < 0) {
		return 1;
	} else if (read(f, &foo, sizeof(foo)) != sizeof(foo)) {
		return 1;
	} else if (close(f) < 0) {
		return 1;
	}

	/* inspect the header */
	if (memcmp(foo.tzh_magic, "TZif", sizeof(foo.tzh_magic))) {
		return 1;
	} else if (*foo.tzh_version && *foo.tzh_version != '2') {
		return 1;
#if defined CHECK_LEAPCNT
	} else if (!foo.tzh_leapcnt[0] && !foo.tzh_leapcnt[1] &&
		   !foo.tzh_leapcnt[2] && !foo.tzh_leapcnt[3]) {
		return CHECK_LEAPCNT;
#endif  /* CHECK_LEAPCNT */
	}

	/* otherwise everything's in order */
	return 0;
}
]])], [## call the whole shebang again with the tzfile
		if ./conftest$EXEEXT probe; then
			if test -z "${ax_tmp_zoneinfo_nested}"; then
				AC_MSG_RESULT([looking good])
			fi
			[]if_found[]
		else
			if test -z "${ax_tmp_zoneinfo_nested}"; then
				AC_MSG_RESULT([looking bad ${ax_tmp_rc}])
			fi
			[]if_not_found[]
		fi
], [
		if test -z "${ax_tmp_zoneinfo_nested}"; then
			AC_MSG_RESULT([impossible])
		fi
		[]if_not_found[]])
	AC_LANG_POP([C])

	popdef([probe])
	popdef([if_found])
	popdef([if_not_found])
])dnl AX_ZONEINFO_CHECK_TZFILE

AC_DEFUN([AX_ZONEINFO_TZDIR], [dnl
	dnl we consider a zoneinfo directory properly populated when it
	dnl provides UTC or UCT or Universal or Zulu

	pushdef([check_tzdir], [dnl
		pushdef([dir], $]1[)dnl
		test -n []dir[] && test -d []dir[] dnl
		popdef([dir])dnl
	])dnl check_tzdir

	dnl try /etc/localtime first, sometimes it's a link into TZDIR
	if test -L "/etc/localtime"; then
		TZDIR_cand="`readlink /etc/localtime` ${TZDIR_cand}"
	fi

	dnl oh, how about we try and check if there is a TZDIR already
	if check_tzdir(["${TZDIR}"]); then
		## bingo
		TZDIR_cand="${TZDIR} ${TZDIR_cand}"
	fi

	dnl often there's a tzselect util which contains the TZDIR path
	AC_PATH_PROG([TZSELECT], [tzselect])
	if test -n "${ac_cv_path_TZSELECT}"; then
		dnl snarf the value
		valtmp="`mktemp`"
		strings "${ac_cv_path_TZSELECT}" | \
			grep -F 'TZDIR=' > "${valtmp}"
		. "${valtmp}"
		TZDIR_cand="${TZDIR} ${TZDIR_cand}"
		rm -f -- "${valtmp}"
	fi

	dnl lastly, append the usual suspects
	TZDIR_cand="${TZDIR_cand} \
/usr/share/zoneinfo \
/usr/lib/zoneinfo \
/usr/local/etc/zoneinfo \
/usr/share/lib/zoneinfo \
"

	dnl go through our candidates
	AC_CACHE_CHECK([for TZDIR], [ax_cv_zoneinfo_tzdir], [dnl
		ax_tmp_zoneinfo_nested="yes"
		for c in ${TZDIR_cand}; do
			ax_cv_zoneinfo_utc=""
			for f in "UTC" "UCT" "Universal" "Zulu"; do
				AX_ZONEINFO_CHECK_TZFILE(["${c}/${f}"], [
					dnl ACTION-IF-FOUND
					ax_cv_zoneinfo_utc="${c}/${f}"
					break
				])
			done
			if test -n "${ax_cv_zoneinfo_utc}"; then
				ax_cv_zoneinfo_tzdir="${c}"
				break
			fi
		done
		ax_tmp_zoneinfo_nested=""
	])dnl ax_cv_tzdir

	TZDIR="${ax_cv_zoneinfo_tzdir}"
	AC_SUBST([TZDIR])

	if check_tzdir(["${ax_cv_zoneinfo_tzdir}"]); then
		AC_DEFINE([HAVE_ZONEINFO], [1], [dnl
Define when zoneinfo directory has been present during configuration.])
		AC_DEFINE_UNQUOTED([TZDIR], ["${ax_cv_zoneinfo_tzdir}"], [
Configuration time zoneinfo directory.])
	fi

	popdef([check_tzdir])
])dnl AX_ZONEINFO_TZDIR

AC_DEFUN([AX_ZONEINFO_RIGHT], [dnl
	AC_REQUIRE([AX_ZONEINFO_TZDIR])

	TZDIR_cand="${TZDIR} \
${TZDIR}/leapseconds \
${TZDIR}-leaps \
${TZDIR}/right \
${TZDIR}-posix \
${TZDIR}/posix \
"

	dnl go through our candidates
	AC_CACHE_CHECK([for leap second file], [ax_cv_zoneinfo_utc_right], [dnl
		ax_tmp_zoneinfo_nested="yes"
		if test -n "${ax_cv_zoneinfo_utc}"; then
			__utc_file="`basename "${ax_cv_zoneinfo_utc}"`"
			for c in ${TZDIR_cand}; do
				if test -d "${c}"; then
					c="${c}/${__utc_file}"
				fi
				AX_ZONEINFO_CHECK_TZFILE(["${c}"], [
					dnl ACTION-IF-FOUND
					ax_cv_zoneinfo_utc_right="${c}"
					break
				], [:], [2])
			done
		fi
		ax_tmp_zoneinfo_nested=""
	])dnl ax_cv_tzdir

	ZONEINFO_UTC_RIGHT="${ax_cv_zoneinfo_utc_right}"
	AC_SUBST([ZONEINFO_UTC_RIGHT])
	AC_SUBST([TZDIR_RIGHT])

	if test -n "${ax_cv_zoneinfo_utc_right}"; then
		TZDIR_RIGHT="`dirname ${ax_cv_zoneinfo_utc_right}`"

		AC_DEFINE([HAVE_ZONEINFO_RIGHT], [1], [dnl
Define when zoneinfo directory has been present during configuration.])
		AC_DEFINE_UNQUOTED([TZDIR_RIGHT],
			["${TZDIR_RIGHT}"], [
Configuration time zoneinfo directory.])
		AC_DEFINE_UNQUOTED([ZONEINFO_UTC_RIGHT],
			["${ax_cv_zoneinfo_utc_right}"], [
Leap-second aware UTC zoneinfo file.])
	fi
])dnl AX_ZONEINFO_RIGHT

AC_DEFUN([AX_ZONEINFO], [
	AC_REQUIRE([AX_ZONEINFO_TZDIR])

	ifelse([$1], [right], [
		AC_REQUIRE([AX_ZONEINFO_RIGHT])
	])

	AC_ARG_VAR([TZDIR], [Directory with compiled zoneinfo files.])
])dnl AX_ZONEINFO

dnl ax_zoneinfo.m4 ends here
