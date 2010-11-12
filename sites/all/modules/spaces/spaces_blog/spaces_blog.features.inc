<?php
// $Id: spaces_blog.features.inc,v 1.1.2.1.2.3 2009/12/17 22:33:05 alexb Exp $

/**
 * Implementation of hook_context_default_contexts().
 */
function spaces_blog_context_default_contexts() {
  module_load_include('inc', 'spaces_blog', 'spaces_blog.defaults');
  $args = func_get_args();
  return call_user_func_array('_spaces_blog_context_default_contexts', $args);
}

/**
 * Implementation of hook_ctools_plugin_api().
 */
function spaces_blog_ctools_plugin_api() {
  module_load_include('inc', 'spaces_blog', 'spaces_blog.defaults');
  $args = func_get_args();
  return call_user_func_array('_spaces_blog_ctools_plugin_api', $args);
}

/**
 * Implementation of hook_menu_default_items().
 */
function spaces_blog_menu_default_items() {
  module_load_include('inc', 'spaces_blog', 'spaces_blog.defaults');
  $args = func_get_args();
  return call_user_func_array('_spaces_blog_menu_default_items', $args);
}

/**
 * Implementation of hook_node_info().
 */
function spaces_blog_node_info() {
  module_load_include('inc', 'spaces_blog', 'spaces_blog.features.node');
  $args = func_get_args();
  return call_user_func_array('_spaces_blog_node_info', $args);
}

/**
 * Implementation of hook_strongarm().
 */
function spaces_blog_strongarm() {
  module_load_include('inc', 'spaces_blog', 'spaces_blog.defaults');
  $args = func_get_args();
  return call_user_func_array('_spaces_blog_strongarm', $args);
}

/**
 * Implementation of hook_views_default_views().
 */
function spaces_blog_views_default_views() {
  module_load_include('inc', 'spaces_blog', 'spaces_blog.features.views');
  $args = func_get_args();
  return call_user_func_array('_spaces_blog_views_default_views', $args);
}
