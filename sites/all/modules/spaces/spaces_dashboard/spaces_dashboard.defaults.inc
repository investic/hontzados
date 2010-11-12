<?php
// $Id: spaces_dashboard.defaults.inc,v 1.1.2.4 2010/01/10 07:54:44 yhahn Exp $

/**
 * Helper to implementation of hook_context_default_contexts().
 */
function _spaces_dashboard_context_default_contexts() {
  $export = array();
  $context = new stdClass;
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'spaces_dashboard';
  $context->description = 'Dashboard';
  $context->tag = 'Dashboard';
  $context->conditions = array(
    'path' => array(
      'values' => array(
        'dashboard' => 'dashboard',
        'dashboard/*' => 'dashboard/*',
      ),
    ),
  );
  $context->reactions = array(
    'menu' => 'dashboard',
  );

  $export['spaces_dashboard'] = $context;
  return $export;
}

/**
 * Helper to implementation of hook_ctools_plugin_api().
 */
function _spaces_dashboard_ctools_plugin_api() {
  $args = func_get_args();
  $module = array_shift($args);
  $api = array_shift($args);
  if ($module == "context" && $api == "context") {
    return array("version" => 3);
  }
}

/**
 * Helper to implementation of hook_menu_default_items().
 */
function _spaces_dashboard_menu_default_items() {
  $items = array();

  $items[] = array(
    'title' => 'Dashboard',
    'path' => 'dashboard',
    'weight' => '0',
  );
  // Translatables
  array(
    t('Dashboard'),
  );


  return $items;
}
