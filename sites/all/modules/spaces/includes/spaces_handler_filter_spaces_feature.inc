<?php
// $Id: spaces_handler_filter_spaces_feature.inc,v 1.1.2.1.2.2.2.3 2010/01/10 07:12:36 yhahn Exp $

/**
 * Filter to only show node types associated with current feature.
 */
class spaces_handler_filter_spaces_feature extends views_handler_filter {
  /**
   * override can_expose()
   */
  function can_expose() { return FALSE; }

  function query() {
    if (module_exists('context')) {
      $types = array();
      foreach (context_get('context') as $context) {
        // @TODO: This hardcoded list is rather ugly... also doesn't this
        // handler really belong in context??? : |
        $plugins = array('node', 'bookroot');
        foreach ($plugins as $plugin) {
          if (!empty($context->conditions[$plugin]['values'])) {
            $types = array_merge($types, $context->conditions[$plugin]['values']);
          }
        }
      }
      $types = drupal_map_assoc($types);

      if (!empty($types)) {
        $placeholders = db_placeholders($types, 'varchar');
        if (!empty($this->relationship)) {
          $table = $this->relationship;
        }
        else {
          $table = $this->query->ensure_table('node');
        }
        $this->query->add_where($this->options['group'], "$table.type IN ($placeholders)", $types);
      }
    }
  }
}
