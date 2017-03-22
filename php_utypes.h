/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_UTYPES_H
#define PHP_UTYPES_H

extern zend_module_entry utypes_module_entry;
#define phpext_utypes_ptr &utypes_module_entry

#define PHP_UTYPES_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_UTYPES_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_UTYPES_API __attribute__ ((visibility("default")))
#else
#	define PHP_UTYPES_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(utypes)
	zval callback;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zend_bool busy;
ZEND_END_MODULE_GLOBALS(utypes)

#define UTG(v) ZEND_MODULE_GLOBALS_ACCESSOR(utypes, v)

#if defined(ZTS) && defined(COMPILE_DL_UTYPES)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif	/* PHP_UTYPES_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
