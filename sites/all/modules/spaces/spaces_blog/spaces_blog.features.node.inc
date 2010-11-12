<?php
// $Id: spaces_blog.features.node.inc,v 1.1.2.2 2009/12/17 22:33:05 alexb Exp $

/**
 * Helper to implementation of hook_node_info().
 */
function _spaces_blog_node_info() {
  $items = array(
    'blog' => array(
      'name' => t('Blog'),
      'module' => 'features',
      'description' => t('A simple blog post for publishing content online.'),
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
