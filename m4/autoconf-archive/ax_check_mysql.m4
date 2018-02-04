# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_check_mysql.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_MYSQL([MYSQL-PLUGIN-NEEDED],[MYSQL-REQUIRED],[MINIMUM-VERSION],[INCLUDES-REQUIRED])
#   AX_SOURCE_MYSQL()
#   AX_CHECK_MYSQL_INSTALL([ROOT-DIR],[IF-FOUND],[IF-NOT-FOUND])
#
# DESCRIPTION
#
#   Looks for a MySQL installation in typical locations, or can take in a
#   flag designating where a MySQL installation is found. Sets the variables
#   stated to various attributes of the desired MySQL installation.
#
#   In detail, AX_CHECK_MYSQL will automatically look for a MySQL
#   installation in the directories that a mysql source or binary install
#   typically install to. AX_CHECK_MYSQL will throw an error if it cannot
#   find one, and it is required.
#
#   AX_CHECK_MYSQL can also check for specific variables passed regarding a
#   location of a MySQL installation.
#
#   If a MySQL installation is found, AX_CHECK_MYSQL sets variables
#   regarding the version of MySQL, its architecture (32 or 64 bit), and
#   wether the version supports Plugins.
#
#   AX_CHECK_MYSQL_INSTALL will check a designated root directory for a
#   command, plugin, and include directory. If a mysql binary is not found
#   or not found, the IF-FOUND and IF-NOT-FOUND directive will be executed,
#   respectively.
#
#   AX_CHECK_MYSQL adds the following flags:
#
#     --with-mysql, for the root of a desired MySQL installation
#     --with-mysql-plugin, for the path to the plugin directory of the MySQL installation
#     --with-mysql-include, for the path to the include directory of the MySQL installation
#     --with-mysql-command, for the path to the binary directory of the MySQL installation
#     --with-mysql-source, for the path to a directory containing the source of the MySQL installation
#
#   AX_CHECK_MYSQL sets:
#
#     MYSQL to indicate whether MySQL was found or not
#     MYSQL_INCLUDES to the include directory (if one exists)
#     MYSQL_PLUGINS  to the plugin directory
#     MYSQL_COMMANDS  to the mysql executable directory
#     MYSQL_ARCHITECTURE to whether MySQL is 32 or 64 bit (32 if 32, 64 if 64)
#     MYSQL_VERSION to what the MySQL version is (5.1,5.5, etc)
#     MYSQL_PLUGIN_OK to whether MySQL version supports plugins (5.1 or greater)
#     MYSQL_55 to whether the version of MySQL is 5.5 or greater
#     MYSQL_SOURCE  to the source directory passed by --with-mysql-source
#
# LICENSE
#
#   Copyright (c) 2011 University of Washington
#   Copyright (c) 2011 Yusuke Tsutsumi <tsutsumi.yusuke@gmail.com>
#   Copyright (c) 2011 Craig Stimmel <cstimmel@uw.edu>
#   Copyright (c) 2011 Eric Wu
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 6

AC_ARG_WITH(mysql,AS_HELP_STRING([--with-mysql],[root of the MySQL installation]))
AC_ARG_WITH(mysql_plugin,AS_HELP_STRING([--with-mysql-plugin],[path to the MySQL installation plugin directory]))
AC_ARG_WITH(mysql_include,AS_HELP_STRING([--with-mysql-include],[path to the MySQL installation include directory]))
AC_ARG_WITH(mysql_command,AS_HELP_STRING([--with-mysql-command],[path to the MySQL executables directory]))
AC_ARG_WITH(mysql_source,AS_HELP_STRING([--with-mysql-source],[path to MySQL source files]))

# Used to look for MySQL installation specifically. Checks if binary exists.
AC_DEFUN([AX_CHECK_MYSQL_COMMANDS],[
    # Define variables passed
    COMMAND_DIR="$1"

    # Check for the binary, and set appropriate variables
    unset ac_cv_mysql_bin_test
    AC_CHECK_PROG(mysql_bin_test,mysql,$COMMAND_DIR,no,$COMMAND_DIR)
    if test "$mysql_bin_test" != "no"; then
        AC_SUBST(MYSQL_COMMANDS,$mysql_bin_test)
        AC_SUBST(MYSQL,yes)
    else
        AC_SUBST(MYSQL_COMMANDS,no)
        AC_SUBST(MYSQL,no)
    fi
])

# Use to look for the plugins directory
AC_DEFUN([AX_CHECK_MYSQL_PLUGINS],[
    # Define variables passed
    PLUGIN_DIR="$1"

    AC_MSG_CHECKING([if $PLUGIN_DIR exists...])
    if [[ -d "$PLUGIN_DIR" ]]; then
       AC_SUBST(MYSQL_PLUGIN,yes)
       AC_MSG_RESULT([yes])
    else
       AC_SUBST(MYSQL_PLUGIN,no)
       AC_MSG_RESULT([no])
    fi
])

