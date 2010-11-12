<?php
// $Id: spaces_ui.theme.inc,v 1.1.2.2 2009/12/17 22:33:05 alexb Exp $

/**
 * Theme function for spaces_presets_list_new().
 */
function theme_spaces_preset_list_new($form) {
  drupal_add_css(drupal_get_path('module', 'spaces') .'/spaces.css');
  $row = $header = array();
  foreach (element_children($form) as $key) {
    if (isset($form[$key]['#title'])) {
      $header[] = $form[$key]['#title'];
      unset($form[$key]['#title']);
    }
    else {
      $header[] = '';
    }
    $row[] = drupal_render($form[$key]);
  }
  $output = theme('table', $header, array($row));
  $output .= drupal_render($form);
  return $output;
}
