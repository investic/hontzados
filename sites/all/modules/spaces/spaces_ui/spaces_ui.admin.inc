<?php
// $Id: spaces_ui.admin.inc,v 1.1.2.5 2010/02/15 03:33:52 yhahn Exp $

/**
 * Presets form.
 */
function spaces_preset_list($form_state) {
  $form = array();

  $types = array();
  foreach (spaces_types() as $type => $info) {
    $types[$type] = $info['title'];
  }

  $form['new'] = array(
    '#tree' => FALSE,
    '#theme' => 'spaces_preset_list_new',
    'space_type' => array(
      '#title' => t('Type'),
      '#type' => 'select',
      '#options' => $types,
    ),
    'name' => array(
      '#type' => 'textfield',
      '#maxlength' => 64,
      '#size' => 32,
      '#element_validate' => array('spaces_preset_name_validate'),
      '#title' => t('Machine ID'),
    ),
    'title' => array(
      '#type' => 'textfield',
      '#maxlength' => 64,
      '#size' => 32,
      '#title' => t('Name'),
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Create new preset'),
      '#submit' => array('spaces_preset_list_new'),
    ),
  );

  // Generate preset options.
  foreach ($types as $type => $title) {
    module_load_include('inc', 'spaces', 'spaces.admin');
    $presets = spaces_preset_load(NULL, $type, TRUE);
    ksort($presets);
    $form[$type] = spaces_preset_form($presets, $type);
    $form[$type]['#title'] = t('@spacetype presets', array('@spacetype' => $title));
    $form[$type]['#description'] = t('Select a default preset for each new @spacetype.', array('@spacetype' => $title));
  }
  $form = system_settings_form($form);
  return $form;
}

/**
 * Submit handler for preset creation.
 */
function spaces_preset_list_new(&$form, &$form_state) {
  $preset = ctools_export_new_object('spaces_presets');
  $preset->name = $form_state['values']['name'];
  $preset->title = $form_state['values']['title'];
  $preset->space_type = $form_state['values']['space_type'];
  spaces_preset_save($preset);
}

/**
 * Validate preset name values.
 */
function spaces_preset_name_validate($element, &$form_state) {
  if (isset($form_state['clicked_button']['#submit']) && $form_state['clicked_button']['#submit'] === 'spaces_preset_list_new') {
    // Check for string identifier sanity
    if (!preg_match('!^[a-z0-9_]+$!', $element['#value'])) {
      form_set_error('name', t('The preset name can only consist of lowercase letters, underscores, and numbers.'));
    }
    // Check for name collision
    else if ($exists = spaces_preset_load($element['#value'])) {
      form_set_error('name', t('A preset  with this name already exists. Please choose another name or delete the existing preset before creating a new one.'));
    }
  }
}

/**
 * Preset editor form.
 */
function spaces_preset_editor($form_state, $preset) {
  $form = array();
  $form['#preset'] = $preset;
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Machine name'),
    '#default_value' => $preset->name,
    '#disabled' => TRUE,
  );
  $form['title']= array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => $preset->title,
    '#maxlength' => 64,
  );
  $form['description']= array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#default_value' => $preset->description,
  );

  $space = spaces_load($preset->space_type, 0);
  $stack = array('preset');
  if ($space && !empty($preset->value)) {
    foreach ($preset->value as $controller => $overrides) {
      if (!empty($overrides) && is_array($overrides)) {
        foreach ($overrides as $key => $value) {
          $form['revert'][$controller][$key] = array(
            '#type' => 'checkbox',
            '#title' => $key,
            '#disabled' => FALSE,
            '#description' => $space->controllers->{$controller}->summary($key, $value),
          );
        }
      }
    }
    if (element_children($form['revert'])) {
      $form['revert']['#tree'] = TRUE;
      $form['revert']['#theme'] = 'spaces_overrides_form';
      $form['revert']['revert'] = array(
        '#type' => 'item',
        '#title' => t('Remove'),
        '#description' => t('Remove the selected overrides from this preset.'),
      );
      $form['revert']['revert']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Remove overrides'),
        '#submit' => array('spaces_preset_editor_remove_overrides'),
      );
    }
  }

  $form['submit'] = array(
    '#weight' => 11,
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['#theme'] = 'system_settings_form';
  return $form;
}

/**
 * Submit handler for removing specific preset overrides.
 */
function spaces_preset_editor_remove_overrides(&$form, &$form_state) {
  // Reload the preset clearing static cache in case someone else's
  // submit handler ran before ours, saving the preset.
  $preset = spaces_preset_load($form['#preset']->name, NULL, TRUE);
  if ($preset) {
    foreach (array_keys(spaces_controllers()) as $controller) {
      if (!empty($preset->value[$controller])) {
        $remove = !empty($form_state['values']['revert'][$controller]) ? array_filter($form_state['values']['revert'][$controller]) : array();
        $preset->value[$controller] = array_diff_key($preset->value[$controller], $remove);
      }
    }
    spaces_preset_save($preset);
    drupal_set_message(t('Saved preset %preset.', array('%preset' => $preset->title)));
  }
}

/**
 * Submit handler for preset editor.
 */
function spaces_preset_editor_submit(&$form, &$form_state) {
  // Reload the preset clearing static cache in case someone else's
  // submit handler ran before ours, saving the preset.
  $preset = spaces_preset_load($form['#preset']->name, NULL, TRUE);
  $preset->title = $form_state['values']['title'];
  $preset->description = $form_state['values']['description'];
  spaces_preset_save($preset);
  drupal_set_message(t('Saved preset %preset.', array('%preset' => $preset->title)));
  $form_state['redirect'] = 'admin/build/spaces';
}

/**
 * Provide a form to confirm one of the provided actions.
 */
function spaces_preset_confirm(&$form_state, $op = 'delete', $preset) {
  $form = array();
  $form['preset'] = array('#type' => 'value', '#value' => $preset);
  $form['action'] = array('#type' => 'value', '#value' => $op);
  switch ($op) {
    case 'revert':
      $action = t('revert');
      $message = t('This action will permanently remove any customizations made to this preset.');
      break;
    case 'delete':
      $action = t('delete');
      $message = t('This action will remove this preset permanently from your site.');
      break;
    case 'disable':
      $action = t('disable');
      $message = '';
      break;
    case 'enable':
      $action = t('enable');
      $message = '';
      break;
  }
  $form = confirm_form($form,
    t('Are you sure you want to !action the preset %title?', array('%title' => $preset->title, '!action' => $action)),
    'admin/build/spaces',
    $message,
    drupal_ucfirst($action), t('Cancel')
  );
  return $form;
}

/**
 * Submit handler for the spaces_preset_confirm form.
 */
function spaces_preset_confirm_submit($form, &$form_state) {
  $preset = $form_state['values']['preset'];
  switch ($form_state['values']['action']) {
    case 'revert':
    case 'delete':
      spaces_preset_delete($preset);
      break;
    case 'disable':
      ctools_export_set_object_status($preset);
      break;
    case 'enable':
      ctools_export_set_object_status($preset, FALSE);
      break;
  }
  cache_clear_all('spaces_presets', 'cache');
  $form_state['redirect'] = 'admin/build/spaces';
}

/**
 * Provides a form with an exported preset definition for use in modules.
 */
function spaces_preset_export(&$form_state, $preset) {
  drupal_set_title(t('Export %title', array('%title' => $preset->title)));
  return ctools_export_form($form_state, spaces_export_spaces_presets($preset), t('Export'));
}