# Use to look if includes are installed (determined by the existence of mysql_version.h)
AC_DEFUN([AX_CHECK_MYSQL_INCLUDES],[
    # Define variables passed
    INCLUDE_DIR="$1"
    AC_CHECK_HEADER($INCLUDE_DIR/mysql_version.h,
        AC_SUBST(MYSQL_INCLUDES,$INCLUDE_DIR/),
        AC_SUBST(MYSQL_INCLUDES,no))
])

AC_DEFUN([AX_CHECK_MYSQL_INSTALL],[

    #Define variables passed
    ROOT_DIR="$1"

    # Check for include directory
    AX_CHECK_MYSQL_INCLUDES([$ROOT_DIR/include/mysql/mysql_version.h])
    if test "$MYSQL_INCLUDES" == "no" ; then
       AX_CHECK_MYSQL_INCLUDES([$ROOT_DIR/include/mysql_version.h])
    fi
    mysql_include_test=$MYSQL_INCLUDES

    # Check for plugin directory
    AX_CHECK_MYSQL_PLUGINS([$ROOT_DIR/lib/mysql/plugin/])
    if test "$MYSQL_PLUGIN" == "no" ; then
       unset $MYSQL_PLUGIN
        AX_CHECK_MYSQL_PLUGINS([$ROOT_DIR/lib/plugin/])
    fi
    mysql_plugin_test=$MYSQL_PLUGIN

    # Check for binary directory
    AX_CHECK_MYSQL_COMMANDS([$ROOT_DIR/bin/])

    if test "$MYSQL" != "no"
    then
        true
        $2
    else
        true
        $3
    fi
])

