/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                       |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Homme Zwaagstra (hrz@geodata.soton.ac.uk)                    |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_version.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php-clearsilver.h"

#include "ClearSilver.h"

#if (PHP_MAJOR_VERSION >=7)
#	if (PHP_MINOR_VERSION >=4)
#		define PHP_CS_74
#	endif
#else
#	if (PHP_MAJOR_VERSION >=5)
#		if (PHP_MINOR_VERSION >=4)
#			define PHP_CS_54
#		endif
#	endif
#endif

#ifdef PHP_CS_74
#define zend_rsrc_list_entry zend_resource
#endif

#ifdef PHP_CS_74
#define PHPCS_HDF_PARAM(_hdf,_input) _hdf = (HDF*)zend_fetch_resource(Z_RES_P(_input),"ClearSilver HDF",le_clearsilver_hdf)
#define PHPCS_CS_PARAM(_cs,_input) _cs = (CSPARSE*)zend_fetch_resource(Z_RES_P(_input),"ClearSilver CS",le_clearsilver_cs)
#else
#define PHPCS_HDF_PARAM(_hdf,_input) ZEND_FETCH_RESOURCE(_hdf, HDF *, &_input, -1, "ClearSilver HDF", le_clearsilver_hdf)
#define PHPCS_CS_PARAM(_cs,_input) ZEND_FETCH_RESOURCE(_cs, CSPARSE *, &_input, -1, "ClearSilver CS", le_clearsilver_cs)
#endif

#define my_debug(str...) { FILE* tst = fopen("mytext.txt","a");if (tst) { fprintf(tst, str);fclose(tst); } }

#ifdef PHP_CS_74
#define PHPCS_STRLEN_VAR size_t
#else
#define PHPCS_STRLEN_VAR int
#endif

/*
void my_debug( const char* str ) {
	FILE* tst;
	tst = fopen("mytext.txt","a");
	if (!tst) {
		return;
	}
	fprintf( tst , "%s\n" , str );
	fclose( tst );
}
*/

/***********************************************************************
 * NOTE FOR FUTURE DEVELOPMENT:
 * Check out
 * http://cvs.sourceforge.net/viewcvs.py/swishe/swish_php/swishe.c?rev=1.4
 * for example of simple OO interface
 ***********************************************************************/

/* If you declare any globals in php_clearsilver.h uncomment this:
   ZEND_DECLARE_MODULE_GLOBALS(clearsilver)
*/

/* True global resources - no need for thread safety here */
static int le_clearsilver_hdf;
static void clearsilver_hdf_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
	HDF *hdf = (HDF *) rsrc->ptr;

	/* we only want to destroy the node if it is the root node */
	if(hdf == hdf_obj_top(hdf)) {
		hdf_destroy(&hdf);
	}
}

static int le_clearsilver_cs;
static void clearsilver_cs_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
	CSPARSE *parse = (CSPARSE *) rsrc->ptr;
	cs_destroy(&parse);
}

static NEOERR *render_cb (void *ctx, char *buf) {
	STRING *str = (STRING *) ctx;

	return nerr_pass(string_append(str, buf));
}

char* neo_error_to_string (NEOERR *err) {
	STRING str;
	char * err_str = NULL;

	string_init (&str);

	if (nerr_match(err, NERR_PARSE)) {
		nerr_error_string (err, &str);
	} else {
		nerr_error_traceback (err, &str);
	}

	err_str = estrdup(str.buf);

	string_clear (&str);

	return err_str;
}


/* {{{ clearsilver_functions[]
 *
 * Every user visible function must have an entry in clearsilver_functions[].
 */
