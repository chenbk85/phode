/*
 * Copyright (c) 2011, Ben Noordhuis <info@bnoordhuis.nl>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#include "uv.h"


typedef struct {
  uv_tcp_t handle;
  zend_object obj;
} tcp_wrap;


static zend_object_value tcp_new(zend_class_entry *class_type TSRMLS_DC) {
  zend_object_value instance;
  tcp_wrap *intern;
  zval *tmp;

  intern = emalloc(sizeof *intern);
  uv_tcp_init(uv_default_loop(), &intern->handle);

  zend_object_std_init(&intern->obj, class_type TSRMLS_CC);
  zend_hash_copy(intern->obj.properties,
                 &class_type->default_properties,
                 (copy_ctor_func_t) zval_add_ref,
                 &tmp,
                 sizeof tmp);

  instance.handle = zend_objects_store_put(intern,
                                           (zend_objects_store_dtor_t) zend_objects_destroy_object,
                                           (zend_objects_free_object_storage_t) zend_object_std_dtor,
                                           NULL TSRMLS_CC);
  instance.handlers = zend_get_std_object_handlers();

  return instance;
}


PHP_FUNCTION(tcp_connect) {
  RETURN_NULL();
}


static zend_function_entry tcp_methods[] = {
  PHP_FALIAS(connect, tcp_connect, NULL)
  { NULL }
};


PHP_MINIT_FUNCTION(node) {
  zend_class_entry ce;

  uv_init();

  INIT_CLASS_ENTRY(ce, "TCP", tcp_methods);
  ce.create_object = tcp_new;
  zend_register_internal_class(&ce TSRMLS_CC);

  return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(node) {
  return SUCCESS;
}


PHP_MINFO_FUNCTION(node) {
  php_info_print_table_start();
  php_info_print_table_header(2, "node", "enabled");
  php_info_print_table_end();
}


PHP_FUNCTION(uv_run) {
  uv_run(uv_default_loop());
  RETURN_NULL();
}


static zend_function_entry functions[] = {
  PHP_FE(uv_run, NULL)
  { NULL, NULL, NULL }
};


zend_module_entry node_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
  STANDARD_MODULE_HEADER,
#endif
  "node",
  functions,
  PHP_MINIT(node),
  PHP_MSHUTDOWN(node),
  NULL,
  NULL,
  PHP_MINFO(node),
#if ZEND_MODULE_API_NO >= 20010901
  "0.0.1",
#endif
  STANDARD_MODULE_PROPERTIES
};


ZEND_GET_MODULE(node)
