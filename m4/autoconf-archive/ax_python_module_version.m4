# =============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_python_module_version.html
# =============================================================================
#
# SYNOPSIS
#
#   AX_PYTHON_MODULE_VERSION(modname, min_version[, python])
#
# DESCRIPTION
#
#   Checks for Python module with at least the given version.
#
#   Triggers an error if module is absent or present but at a lower version.
#   The third parameter can either be "python" for Python 2 or "python3" for
#   Python 3; defaults to Python 3.
#
# LICENSE
#
#   Copyright (c) 2015 Endless Mobile, Inc.; contributed by Philip Chimento <philip@endlessm.com> and Kurt von Laven <kurt@endlessm.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 3

AC_DEFUN([AX_PYTHON_MODULE_VERSION], [
    AX_PYTHON_MODULE([$1], [required], [$3])
    AC_MSG_CHECKING([for version $2 or higher of $1])
    $PYTHON -c "import sys, $1; from distutils.version import StrictVersion; sys.exit(StrictVersion($1.__version__) < StrictVersion('$2'))" 2> /dev/null
    AS_IF([test $? -eq 0], [], [
        AC_MSG_RESULT([no])
        AC_MSG_ERROR([You need at least version $2 of the $1 Python module.])
    ])
    AC_MSG_RESULT([yes])
])
