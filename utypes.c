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
  | Author: krakjoe                                                      |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_utypes.h"

#if PHP_VERSION_ID < 70200
#define ZEND_TYPE_IS_CLASS(i)    ((i).type_hint == IS_OBJECT)
#define ZEND_TYPE_NAME(i)        ((i).class_name)
#define PHP_UTYPE(t)             (t)
#elif PHP_VERSION_ID >= 70200
#define PHP_UTYPE(t)             (t).type
#endif

ZEND_DECLARE_MODULE_GLOBALS(utypes);

/* {{{ proto void utypes(callable handler) */
PHP_FUNCTION(utypes)
{
	zval *callback;
	zend_fcall_info fci = empty_fcall_info;
	zend_fcall_info_cache fcc = empty_fcall_info_cache;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &callback) != SUCCESS) {
		return;
	}

	if (zend_fcall_info_init(callback, IS_CALLABLE_CHECK_SILENT, &fci, &fcc, NULL, NULL) != SUCCESS) {
		return;
	}

	if (Z_TYPE(UTG(callback))) {
		zval_ptr_dtor(&UTG(callback));
	}

	memcpy(&UTG(fci), &fci, sizeof(zend_fcall_info));
	memcpy(&UTG(fcc), &fcc, sizeof(zend_fcall_info_cache));

	ZVAL_COPY(&UTG(callback), callback);
}
/* }}} */

/* {{{ php_utypes_init_globals
 */
static void php_utypes_init_globals(zend_utypes_globals *utg)
{
	memset(utg, 0, sizeof(*utg));
}
/* }}} */

/* {{{ */
static inline int php_utypes_args(zend_execute_data *execute_data, zend_function *func, const zend_op *opline, zend_fcall_info *fci) {
	zval name;

	ZVAL_UNDEF(&name);

	switch (opline->opcode) {
		case ZEND_RECV:	
		case ZEND_RECV_INIT:
			if (!ZEND_TYPE_IS_CLASS(PHP_UTYPE(func->common.arg_info[opline->op1.num-1]))) {
				return FAILURE;
			}

			ZVAL_STR(&name, ZEND_TYPE_NAME(PHP_UTYPE(func->common.arg_info[opline->op1.num-1])));

			zend_fcall_info_argn(fci, 2, &name, ZEND_CALL_ARG(execute_data, opline->op1.num));

			return SUCCESS;
		break;

		case ZEND_RECV_VARIADIC: {
			zval params, *variadic;
			uint32_t n = opline->op1.num, c = ZEND_CALL_NUM_ARGS(execute_data);

			if (!ZEND_TYPE_IS_CLASS(PHP_UTYPE(func->common.arg_info[opline->op1.num-1]))) {
				return FAILURE;
			}

			ZVAL_STR(&name, ZEND_TYPE_NAME(PHP_UTYPE(func->common.arg_info[opline->op1.num-1])));

			array_init(&params);

			if (n <= c) {
				variadic = ZEND_CALL_VAR_NUM(execute_data, func->op_array.last_var + func->op_array.T);

				do {
					add_next_index_zval(&params, variadic);
					Z_TRY_ADDREF_P(variadic);
					variadic++;
				} while(++n <= c);
			}

			zend_fcall_info_argn(fci, 2, &name, &params);

			Z_SET_REFCOUNT_P(&params, 1);

			return SUCCESS;
		} break;

		case ZEND_VERIFY_RETURN_TYPE: {
			zval nil, *value = &nil;

			if (!ZEND_TYPE_IS_CLASS(PHP_UTYPE(func->common.arg_info[-1]))) {
				return FAILURE;
			}

			ZVAL_STR(&name, ZEND_TYPE_NAME(PHP_UTYPE(func->common.arg_info[-1])));

			if (opline->op1_type == IS_CONST) {
				value = EX_CONSTANT(opline->op1);
			} else {
				value = ZEND_CALL_VAR(execute_data, opline->op1.num);

				if (Z_TYPE_P(value) == IS_INDIRECT) {
					value = Z_INDIRECT_P(value);
				}

				if (Z_ISUNDEF_P(value)) {
					ZVAL_NULL(&nil);
				}

				ZVAL_DEREF(value);
			}

			zend_fcall_info_argn(fci, 2, &name, value);

			return SUCCESS;
		} break;
	}

	return FAILURE;
} /* }}} */

