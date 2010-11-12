<?php

/**
 * Implementation of hook_ctools_plugin_api().
 */
function hz_publica_ctools_plugin_api() {
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
function hz_publica_node_info() {
  $items = array(
    'noticias_portada' => array(
      'name' => t('Noticias Portada'),
      'module' => 'features',
      'description' => t('Contenido para la creación de noticias que se vean en la portada de acceso publico'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'has_body' => '0',
      'body_label' => '',
      'min_word_count' => '0',
      'help' => '',
    ),
    'rss_feed' => array(
      'name' => t('Rss home pública'),
      'module' => 'features',
      'description' => t('Contenido para la importacion de contenidos de un Rss'),
      'has_title' => '1',
      'title_label' => t('Titulo'),
      'has_body' => '0',
      'body_label' => '',
      'min_word_count' => '0',
      'help' => '',
    ),
    'rss_feed_importador' => array(
      'name' => t('Rss importador'),
      'module' => 'features',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Título'),
      'has_body' => '0',
      'body_label' => '',
      'min_word_count' => '0',
      'help' => '',
    ),
  );
  return $items;
}

/**
 * Implementation of hook_views_api().
 */
function hz_publica_views_api() {
  return array(
    'api' => '2',
  );
}
