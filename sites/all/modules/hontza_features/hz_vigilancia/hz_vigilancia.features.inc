<?php

/**
 * Implementation of hook_ctools_plugin_api().
 */
function hz_vigilancia_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "context" && $api == "context") {
    return array("version" => 3);
  }
  elseif ($module == "feeds" && $api == "feeds_importer_default") {
    return array("version" => 1);
  }
  elseif ($module == "strongarm" && $api == "strongarm") {
    return array("version" => 1);
  }
}

/**
 * Implementation of hook_node_info().
 */
function hz_vigilancia_node_info() {
  $items = array(
    'canal_busqueda' => array(
      'name' => t('Canal Búsqueda'),
      'module' => 'features',
      'description' => t('Almacena búsquedas locales de contenido.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'has_body' => '1',
      'body_label' => t('Body'),
      'min_word_count' => '0',
      'help' => '',
    ),
    'canal_de_supercanal' => array(
      'name' => t('Canal de supercanal'),
      'module' => 'features',
      'description' => t('Creado a partir de fuentes Dapper y PIPE.'),
      'has_title' => '1',
      'title_label' => t('Título'),
      'has_body' => '1',
      'body_label' => t('Cuerpo'),
      'min_word_count' => '0',
      'help' => '',
    ),
    'canal_de_yql' => array(
      'name' => t('Canal-Filtro de multiples RSS'),
      'module' => 'features',
      'description' => t('Canal que engloba varias fuentes RSS, concatenados con la YQL.'),
      'has_title' => '1',
      'title_label' => t('Título'),
      'has_body' => '0',
      'body_label' => '',
      'min_word_count' => '0',
      'help' => '',
    ),
    'item' => array(
      'name' => t('Item'),
      'module' => 'features',
      'description' => t('Nodo importado mediante el sistema de Feeds.'),
      'has_title' => '1',
      'title_label' => t('Título'),
      'has_body' => '1',
      'body_label' => t('Cuerpo'),
      'min_word_count' => '0',
      'help' => '',
    ),
    'noticia' => array(
      'name' => t('Noticia de usuario'),
      'module' => 'features',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'has_body' => '1',
      'body_label' => t('Body'),
      'min_word_count' => '0',
      'help' => '',
    ),
  );
  return $items;
}

/**
 * Implementation of hook_views_api().
 */
function hz_vigilancia_views_api() {
  return array(
    'api' => '2',
  );
}