/* {{{ */
static inline zend_bool php_utypes_hints(zend_execute_data *execute_data) {
	switch (EX(opline)->opcode) {
		case ZEND_RECV:
		case ZEND_RECV_INIT:
		case ZEND_RECV_VARIADIC:
			return (EX(func)->common.fn_flags & ZEND_ACC_HAS_TYPE_HINTS) == ZEND_ACC_HAS_TYPE_HINTS;

		case ZEND_VERIFY_RETURN_TYPE:
			return (EX(func)->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) == ZEND_ACC_HAS_RETURN_TYPE;
	}

	return 0;
} /* }}} */

/* {{{ */
static inline zend_bool php_utypes_ready() {
	return !UTG(busy) || Z_ISUNDEF(UTG(callback));
} /* }}} */

/* {{{ */
static inline void php_utypes_enter(zval *rv) {
	UTG(busy) = 1;
	
	UTG(fci).retval = rv;

	ZVAL_UNDEF(UTG(fci).retval);
} /* }}} */

/* {{{ */
static inline int php_utypes_leave(int value) {
	UTG(busy) = 0;

	if (UTG(fci).param_count) {
		zend_fcall_info_args_clear(&UTG(fci), 1);
	}

	if (!Z_ISUNDEF_P(UTG(fci).retval)) {
		zval_ptr_dtor(UTG(fci).retval);

		ZVAL_UNDEF(UTG(fci).retval);
	}

	return value;
} /* }}} */

/* {{{ */
static int php_utypes_handler(zend_execute_data *execute_data) {
	zval rv;

	if (!php_utypes_ready() || !php_utypes_hints(execute_data)) {
		return ZEND_USER_OPCODE_DISPATCH;
	}

	if (EX(func)->type != ZEND_USER_FUNCTION) {
		return ZEND_USER_OPCODE_DISPATCH;
	}

	php_utypes_enter(&rv);

	if (php_utypes_args(execute_data, EX(func), EX(opline), &UTG(fci)) != SUCCESS) {
		return php_utypes_leave(ZEND_USER_OPCODE_DISPATCH);
	}

	if (zend_call_function(&UTG(fci), &UTG(fcc)) != SUCCESS) {
		return php_utypes_leave(ZEND_USER_OPCODE_DISPATCH);
	}

	if (zend_is_true(&rv)) {
		EX(opline)++;

		return php_utypes_leave(ZEND_USER_OPCODE_CONTINUE);
	}

	return php_utypes_leave(ZEND_USER_OPCODE_DISPATCH);
} /* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(utypes)
{
	ZEND_INIT_MODULE_GLOBALS(utypes, php_utypes_init_globals, NULL);

	zend_set_user_opcode_handler(ZEND_RECV, php_utypes_handler);
	zend_set_user_opcode_handler(ZEND_RECV_INIT, php_utypes_handler);
	zend_set_user_opcode_handler(ZEND_RECV_VARIADIC, php_utypes_handler);
	zend_set_user_opcode_handler(ZEND_VERIFY_RETURN_TYPE, php_utypes_handler);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(utypes)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(utypes)
{
#if defined(COMPILE_DL_UTYPES) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	ZVAL_UNDEF(&UTG(callback));

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(utypes)
{
	if (Z_TYPE(UTG(callback))) {
		zval_dtor(&UTG(callback));
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(utypes)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "utypes support", "enabled");
	php_info_print_table_end();
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(php_utypes_arginfo, 0, 0, 1)
	ZEND_ARG_CALLABLE_INFO(0, callback, 0)
ZEND_END_ARG_INFO()

/* {{{ utypes_functions[]
 */
const zend_function_entry utypes_functions[] = {
	PHP_FE(utypes,	php_utypes_arginfo)
	PHP_FE_END
};
/* }}} */

/* {{{ utypes_module_entry
 */
zend_module_entry utypes_module_entry = {
	STANDARD_MODULE_HEADER,
	"utypes",
	utypes_functions,
	PHP_MINIT(utypes),
	PHP_MSHUTDOWN(utypes),
	PHP_RINIT(utypes),
	PHP_RSHUTDOWN(utypes),
	PHP_MINFO(utypes),
	PHP_UTYPES_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_UTYPES
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(utypes)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
