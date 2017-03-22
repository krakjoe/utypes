dnl $Id$
dnl config.m4 for extension utypes

PHP_ARG_ENABLE(utypes, whether to enable utypes support,
[  --enable-utypes           Enable utypes support])

if test "$PHP_UTYPES" != "no"; then
  PHP_NEW_EXTENSION(utypes, utypes.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