AC_DEFUN([AX_CHECK_MYSQL],[
    mysql_test="no"

    # Define variables
    MYSQL_PLUGIN_NEEDED=`echo $1 | grep -i -o "y"`
    MYSQL_REQUIRED=`echo $2 | grep -i -o "y"`
    MINIMUM_V="$3"
    INCLUDES_REQUIRED=`echo $4 | grep -i -o "y"`
    CLASSIFIER="none"
    mysql_issue=""



    # Checks for common installation locations of MySQL

    echo "Testing if MySQL was installed to common source/binary directory"
    AC_CHECK_PROG(mysqlsource,mysql,yes,no,/usr/local/mysql/bin,)
    echo "Testing if MySQL was installed to common package manager directory"
    AC_CHECK_PROG(mysqlpackage,mysql,yes,no,/usr/bin,)

    # Checks whether the directories contains what they're supposed to, then produces an error otherwise.
    # In addition, will also generate an error if no installations exist, or two installations are detected.

    if test "$ac_cv_prog_mysqlsource" == "yes" && test "$ac_cv_prog_mysqlpackage" == "yes"
    then
        mysql_issue="Multiple MySQL installations found. Please specify the MySQL installation directory with --with-mysql"
    else if test "$ac_cv_prog_mysqlsource" == "yes"
    then
        AX_CHECK_MYSQL_INSTALL(/usr/local/mysql,,)
        CLASSIFIER="source"
    else if test "$ac_cv_prog_mysqlpackage" == "yes"
    then
        AX_CHECK_MYSQL_INSTALL(/usr,,)
        CLASSIFIER="package"
    else
        mysql_issue="No default MySQL installs detected. Please specify the MySQL installation directory with --with-mysql"
    fi
    fi
    fi

    # Checks if --with-mysql flag was passed. If so, verifies that the directory follows assumed
    # structure and include,plugin, and bin directory is found. If there are no issues, this
    # will nullify any errors that would have been thrown by the above checking.
    if test "$with_mysql" != ""
    then
        AX_CHECK_MYSQL_INSTALL($with_mysql,,)
        CLASSIFIER="root"
        mysql_issue=""
    fi


    # Checks if specific MySQL directory flags were passed (--with-mysql-plugin, --with-mysql-include, --with-mysql-bin)
    # If so then checks if these variables are proper directories. If not, returns an error. Requires that all three directories must be defined.

    if test "$with_mysql_plugin" != "" || test "$with_mysql_include" != "" || test "$with_mysql_command" != ""
    then
        mysql_test="yes"
        if test "$with_mysql_plugin" == "" || test "$with_mysql_command" == ""
        then
            mysql_test="no"
            if test "$MYSQL_REQUIRED" != ""
            then
                AC_MSG_ERROR([Argument is missing! When using --with-mysql-plugin --with-mysql-bin please enter arguments for each.])
            else
                AC_MSG_WARN([Argument is missing! When using --with-mysql-plugin --with-mysql-bin please enter arguments for each.])
            fi
        else
        mysql_issue=""
        AX_CHECK_MYSQL_PLUGINS([$with_mysql_plugin])
        AX_CHECK_MYSQL_INCLUDES([$with_mysql_include])
        AX_CHECK_MYSQL_COMMANDS([$with_mysql_command])
        CLASSIFIER="custom"
        fi
    fi
    # If the installation does not exist or satisfy requirements, send an error or warning
    mysql_dne_message=""
    mysql_include_dne_message=""

    if test "$mysql_issue" != ""
    then
        if test "$MYSQL_REQUIRED" != ""
        then
            AC_MSG_ERROR([$mysql_issue])
        else
            AC_MSG_WARN([$mysql_issue])
        fi
    fi
    # Error message for not finding mysql executable
    if test "$MYSQL_BIN" == "no"
    then
        if test "$CLASSIFIER" == "root"
        then
            mysql_dne_message="Could not find directory containing MySQL includes. Please designate the command\,plugin\,and include directories manually with --with-mysql-command\, --with-mysql-plugin\, and --with-mysql-include"
        fi
        if test "$CLASSIFIER" == "custom"
        then
            mysql_dne_message="Could not find mysql executable in designated command directory. Please pass the directory containing the mysql executable with --with-mysql-command"
        fi
    fi

    # Error message for not finding mysql plugin directory
    if test "$MYSQL_PLUGINS" == "no"
    then
        if test "$CLASSIFIER" == "source"
        then
            AC_MSG_WARN([Could not find plugin directory for detected source installation. Please pass the root directory of the MySQL installation with --with-mysql])
        fi
        if test "$CLASSIFIER" == "package"
        then
            AC_MSG_WARN([Could not find plugin directory for detected package installation. Please pass the root directory of the MySQL installation with --with-mysql])
        fi
        if test "$CLASSIFIER" == "root"
        then
            AC_MSG_WARN([Could not find directory for MySQL plugins. Please designate the command\,plugin\,and include directories manually with --with-mysql-command\, --with-mysql-plugin\, and --with-mysql-include])
        fi
        if test "$CLASSIFIER" == "custom"
        then
            AC_MSG_WARN([Could not find mysql includes in designated plugin directory. Please pass the directory containing the mysql executable with --with-mysql-plugin])
        fi
    fi

    # Error message for not finding mysql include directory
   if test "$MYSQL_INCLUDES" == "no"
    then
        if test "$CLASSIFIER" == "source"
        then
            mysql_include_dne_message="A source install was detected, but the include directory could not be found! MySQL development library may not be installed. If development library is installed please use --with-mysql-include --with-mysql-plugin --with-mysql-command to manually assign directory locations"
        fi
        if test "$CLASSIFIER" == "package"
        then
            mysql_include_dne_message="A package install was detected, but the include directory could not be found! MySQL development library may not be installed. If development library is installed please use --with-mysql-include --with-mysql-plugin --with-mysql-command to manually assign directory locations"
        fi
        if test "$CLASSIFIER" == "root"
        then
            mysql_include_dne_message="Could not find directory containing MySQL includes. The MySQL development library may not be installed. If development library is installed\, please designate the command\,plugin\,and include directories manually with --with-mysql-command\, --with-mysql-plugin\, and --with-mysql-include"
        fi
        if test "$CLASSIFIER" == "custom"
        then
            mysql_include_dne_message="Could not find mysql includes in designated include directory. Please pass the directory containing the mysql_version.h include file with --with-mysql-include"
        fi
    fi

    # And execute the error messages
    if test "$mysql_dne_message" != ""
    then
         if test "$MYSQL_REQUIRED" != ""
         then
            AC_MSG_ERROR([$mysql_dne_message])
         else
            AC_MSG_WARN([$mysql_dne_message])
         fi
    fi
    if test "$mysql_include_dne_message" != ""
    then
         if test "$INCLUDES_REQUIRED" != ""
         then
            AC_MSG_ERROR([$mysql_include_dne_message])
         else
            AC_MSG_WARN([$mysql_include_dne_message])
         fi
    fi

    if test "$MYSQL" == "yes"
    then
        # Check MySQL version, wether it's 32 or 64 bit, and modifies the architecture variable accordingly
        AC_MSG_CHECKING([MySQL Architecture])
        MYSQL_ARCHITECTURE='file '$MYSQL_COMMANDS'/mysql'
        MYSQL_ARCHITECTURE=`$MYSQL_ARCHITECTURE | grep -o ".*bit" | sed s/-bit//g | grep -o "[[0-9]][[0-9]]$"`
        AC_MSG_RESULT([$MYSQL_ARCHITECTURE])
        AC_SUBST(MYSQL_ARCHITECTURE,$MYSQL_ARCHITECTURE)

        # Checks MySQL binary version
        AC_MSG_CHECKING([MySQL Version])
        MYSQL_PREFIX=$MYSQL_COMMANDS'/mysqladmin -v'
        MYSQL_V=`$MYSQL_PREFIX | grep -o 'Distrib.*,' | sed s/Distrib\ //g | sed s/,//g`
        AC_MSG_RESULT([$MYSQL_V])

        # Checks whether MySQL version is greater than 5.1, the version needed for plugins
        AC_MSG_CHECKING([if MySQL install supports Plugins])
        MYSQL_MAJOR_V=`echo $MYSQL_V | cut -c 1`
        MYSQL_MINOR_V=`echo $MYSQL_V | cut -c 3`
        MYSQL_REV_V=`echo $MYSQL_V | cut -c 5-6`
        MYSQL_PLUGIN_MINOR_V=1
        MYSQL_PLUGIN_MAJOR_V=5
        if test "$MYSQL_MAJOR_V" -lt "$MYSQL_PLUGIN_MAJOR_V" || (test "$MYSQL_MAJOR_V" -eq "$MYSQL_PLUGIN_MAJOR_V" && test "$MYSQL_MINOR_V" -lt "$MYSQL_PLUGIN_MINOR_V")
        then
            AC_SUBST(MYSQL_PLUGIN_OK,no)
            AC_MSG_RESULT([no])
        else
            AC_SUBST(MYSQL_PLUGIN_OK,yes)
            AC_MSG_RESULT([yes])
        fi

        if test "$MYSQL_PLUGIN_NEEDED" != ""
        then
            if test "$MYSQL_PLUGIN_OK" == "no"
            then
                AC_MSG_ERROR([MySQL version is not able to support plugins! Please upgrade your version of MySQL before installing])
            fi
        fi

        # Checks wether MINIMUM-VERSION was passed, does error checking for the value, and checks for version
        if test "$MINIMUM_V" != ""
        then
            MINIMUM_MAJOR_V=`echo $MINIMUM_V | cut -c 1`
            MINIMUM_MINOR_V=`echo $MINIMUM_V | cut -c 3`
            MINIMUM_REV_V=`echo $MINIMUM_V | cut -c 5-6`
            CHECKER_MAJOR=`echo $MINIMUM_MAJOR_V | grep -o '[[0-9]]'`
            CHECKER_MINOR=`echo $MINIMUM_MINOR_V | grep -o '[[0-9]]'`
            CHECKER_REV=`echo $MINIMUM_REV_V | grep -o  '^[[0-9]]+'`
            if test "$CHECKER_MAJOR" != "" && test "$CHECKER_MINOR" != "" && test "$CHECKER_REV" == ""
            then
                    AC_MSG_CHECKING([if MySQL version is equal or greater than $MINIMUM_V])
                    if test "$MYSQL_MAJOR_V" -lt "$MINIMUM_MAJOR_V" || (test "$MYSQL_MAJOR_V" -eq "$MINIMUM_MAJOR_V" && test "$MYSQL_MINOR_V" -lt "$MINIMUM_MINOR_V") || (test "$MYSQL_MAJOR_V" -eq "$MINIMUM_MAJOR_V" && test "$MYSQL_MINOR_V" -eq "$MINIMUM_MINOR_V" && "$MYSQL_REV_V" -lt "MINIMUM_REV_V")
                    then
                        AC_SUBST(MYSQL_PLUGIN_OK,no)
                        AC_MSG_RESULT([no])
                        AC_MSG_ERROR([installed MySQL version is not above $MINIMUM_V. Please upgrade your version of MySQL])
                    else
                        AC_SUBST(MYSQL_PLUGIN_OK,yes)
                        AC_MSG_RESULT([yes])
                    fi
            else
                    AC_MSG_ERROR([MINIMUM-VERSION variable in AX_CHEC_MYSQL is not formatted properly. Please use X.X or X.X.XX])
            fi
        fi

        # Checks whether MySQL version is 5.5 or greater, the production release with major header/include changes from before
        if test "$MYSQL_MAJOR_V" -gt 4 && test "$MYSQL_MINOR_V" -gt 4
        then
            AC_SUBST(MYSQL_55,yes)
        else
            AC_SUBST(MYSQL_55,no)
        fi
    fi

])

AC_DEFUN([AX_SOURCE_MYSQL],[
    if test "$with_mysql_source" == ""
    then
       AC_MSG_ERROR(["Please Designate MySQL source path, using --with-mysql-source=YOUR_PATH"])
    else
       AC_SUBST(MYSQL_SOURCE,$with_mysql_source)
    fi
])
