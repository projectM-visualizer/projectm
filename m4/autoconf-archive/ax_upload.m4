# ===========================================================================
#        https://www.gnu.org/software/autoconf-archive/ax_upload.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_UPLOAD([command])
#
# DESCRIPTION
#
#   Adds support for uploading dist files. %%s in the command will be
#   substituted with the name of the file. e.g:
#
#     AX_UPLOAD([ncftpput -v upload.sourceforge.net /incoming %%s])
#
#   To add upload support for other custom dists add upload-<TYPE> to
#   UPLOAD_BIN or UPLOAD_SRC, where <TYPE> is the type of dist that is being
#   uploaded and add a mapping from <TYPE> to the dist file name in the
#   format '{<TYPE>=><FILENAME>}' to UPLOAD_TARGETS. For example:
#
#     UPLOAD_BIN += upload-foobar
#     UPLOAD_TARGETS += {foobar=>@PACKAGE@-@VERSION@.fb}
#
#   You can then upload of the src distribution files by running:
#
#     make upload-src
#
#   all the binary distribution files by running:
#
#     make upload-bin
#
#   or both by running:
#
#     make upload
#
# LICENSE
#
#   Copyright (c) 2009 Tom Howard <tomhoward@users.sf.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 13

AC_DEFUN([AX_UPLOAD],
[
AC_MSG_NOTICE([adding upload support])
AM_CONDITIONAL(USING_AX_UPLOAD, [true])
AC_MSG_NOTICE([setting upload command... \`$1\`])
AX_ADD_AM_MACRO_STATIC([
UPLOAD_BIN =
UPLOAD_SRC = upload-gzip upload-bzip2 upload-zip
UPLOAD_TARGETS = \\
{gzip=>$PACKAGE-$VERSION.tar.gz} \\
{bzip2=>$PACKAGE-$VERSION.tar.bz2} \\
{zip=>$PACKAGE-$VERSION.zip}

\$(UPLOAD_BIN) \$(UPLOAD_SRC):
	@TYPE=\`echo ${AX_DOLLAR}@ | \$(SED) -e \'s/upload-//\'\`; \\
	DIST=\"dist-\$\${TYPE}\"; \\
	\$(MAKE) \$(AM_MAKEFLAGS) \$\${DIST}; \\
	list=\'\$(UPLOAD_TARGETS)\'; \\
	pattern=\`echo \"^{\$\${TYPE}=>\"\`; \\
	for dist in \$\$list; do \\
		echo \$\$dist | \$(EGREP) \"^{\$\${TYPE}=>\" > /dev/null 2>&1; \\
		if test \"\$\$?\" -eq \"0\"; then \\
			TARGET=\`echo \"\$\$dist\" | \$(AWK) -v pattern=\$\$pattern \'{ sub( pattern, \"\"); sub( /}\$\$/, \"\" ); print; }\'\`; \\
			UPLOAD_COMMAND=\`printf \"$1\" \$\$TARGET \`; \\
			echo \"Uploading \$\$TARGET ...\"; \\
			\$\$UPLOAD_COMMAND; \\
		fi \\
	done

upload-src: \$(UPLOAD_SRC)

upload-bin: \$(UPLOAD_BIN)

upload upload-all all-upload: upload-src upload-bin
])
])
