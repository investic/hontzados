<?php
// $Id: node_export.drush.inc,v 1.1.2.10 2010/05/06 04:06:24 danielb Exp $

/**
 * @file
 *   Drush support for node_export.
 */

/**
 * Implementation of hook_drush_command().
 */
function node_export_drush_command() {
  $items = array();

  $items['node-export-export'] = array(
    'callback' => 'drush_node_export_export',
    'description' => "Export nodes to code.",
    'arguments' => array(
      'nids' => 'Optional list of IDs of nodes to export. If not provided all nodes matching the filtering options will be exported.',
    ),    
    'options' => array(
      '--file' => 'Path to the output file. If not supplied, exported code will be printed to stdout.',
    ),
    'examples' => array(
      'drush ne-export 45 46 47' => 'Export nodes 45, 46, and 47 to stdout.',
      'drush ne-export --status=published --type="page,story" --file=filename.txt' => 'Export published nodes of type page and story to filename.txt'
    ),
    'aliases' => array('ne-export'),
  );
  drush_bootstrap_max();
  if (drush_get_context('DRUSH_BOOTSTRAP_PHASE') >= DRUSH_BOOTSTRAP_DRUPAL_FULL) {
    module_load_include('inc', 'node', 'node.admin');
    $choices = array();
    foreach (node_filters() as $filter => $data) {
      foreach ($data['options'] as $key => $value) {
        $choices[] = "$key ($value)";
      }
      $items['node-export-export']['options']['--'.$filter] = dt('Filter nodes by !option. Choices are: !choices.', array('!option' => $filter, '!choices' => implode(', ', $choices)));
    }
  }

  $items['node-export-import'] = array(
    'description' => "Import nodes from a previous export.",
    'options' => array(
      '--uid' => "User ID to save nodes with. If not supplied uid 1 will be used. You may specify 0 for the Anonymous user.",
      '--file' => "Path to the input file. If not supplied, it will read from stdin.",
    ),
    'examples' => array(
      'drush ne-import --file=filename.txt' => 'Import nodes from filename.txt.',
      'drush ne-import --uid=2' => 'Import nodes from stdin. Nodes will be saved with uid 2.',
    ),
    'aliases' => array('ne-import'),
  );

  return $items;
}

/**
 * Implementation of hook_drush_help().
 */
function node_export_drush_help($section) {
  switch ($section) {
    case 'drush:node-export-export':
      return dt('Export nodes to code. You can select what nodes to export by several criteria.');
    case 'drush:node-export-import':
      return dt('Import nodes from a previous export.');
  }
}

/**
 * Drush command callback.
 *
 * Export nodes.
 */
function drush_node_export_export() {
  $nids = func_get_args();

  // If nids are not provided, use the filters. Ignore otherwise.
  if (!count($nids)) {
    module_load_include('inc', 'node', 'node.admin');
    $_SESSION['node_overview_filter'] = array();
    foreach (node_filters() as $filter => $data) {
      $value = drush_get_option($filter);
      if (!is_null($value)) {
        $_SESSION['node_overview_filter'][] = array($filter, $value);
      }
    }
    if (empty($_SESSION['node_overview_filter'])) {
      if (!drush_confirm(dt('Do you want to export all nodes?'))) {
        return drush_log(dt('Aborting.'));
      }
      $query = array('where' => '', 'join' => '', 'args' => array());
    }
    else {
      $query = node_build_filter_query();
    }
    $rsc = db_query('SELECT nid FROM {node} n '.$query['join'].' '.$query['where'], $query['args']);
    if ($rsc === FALSE) {
      return drush_log(dt('Aborting.'));
    }
    while ($nid = db_result($rsc)) {
      $nids[] = $nid;
    }
  }

  $data = node_export_node_bulk($nids, TRUE);
  $filename = drush_get_option('file');
  if ($filename) {
    // Output data to file. Note this only takes a flat filename for the current directory.
    // If file exists, ask for whether to overwrite.
    if (file_exists($filename)) {
      if (!drush_confirm(dt('File !filename exists. Do you really want to overwrite?', array('!filename' => $filename)))) {
        return; 
      }
    }
    // Write the file.
    file_put_contents($filename, $data);
  }
  else {
    // Print to terminal.
    drush_print_r($data);
  }
}

/**
 * Drush command callback.
 *
 * Import nodes from data.
 */
function drush_node_export_import() {
  $commands = func_get_args();
    
  // Switch to admin or the specified user so imported nodes are not anonymous.
  $uid = drush_get_option('uid');
  // Test on NULL so uid may be given as 0.
  if (is_null($uid)) {
    $uid = 1;
  }
  // User 0 is already loaded.
  if ($uid != 0) {
    global $user;
    $user = user_load($uid);
  }

  $filename = drush_get_option('file');

  if ($filename) {
    $node_code = file_get_contents($filename, "r");
  }
  else {
    $node_code = file_get_contents("php://stdin", "r");
  }

  if (!empty($node_code)) {
    $result = node_export_import($node_code, 'save-edit', FALSE, 'drush_print', 'dt');
    if ($result === FALSE) {
      // There was a problem with types?
      drush_set_error('DRUSH_NOT_COMPLETED', "A problem was found with the import data. Check that the node types and all the required modules exist.");
    }
  }
}
