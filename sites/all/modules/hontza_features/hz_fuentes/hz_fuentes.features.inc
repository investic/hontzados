<?php

/**
 * Implementation of hook_ctools_plugin_api().
 */
function hz_fuentes_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "context" && $api == "context") {
    return array("version" => 3);
  }
  elseif ($module == "strongarm" && $api == "strongarm") {
    return array("version" => 1);
  }
}

/**
 * Implementation of hook_node_info().
 */
function hz_fuentes_node_info() {
  $items = array(
    'fuentedapper' => array(
      'name' => t('Fuente DAPPER'),
      'module' => 'features',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Identificador del Dapper'),
      'has_body' => '0',
      'body_label' => '',
      'min_word_count' => '0',
      'help' => '',
    ),
    'supercanal' => array(
      'name' => t('Fuente PIPE'),
      'module' => 'features',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Título'),
      'has_body' => '1',
      'body_label' => t('Cuerpo'),
      'min_word_count' => '0',
      'help' => '',
    ),
  );
  return $items;
}

/**
 * Implementation of hook_views_api().
 */
function hz_fuentes_views_api() {
  return array(
    'api' => '2',
  );
}