#if (defined(PHP_CS_54) || defined(PHP_CS_74))
zend_function_entry clearsilver_functions[] = {
#else
function_entry clearsilver_functions[] = {
#endif
	PHP_FE(hdf_init,	NULL)
	PHP_FE(hdf_read_file,	NULL)
	PHP_FE(hdf_write_file,	NULL)
	PHP_FE(hdf_read_string,  NULL)
	PHP_FE(hdf_write_string,  NULL)
	PHP_FE(hdf_set_value,	NULL)
	PHP_FE(hdf_get_value,	NULL)
	PHP_FE(hdf_get_node,	NULL)
	PHP_FE(hdf_set_node,	NULL)
	PHP_FE(hdf_get_obj,	NULL)
	PHP_FE(hdf_obj_name,	NULL)
	PHP_FE(hdf_obj_value,	NULL)
	PHP_FE(hdf_obj_child,	NULL)
	PHP_FE(hdf_obj_next,	NULL)
	PHP_FE(hdf_get_child,	NULL)
	PHP_FE(hdf_remove_tree,	NULL)
	PHP_FE(hdf_destroy,	NULL)
	PHP_FE(cs_init,	NULL)
	PHP_FE(cs_parse_string,	NULL)
	PHP_FE(cs_parse_file,	NULL)
	PHP_FE(cs_render,	NULL)
	PHP_FE(cs_destroy,	NULL)
	{NULL, NULL, NULL}	/* Must be the last line in clearsilver_functions[] */
};
/* }}} */

/* {{{ clearsilver_module_entry
 */
zend_module_entry clearsilver_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"clearsilver",
	clearsilver_functions,
	PHP_MINIT(clearsilver),
	PHP_MSHUTDOWN(clearsilver),
	NULL,	   /* Replace with NULL if there's nothing to do at request start */
	NULL,   /* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(clearsilver),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_CLEARSILVER_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_CLEARSILVER
ZEND_GET_MODULE(clearsilver)
#endif

	 /* {{{ PHP_INI
	  */
	 /* Remove comments and fill if you need to have entries in php.ini
		PHP_INI_BEGIN()
		STD_PHP_INI_ENTRY("clearsilver.global_value",  "42", PHP_INI_ALL, OnUpdateInt, global_value, zend_clearsilver_globals, clearsilver_globals)
		STD_PHP_INI_ENTRY("clearsilver.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_clearsilver_globals, clearsilver_globals)
		PHP_INI_END()
	 */
	 /* }}} */

	 /* {{{ php_clearsilver_init_globals
	  */
	 /* Uncomment this function if you have INI entries
		static void php_clearsilver_init_globals(zend_clearsilver_globals *clearsilver_globals)
		{
		clearsilver_globals->global_value = 0;
		clearsilver_globals->global_string = NULL;
		}
	 */
	 /* }}} */

	 /* {{{ PHP_MINIT_FUNCTION
	  */
	 PHP_MINIT_FUNCTION(clearsilver)
{
	/* If you have INI entries, uncomment these lines
	   ZEND_INIT_MODULE_GLOBALS(clearsilver, php_clearsilver_init_globals, NULL);
	   REGISTER_INI_ENTRIES();
	*/

	REGISTER_STRING_CONSTANT("PHP_CLEARSILVER_VERSION", PHP_CLEARSILVER_VERSION, CONST_CS | CONST_PERSISTENT);
	my_debug( "init\n" );
	le_clearsilver_hdf = zend_register_list_destructors_ex(clearsilver_hdf_dtor,
														   NULL,
														   "ClearSilver HDF",
														   module_number);

	le_clearsilver_cs = zend_register_list_destructors_ex(clearsilver_cs_dtor,
														  NULL,
														  "ClearSilver CS",
														  module_number);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(clearsilver)
{
	my_debug( "shutdown\n" );
	my_debug( "-----------------------------------\n" );
	/* uncomment this line if you have INI entries
	   UNREGISTER_INI_ENTRIES();
	*/

	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(clearsilver)
{
	my_debug( "rinit\n" );
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(clearsilver)
{
	my_debug( "rshutdown\n" );
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(clearsilver)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "ClearSilver support", "enabled");
	php_info_print_table_header(2, "PHP ClearSilver version", PHP_CLEARSILVER_VERSION);
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	   DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ proto resource hdf_init()
   Initialize an HDF data set */
PHP_FUNCTION(hdf_init) {
	HDF *hdf;
	NEOERR *err;

	my_debug( "hdf_init called\n" );
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	err = hdf_init(&hdf);

	if (err != STATUS_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, neo_error_to_string(err));
		RETURN_NULL();
	}
#ifdef PHP_CS_74
	my_debug( "hdf_init register resource %016x with num %d\n" , hdf , le_clearsilver_hdf );
	RETURN_RES(zend_register_resource(hdf,le_clearsilver_hdf));
#else
	ZEND_REGISTER_RESOURCE(return_value, hdf, le_clearsilver_hdf);	// TODO LORI
#endif
}
/* }}} */

/* {{{ proto bool hdf_read_file(resource hdf, string path)
   Read an HDF data file */
PHP_FUNCTION(hdf_read_file) {
	zval *zhdf = NULL;
	int argc = ZEND_NUM_ARGS();
	char *path = NULL;
	PHPCS_STRLEN_VAR path_len = 0;

	HDF *hdf = NULL;
	NEOERR *err = NULL;

	my_debug( "hdf_read_file started with args %d\n" , argc );
	if (zend_parse_parameters(argc TSRMLS_CC, "rs", &zhdf,&path,&path_len) != SUCCESS) {
		my_debug( "hdf_read_file parse failed\n" );
		RETURN_FALSE;
	}

	my_debug( "hdf_read_file parse succeed, zhdf %016x path: %s\n" , zhdf , path );
	my_debug( "fetching\n" );
	PHPCS_HDF_PARAM(hdf,zhdf);
	my_debug( "fetched\n" );
	if (!hdf) {
		my_debug( "hdf_read_file resource failed\n" );
		RETURN_FALSE;
	}
	my_debug( "hdf_read_file we cool\n" );

	if (!hdf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to retrieve HDF resource");
		RETURN_FALSE;
	}

	err = hdf_read_file(hdf, path);

	if (err != STATUS_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, neo_error_to_string(err));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool hdf_write_file(resource hdf, string path)
   Write an HDF data set to disk */
PHP_FUNCTION(hdf_write_file) {
	zval *zhdf = NULL;
	char *path = NULL;
	int argc = ZEND_NUM_ARGS();
	PHPCS_STRLEN_VAR path_len = 0;
	HDF *hdf = NULL;
	NEOERR *err = NULL;

	if (zend_parse_parameters(argc TSRMLS_CC, "rs", &zhdf, &path, &path_len) == FAILURE)
		RETURN_FALSE;

	PHPCS_HDF_PARAM(hdf,zhdf);

	if (!hdf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to retrieve HDF resource");
		RETURN_FALSE;
	}

	err = hdf_write_file(hdf, path);

	if (err != STATUS_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, neo_error_to_string(err));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string hdf_write_string(resource hdf)
   Serialize an HDF dataset to a string */
PHP_FUNCTION(hdf_write_string) {
	zval *zhdf = NULL;
	int argc = ZEND_NUM_ARGS();
	HDF *hdf = NULL;
	NEOERR *err = NULL;
	char *str = NULL;

	if (zend_parse_parameters(argc TSRMLS_CC, "r", &zhdf) == FAILURE)
		return;

	PHPCS_HDF_PARAM(hdf,zhdf);

	if (!hdf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to retrieve HDF resource");
		RETURN_NULL();
	}

	err = hdf_write_string(hdf, &str);

	if (err != STATUS_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, neo_error_to_string(err));
		RETURN_NULL();
	}
#ifdef PHP_CS_74
	RETURN_STRING(str);
#else
	RETURN_STRING(str, 1);
#endif
}
/* }}} */

/* {{{ proto bool hdf_read_string(resource hdf, string buf)
   Read an HDF string */
PHP_FUNCTION(hdf_read_string) {
	zval *zhdf = NULL;
	int argc = ZEND_NUM_ARGS();
	HDF *hdf = NULL;
	NEOERR *err = NULL;
	char *str = NULL;
	PHPCS_STRLEN_VAR str_len = 0;

	if (zend_parse_parameters(argc TSRMLS_CC, "rs", &zhdf, &str, &str_len) == FAILURE)
		RETURN_FALSE;

	PHPCS_HDF_PARAM(hdf,zhdf);

	if (!hdf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to retrieve HDF resource");
		RETURN_FALSE;
	}

	err = hdf_read_string(hdf, str);

	if (err != STATUS_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, neo_error_to_string(err));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/*  code modified from php_array_walk() in ext/standard/array.c in php-5.1.0b3*/
int hdf_set_array(HDF *hdf, HashTable *src TSRMLS_DC) {
#ifdef PHP_CS_74
	zval* src_entry = NULL;
#else
	zval** src_entry = NULL;
#endif
#ifdef PHP_CS_74
	zval key;
	zend_string* string_key = NULL;
	zend_ulong	string_key_len;
#else
	zval* key = NULL;
	char	   *string_key = NULL;
	uint		string_key_len;
#endif
	ulong	   num_key;
	HashPosition pos;
	NEOERR *err = NULL;

	zend_hash_internal_pointer_reset_ex(src, &pos);
#ifdef PHP_CS_74
	while (src_entry = zend_hash_get_current_data_ex(src, &pos)) {			// TODO LORI
#else
	while (zend_hash_get_current_data_ex(src, (void **)&src_entry, &pos) == SUCCESS) {
#endif
		HDF *node = NULL;		   /* child node */


#ifdef PHP_CS_74
		/* Set up the key */
		if (zend_hash_get_current_key_ex(src, &string_key, &num_key, &pos) == HASH_KEY_IS_LONG) {	// TODO LORI
#else
		/* Allocate space for key */
		MAKE_STD_ZVAL(key);
		/* Set up the key */
		if (zend_hash_get_current_key_ex(src, &string_key, &string_key_len, &num_key, 0, &pos) == HASH_KEY_IS_LONG) {
#endif
#ifdef PHP_CS_74
			ZVAL_LONG(&key , num_key );
#else
			Z_TYPE_P(key) = IS_LONG;
			Z_LVAL_P(key) = num_key;
#endif
		} else {
#ifdef PHP_CS_74
			ZVAL_STRING(&key, "abc" );
#else
			ZVAL_STRINGL(key, string_key, string_key_len-1, 1);
#endif
		}

#ifndef PHP_CS_74
		convert_to_string_ex(&key);			// TODO LORI
#endif

		/* Create a HDF node equivalent for the current key*/
		//php_printf("key='%s'\n", Z_STRVAL_P(key));
		err = hdf_get_node(hdf, Z_STRVAL(key), &node);

		/* we don't need the key anymore */
#ifdef PHP_CS_74
		zval_ptr_dtor(&key);
#else
		zval_ptr_dtor(&key);
		key = NULL;
#endif

		if (err != STATUS_OK) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, neo_error_to_string(err));
			return 0;
		}

#ifdef PHP_CS_74
		if (Z_TYPE_P(src_entry) == IS_ARRAY) {
#else
		if (Z_TYPE_PP(src_entry) == IS_ARRAY) {
#endif
			HashTable *thash;

			/* we need to check for recursion */
#ifdef PHP_CS_74
			thash = HASH_OF((src_entry));		// TODO LORI
#else
			thash = HASH_OF(*(src_entry));
#endif
			if (thash == src) {
				hdf_set_value(node, NULL, "*RECURSION*");
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "recursion detected");

				return 0;
			}

			if (!hdf_set_array(node, thash TSRMLS_CC)) {
				return 0;
			}

		} else {
			convert_to_string_ex(src_entry);
			//php_printf("key='%s' value='%s'\n", string_key, Z_STRVAL_PP(src_entry));

#ifdef PHP_CS_74
			err = hdf_set_value(node, NULL, Z_STRVAL_P(src_entry));
#else
			err = hdf_set_value(node, NULL, Z_STRVAL_PP(src_entry));
#endif
			if (err != STATUS_OK) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, neo_error_to_string(err));
				return 0;
			}
		}

		zend_hash_move_forward_ex(src, &pos);
	}

	return 1;
}

/* {{{ proto bool hdf_set_value(resource hdf, string name, mixed value)
   Set the value of a named node */
PHP_FUNCTION(hdf_set_value) {
	zval *zhdf = NULL;
	zval *value = NULL;
	char *name = NULL;
	int argc = ZEND_NUM_ARGS();
	PHPCS_STRLEN_VAR name_len = 0;
	HDF *hdf = NULL;
	NEOERR *err = NULL;

	if (zend_parse_parameters(argc TSRMLS_CC, "rsz", &zhdf, &name, &name_len, &value) == FAILURE)
		return;

	PHPCS_HDF_PARAM(hdf,zhdf);

	if (!hdf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to retrieve HDF resource");
		RETURN_FALSE;
	}

	/* start buffering */
#if (defined( PHP_CS_74) || defined( PHP_CS_74) )
	php_output_start_default(TSRMLS_C);
#else
	php_start_ob_buffer (NULL, 0, 1 TSRMLS_CC);
#endif

	/* print zval */
	zend_print_variable(value);

	/* retrieve output buffer */
#if (defined( PHP_CS_74) || defined( PHP_CS_74) )
	php_output_get_contents(return_value TSRMLS_CC);
	php_output_discard(TSRMLS_C);
#else
	php_ob_get_buffer(return_value TSRMLS_CC);
	php_end_ob_buffer(0, 0 TSRMLS_CC);
#endif

	err = hdf_set_value(hdf, name, Z_STRVAL_P(return_value));
	if (err != STATUS_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, neo_error_to_string(err));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string hdf_get_value(resource hdf, string name, string defval)
   Return the value of a node in the data set */
PHP_FUNCTION(hdf_get_value)
{
	zval *zhdf = NULL;
	char *name = NULL;
	char *defval = NULL;
	char *value = NULL;
	int argc = ZEND_NUM_ARGS();
	PHPCS_STRLEN_VAR name_len = 0;
	PHPCS_STRLEN_VAR defval_len = 0;
	HDF *hdf = NULL;

	if (zend_parse_parameters(argc TSRMLS_CC, "rss", &zhdf, &name, &name_len, &defval, &defval_len) == FAILURE)
		return;

	PHPCS_HDF_PARAM(hdf,zhdf);

	if (!hdf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to retrieve HDF resource");
		RETURN_NULL();
	}

	value = hdf_get_value(hdf, name, defval);

#ifdef PHP_CS_74
	RETURN_STRING(value);
#else
	RETURN_STRING(value, 1);
#endif
}
/* }}} */

/* {{{ proto resource hdf_get_node(resource hdf, string name)
   Return the HDF data set node at a named location, creating it if it does not exist */
PHP_FUNCTION(hdf_get_node)
{
	zval *zhdf = NULL;
	char *name = NULL;
	int argc = ZEND_NUM_ARGS();
	NEOERR *err = NULL;
	HDF *hdf = NULL;
	HDF *node = NULL;
	PHPCS_STRLEN_VAR name_len = 0;

	if (zend_parse_parameters(argc TSRMLS_CC, "rs", &zhdf, &name, &name_len) == FAILURE)
		return;

	PHPCS_HDF_PARAM(hdf,zhdf);

	if (!hdf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to retrieve HDF resource");
		RETURN_NULL();
	}

	err = hdf_get_node(hdf, name, &node);

	if (err != STATUS_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, neo_error_to_string(err));
		RETURN_NULL();
	}

	if (!node) {
		RETURN_NULL();
	}

#ifdef PHP_CS_74
	RETURN_RES(zend_register_resource(node,le_clearsilver_hdf));
#else
	ZEND_REGISTER_RESOURCE(return_value, node, le_clearsilver_hdf);
#endif
}
/* }}} */

/* {{{ proto resource hdf_get_obj(resource hdf, string name)
   Return the HDF data set node at a named location */
PHP_FUNCTION(hdf_get_obj)
{
	zval *zhdf = NULL;
	char *name = NULL;
	int argc = ZEND_NUM_ARGS();
	PHPCS_STRLEN_VAR name_len = 0;
	HDF *hdf = NULL;
	HDF *obj = NULL;

	if (zend_parse_parameters(argc TSRMLS_CC, "rs", &zhdf, &name, &name_len) == FAILURE)
		return;

	PHPCS_HDF_PARAM(hdf,zhdf);

	if (!hdf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to retrieve HDF resource");
		RETURN_NULL();
	}

	obj = hdf_get_obj(hdf, name);

	if (!obj) {
		RETURN_NULL();
	}

#ifdef PHP_CS_74
	RETURN_RES(zend_register_resource(obj,le_clearsilver_hdf));
#else
	ZEND_REGISTER_RESOURCE(return_value, obj, le_clearsilver_hdf);
#endif
}
/* }}} */

/* {{{ proto bool hdf_set_node(resource hdf, string name, mixed value)
   Set the node structure of a named node */
PHP_FUNCTION(hdf_set_node)
{
	zval *zhdf = NULL;
	zval *value = NULL;
	char *name = NULL;
	int argc = ZEND_NUM_ARGS();
	PHPCS_STRLEN_VAR name_len = 0;
	HDF *hdf = NULL;
	HDF *node = NULL;		   /* child node */
	NEOERR *err = NULL;

	if (zend_parse_parameters(argc TSRMLS_CC, "rsz", &zhdf, &name, &name_len, &value) == FAILURE)
		return;

	PHPCS_HDF_PARAM(hdf,zhdf);

	if (!hdf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to retrieve HDF resource");
		RETURN_FALSE;
	}

	switch(Z_TYPE_P(value)) {
	case IS_ARRAY:
		err = hdf_get_node(hdf, name, &node);
		if (err != STATUS_OK) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, neo_error_to_string(err));
			RETURN_FALSE;
		}

		if (!hdf_set_array(node, Z_ARRVAL_P(value) TSRMLS_CC)) {
			RETURN_FALSE;
		}

		break;
	case IS_RESOURCE:
		PHPCS_HDF_PARAM(node,value);

		if (!node) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to retrieve HDF resource from value");
			RETURN_FALSE;
		}

		if (hdf == node) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot copy a node into itself");
			RETURN_FALSE;
		}

		err = hdf_copy(hdf, name, node);
		if (err != STATUS_OK) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, neo_error_to_string(err));
			RETURN_FALSE;
		}

		break;
	default:
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid type for value; expecting array or HDF resource");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string hdf_obj_name(resource hdf)
   Return the name of a node */
