# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_lib_crypto.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_CRYPTO([yes|no|auto])
#
# DESCRIPTION
#
#   Searches for the 'crypto' library with the --with... option.
#
#   If found, define HAVE_CRYPTO and macro CRYPTO_LIBS. Also defines
#   CRYPTO_WITH_<algo> for the algorithms found available. Possible
#   algorithms: AES BF CAMELLIA CAST DES IDEA RC2 RC5 MD2 MD4 MD5 SHA RIPEMD
#   RSA DSA DH
#
#   The argument is used if no --with...-crypto option is set. Value "yes"
#   requires the configuration by default. Value "no" does not require it by
#   default. Value "auto" configures the library only if available.
#
#   See also AX_LIB_BEECRYPT and AX_LIB_GCRYPT.
#
# LICENSE
#
#   Copyright (c) 2009 Fabien Coelho <autoconf.archive@coelho.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 10

# AX_CHECK_CRYPTO_LIB([algo-name],[function])
AC_DEFUN([AX_CHECK_CRYPTO_LIB],[
  AC_CHECK_LIB([crypto], $2, [
    AC_DEFINE([CRYPTO_WITH_$1],[1],[Algorithm $1 in openssl crypto library])
  ])
])

# AX_LIB_CRYPTO([yes|no|auto])
AC_DEFUN([AX_LIB_CRYPTO],[
  AC_MSG_CHECKING([whether openssl crypto is enabled])
  AC_ARG_WITH([crypto],[  --with-crypto           requite crypto library
  --without-crypto        disable crypto library],[
    AC_MSG_RESULT([$withval])
    ac_with_crypto=$withval
  ],[
    AC_MSG_RESULT([$1])
    ac_with_crypto=$1
  ])
  if test "$ac_with_crypto" = "yes" -o "$ac_with_crypto" = "auto" ; then
    AC_CHECK_HEADERS([openssl/opensslconf.h],[
      AC_CHECK_LIB([crypto], [CRYPTO_lock],[
        AC_DEFINE([HAVE_CRYPTO],[1],[Openssl crypto library is available])
	HAVE_CRYPTO=1
	AC_SUBST([CRYPTO_LIBS],[-lcrypto])
	# ciphers
        AX_CHECK_CRYPTO_LIB([AES],[AES_ecb_encrypt])
        AX_CHECK_CRYPTO_LIB([BF],[BF_ecb_encrypt])
        AX_CHECK_CRYPTO_LIB([CAST],[CAST_ecb_encrypt])
        AX_CHECK_CRYPTO_LIB([CAMELLIA],[Camellia_ecb_encrypt])
        AX_CHECK_CRYPTO_LIB([DES],[DES_ecb_encrypt])
        AX_CHECK_CRYPTO_LIB([IDEA],[idea_ecb_encrypt])
        AX_CHECK_CRYPTO_LIB([RC2],[RC2_ecb_encrypt])
        AX_CHECK_CRYPTO_LIB([RC5],[RC5_32_ecb_encrypt])
	# digests
        AX_CHECK_CRYPTO_LIB([MD2],[MD2])
        AX_CHECK_CRYPTO_LIB([MD4],[MD4])
        AX_CHECK_CRYPTO_LIB([MD5],[MD5])
        AX_CHECK_CRYPTO_LIB([RIPEMD],[RIPEMD160])
        AX_CHECK_CRYPTO_LIB([SHA],[SHA1])
	# others
	AX_CHECK_CRYPTO_LIB([RSA],[RSA_set_method])
	AX_CHECK_CRYPTO_LIB([DSA],[DSA_set_method])
	AX_CHECK_CRYPTO_LIB([DH],[DH_set_method])
      ])
    ])
    # complain only if crypto as *explicitly* required
    if test "$ac_with_crypto" = "yes" -a "x$HAVE_CRYPTO" = "x" ; then
      AC_MSG_ERROR([cannot configure required openssl crypto library])
    fi
  fi
])