PHP_FUNCTION(hdf_obj_name)
{
	zval *zhdf = NULL;
	int argc = ZEND_NUM_ARGS();
	char *name = NULL;
	HDF *hdf = NULL;

	if (zend_parse_parameters(argc TSRMLS_CC, "r", &zhdf) == FAILURE)
		return;

	PHPCS_HDF_PARAM(hdf,zhdf);

	if (!hdf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to retrieve HDF resource");
		RETURN_NULL();
	}

	name = hdf_obj_name(hdf);

	if (!name) {
		RETURN_NULL();
	}

#ifdef PHP_CS_74
	RETURN_STRING(name);
#else
	RETURN_STRING(name, 1);
#endif
}
/* }}} */

/* {{{ proto string hdf_obj_value(resource hdf)
   Return the value of a node */
PHP_FUNCTION(hdf_obj_value)
{
	zval *zhdf = NULL;
	int argc = ZEND_NUM_ARGS();
	char *value = NULL;
	HDF *hdf = NULL;

	if (zend_parse_parameters(argc TSRMLS_CC, "r", &zhdf) == FAILURE)
		return;

	PHPCS_HDF_PARAM(hdf,zhdf);
	if (!hdf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to retrieve HDF resource");
		RETURN_NULL();
	}

	value = hdf_obj_value(hdf);

	if (!value) {
		RETURN_EMPTY_STRING();
	}

#ifdef PHP_CS_74
	RETURN_STRING(value);
#else
	RETURN_STRING(value, 1);
#endif
}
/* }}} */

/* {{{ proto resource hdf_obj_child(resource hdf)
   Return the first child of a dataset node */
PHP_FUNCTION(hdf_obj_child)
{
	zval *zhdf = NULL;
	int argc = ZEND_NUM_ARGS();
	HDF *hdf = NULL;
	HDF *child = NULL;

	if (zend_parse_parameters(argc TSRMLS_CC, "r", &zhdf) == FAILURE)
		return;

	PHPCS_HDF_PARAM(hdf,zhdf);
	if (!hdf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to retrieve HDF resource");
		RETURN_NULL();
	}

	child = hdf_obj_child(hdf);

	if (!child) {
		RETURN_NULL();
	}

#ifdef PHP_CS_74
	RETURN_RES(zend_register_resource(child,le_clearsilver_hdf));
#else
	ZEND_REGISTER_RESOURCE(return_value, child, le_clearsilver_hdf);
#endif
}
/* }}} */

/* {{{ proto resource hdf_get_child(resource hdf, string name)
   Return the first child of the named node */
PHP_FUNCTION(hdf_get_child)
{
	zval *zhdf = NULL;
	int argc = ZEND_NUM_ARGS();
	char *name = NULL;
	PHPCS_STRLEN_VAR name_len = 0;
	HDF *hdf = NULL;
	HDF *child = NULL;

	if (zend_parse_parameters(argc TSRMLS_CC, "rs", &zhdf, &name, &name_len) == FAILURE)
		return;

	PHPCS_HDF_PARAM(hdf,zhdf);
	if (!hdf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to retrieve HDF resource");
		RETURN_NULL();
	}

	child = hdf_get_child(hdf, name);

	if (!child) {
		RETURN_NULL();
	}

#ifdef PHP_CS_74
	RETURN_RES(zend_register_resource(child,le_clearsilver_hdf));
#else
	ZEND_REGISTER_RESOURCE(return_value, child, le_clearsilver_hdf);
#endif
}
/* }}} */


/* {{{ proto resource hdf_obj_next(resource hdf)
   Return the next node of a dataset level */
PHP_FUNCTION(hdf_obj_next)
{
	zval *zhdf = NULL;
	int argc = ZEND_NUM_ARGS();
	HDF *hdf = NULL;
	HDF *next = NULL;

	if (zend_parse_parameters(argc TSRMLS_CC, "r", &zhdf) == FAILURE)
		return;

	PHPCS_HDF_PARAM(hdf,zhdf);

	if (!hdf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to retrieve HDF resource");
		RETURN_NULL();
	}

	next = hdf_obj_next(hdf);

	if (!next) {
		RETURN_NULL();
	}

#ifdef PHP_CS_74
	RETURN_RES(zend_register_resource(next,le_clearsilver_hdf));
#else
	ZEND_REGISTER_RESOURCE(return_value, next, le_clearsilver_hdf);
#endif
}
/* }}} */

/* {{{ proto bool hdf_remove_tree(resource hdf, string name)
   Delete a subtree of an HDF dataset */
PHP_FUNCTION(hdf_remove_tree)
{
	zval *zhdf = NULL;
	int argc = ZEND_NUM_ARGS();
	HDF *hdf = NULL;
	char *name = NULL;
	PHPCS_STRLEN_VAR name_len = 0;
	NEOERR *err = NULL;

	if (zend_parse_parameters(argc TSRMLS_CC, "rs", &zhdf, &name, &name_len) == FAILURE)
		RETURN_FALSE;

	PHPCS_HDF_PARAM(hdf,zhdf);

	if (!hdf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to retrieve HDF resource");
		RETURN_FALSE;
	}

	err = hdf_remove_tree(hdf, name);

	if (err != STATUS_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, neo_error_to_string(err));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool hdf_destroy(resource hdf)
   Deallocate an HDF data set */
PHP_FUNCTION(hdf_destroy)
{
	zval *zhdf = NULL;
	int argc = ZEND_NUM_ARGS();
	HDF *hdf = NULL;

	my_debug( "hdf_destroy parse\n" );
	if (zend_parse_parameters(argc TSRMLS_CC, "r", &zhdf) == FAILURE) {
		my_debug( "hdf_destroy parse failed\n" );
		RETURN_FALSE;
	}

	my_debug( "hdf_destroy parse succeed %016x\n" , Z_RES_P(zhdf) );
	PHPCS_HDF_PARAM(hdf,zhdf);
	my_debug( "hdf_destroy fetch succeed %016x\n" , hdf );

	if(hdf != hdf_obj_top(hdf)) {
		my_debug( "hdf_destroy could not find\n" );
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot destroy HDF node: the node is not the root node");
		RETURN_FALSE;
	}

#ifdef PHP_CS_74
	zend_list_delete(Z_RES_P(zhdf));		// TODO LORI
	my_debug( "hdf_destroy succeed\n" );
#else
	zend_list_delete(Z_LVAL_P(zhdf));
#endif

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto resource cs_init(resource hdf)
   Create and initialize a CS context */
PHP_FUNCTION(cs_init) {
	zval *zhdf = NULL;
	CSPARSE *parse = NULL;
	NEOERR *err = NULL;
	int argc = ZEND_NUM_ARGS();
	HDF *hdf = NULL;

	my_debug( "cs_init started\n" );
	if (zend_parse_parameters(argc TSRMLS_CC, "r", &zhdf) == FAILURE) {
		my_debug( "cs_init parse failed\n" );
		return;
	}

	my_debug( "cs_init parse succeed\n" );
	PHPCS_HDF_PARAM(hdf,zhdf);

	my_debug( "cs_init hdf is %016x\n" , hdf );
	if (!hdf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to retrieve HDF resource");
		RETURN_NULL();
	}

	err = cs_init(&parse, hdf);

	if (err != STATUS_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, neo_error_to_string(err));
		RETURN_NULL();
	}

#ifdef PHP_CS_74
	my_debug( "cs_init register resource %016x with num %d\n" , parse , le_clearsilver_cs);
	RETURN_RES(zend_register_resource(parse,le_clearsilver_cs));
#else
	ZEND_REGISTER_RESOURCE(return_value, parse, le_clearsilver_cs);
#endif
}
/* }}} */

/* {{{ proto bool cs_parse_string(resource parse, string buf)
   parse a CS template string */
PHP_FUNCTION(cs_parse_string)
{
	zval *zparse = NULL;
	CSPARSE *parse = NULL;
	NEOERR *err = NULL;
	char *template = NULL;
	char *buf = NULL;
	PHPCS_STRLEN_VAR template_len = 0;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "rs", &zparse, &template, &template_len) == FAILURE)
		RETURN_FALSE;

	PHPCS_CS_PARAM(parse,zparse);

	if (!parse) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to retrieve CS resource");
		RETURN_FALSE;
	}

	buf = strdup(template);

	if (!buf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to duplicate string");
		RETURN_FALSE;
	}

	err = cs_parse_string(parse, buf, template_len);

	if (err != STATUS_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, neo_error_to_string(err));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool cs_parse_file(resource parse, string path)
   parse a CS template file */
PHP_FUNCTION(cs_parse_file)
{
	zval *zparse = NULL;
	CSPARSE *parse = NULL;
	NEOERR *err = NULL;
	char *path = NULL;
	PHPCS_STRLEN_VAR path_len = 0;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "rs", &zparse, &path, &path_len) == FAILURE)
		RETURN_FALSE;

	PHPCS_CS_PARAM(parse,zparse);

	if (!parse) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to retrieve CS resource");
		RETURN_FALSE;
	}

	err = cs_parse_file(parse, path);

	if (err != STATUS_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, neo_error_to_string(err));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string cs_render(resource cs)
   Render a CS parse tree to a string */
PHP_FUNCTION(cs_render)
{
	zval *zparse = NULL;
	CSPARSE *parse = NULL;
	NEOERR *err = NULL;
	STRING str;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "r", &zparse) == FAILURE)
		return;

	PHPCS_CS_PARAM(parse,zparse);

	if (!parse) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to retrieve CS resource");
		RETURN_NULL();
	}

	string_init(&str);
	err = cs_render(parse, &str, render_cb);

	if (err != STATUS_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, neo_error_to_string(err));
		RETURN_NULL();
	}

	if (!str.buf) {
		string_clear (&str);
		RETURN_NULL();
	}

#ifdef PHP_CS_74
	RETVAL_STRING(str.buf);
#else
	RETVAL_STRING(str.buf, 1);
#endif
	string_clear (&str);
}
/* }}} */

/* {{{ proto void cs_destroy(resource parse)
   Clean up and deallocate a CS parse tree */
PHP_FUNCTION(cs_destroy) {
	zval *zparse = NULL;
	CSPARSE *parse = NULL;
	int argc = ZEND_NUM_ARGS();

	my_debug( "cs_destroy started with %d parameters\n" , argc );
	if (zend_parse_parameters(argc TSRMLS_CC, "r", &zparse) == FAILURE) {
		my_debug( "cs_destroy parse failed\n" );
		RETURN_FALSE;
	}
	my_debug( "cs_destroy parse succeed\n" );

	PHPCS_CS_PARAM(parse,zparse);
#ifdef PHP_CS_74
	zend_list_delete(Z_RES_P(zparse));
#else
	zend_list_delete(Z_LVAL_P(zparse));
#endif
	RETURN_TRUE;
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
