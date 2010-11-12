<?php

/**
 * Formulario multistep para que añadan una fuente parametrizable (algunos le llaman supercanal)
 */
function supercanal_wizard(){
  $step = arg(2);
  ctools_include('wizard');
  ctools_include('object-cache');
  
  $form_info = array(
    'id' => 'supercanal_wizard_form',
    'path' => "crear/fuente-supercanal/%step",
    'show trail' => TRUE,
    'show back' => TRUE,
    'show cancel' => true,
    'show return' =>false,
    'next text' => t('Siguiente paso'),
    'next callback' =>  'supercanal_wizard_next',
    'finish callback' => 'supercanal_wizard_finish',
    'cancel callback' => 'supercanal_wizard_cancel',
    'order' => array(
      'supercanal_wizard_nombre' => t('Paso 1: Nombre e ID del pipe'),
      'supercanal_wizard_num_args' => t('Paso 2: Número de argumentos'),
      'supercanal_wizard_args' => t('Paso 3: Nombre de los argumentos'),
    ),
    'forms' => array(
      'supercanal_wizard_nombre' => array(
        'form id' => 'supercanal_wizard_nombre_form',
      ),
      'supercanal_wizard_num_args' => array(
        'form id' => 'supercanal_wizard_num_args_form',
      ),
      'supercanal_wizard_args' => array(
        'form id' => 'supercanal_wizard_args_form',
      ),
    ),
  );
  $form_state = array(
    'cache name' => NULL,
  );
  // no matter the step, you will load your values from the callback page
  $fuente = ctools_object_cache_get('supercanal_obj', NULL);
  if (!$fuente) {
    global $user;
    // set form to first step -- we have no data
    $step = current(array_keys($form_info['order']));
    $fuente = new stdClass();
    $fuente->type = 'supercanal';
    $fuente->uid = $user->uid;
    $fuente->status = 1;
    // ** set the storage object so its ready for whatever comes next
    ctools_object_cache_set('supercanal_obj', $form_state['cache name'], $fuente);
  }
  //THIS IS WHERE WILL STORE ALL FORM DATA
  $form_state['supercanal_obj'] = $fuente;
  
  $output = ctools_wizard_multistep_form($form_info, $step, $form_state);
  return $output;
}




function supercanal_wizard_next(&$form_state) {
  $fuente = $form_state['supercanal_obj'];
  $cache = ctools_object_cache_set('supercanal_obj', $form_state['cache name'], $fuente);
}

function supercanal_wizard_finish(&$form_state){
  node_save($form_state['supercanal_obj']);
  //$node = node_load(array('title' => $form_state['supercanal_obj']->title));
  $node = $form_state['supercanal_obj'];
  //drupal_set_message(t('Se ha creado la fuente !name', array('!name' => l($node->title, 'node/'.$node->nid))));
  ctools_object_cache_clear('supercanal_obj', $form_state['cache name']);
  //$form_state['redirect'] = 'node/'.$node->nid;
  $form_state['redirect'] = 'fuentes';

}
function supercanal_wizard_cancel(&$form_state){
  ctools_object_cache_clear('supercanal_obj', $form_state['cache name']);
  //$form_state['redirect'] = 'node';
  drupal_goto('fuentes/todas');
}


/**
 * Formulario para el primer paso. Aquí introducimos los campos de título,
 * categoría y valoración
 */
function supercanal_wizard_nombre_form(&$form, &$form_state){
  $fuente = $form_state['supercanal_obj'];
  $form['titulo'] = array(
    '#title' => t('Nombre para la fuente Pipe'),
    '#type' => 'textfield',
    '#required' => 1,
  );
  
  //$form['grupos'] = array(
  //  '#title' => t('Grupos'),
  //  '#type' => 'checkboxes',
  //  '#required' => TRUE,
  //  '#options' => get_grupos(),
  //);
  $form['pipe_id'] = array(
    '#title' => t('ID del Pipe'),
    '#type' => 'textfield',
    '#size' => 30,
    '#required' => TRUE,
  ); 
  $padres = taxonomy_get_tree(1, 0, -1, 1);
  $taxo = array();
  foreach ($padres as $padre){
    $taxo[$padre->tid] = $padre->name;
    $hijos = taxonomy_get_children($padre->tid);
    if (!empty($hijos)){
      foreach ($hijos as $hijo){
        $taxo[$hijo->tid] = '--'.$hijo->name;
      }
    }
  }
  $form['taxonomia'] = array(
    '#title' => t('Asignar una o más clasificaciones'),
    '#type' => 'checkboxes',
    '#multiple' => TRUE,
    '#required' => TRUE,
    '#options' => $taxo,
  );
  $form['valoracion'] = array(
    '#title' => t('Valoración de la fuente'),
    '#type' => 'fieldset',
  );
  $form['valoracion']['calidad'] = array(
    '#title' => t('Calidad de la información'),
    '#type' => 'select',
    '#required' => TRUE,
    '#options' => array('Muy mala','Mala','Normal','Buena','Muy buena'),
    '#default_value' => 2,
  );
  $form['valoracion']['exhaustividad'] = array(
    '#title' => t('Exhaustividad'),
    '#type' => 'select',
    '#required' => TRUE,
    '#options' => array('Muy mala','Mala','Normal','Buena','Muy buena'),
    '#default_value' => 2,
  );
  $form['valoracion']['actualizacion'] = array(
    '#title' => t('Actualización'),
    '#type' => 'select',
    '#required' => TRUE,
    '#options' => array('Muy mala','Mala','Normal','Buena','Muy buena'),
    '#default_value' => 2,
  );
  /*
  $form['publica'] = array(
    '#title' => t('Fuente p&uacute;blica'),
    '#type' => 'fieldset',
  );
  $form['publica']['check']= array(
    '#title' => t('Hacer p&uacute;blica a todos los grupos esta fuente Dapper'),
    '#type' => 'checkbox',
  );
  */
  $form_state['no buttons'] = TRUE; 
}

function supercanal_wizard_nombre_form_submit(&$form, &$form_state){
  $fuente = $form_state['supercanal_obj'];
  $fuente->title = $form_state['values']['titulo'];
  $vid = db_result(db_query("SELECT vid FROM {term_data} WHERE tid = %d", key($form_state['values']['taxonomia'])));
  foreach($form_state['values']['taxonomia'] as $tid){
    $fuente->taxonomy[$vid][$tid] = $tid;
  }
  //$fuente->field_pipe_args[0]['value'] = array('value' => 'http://pipes.yahoo.com/pipes/pipe.run?_id='.$form_state['values']['pipe_id'].'&_render=rss');
  $fuente->field_supercanal_fuente[0] = array('value' => $form_state['values']['pipe_id']);
  
  $fuente->field_supercanal_calidad[0]['rating'] = ($form_state['values']['calidad'] + 1) * 20;
  $fuente->field_supercanal_calidad[0]['target'] = 0;
  
  $fuente->field_supercanal_exhaustividad[0]['rating'] = ($form_state['values']['exhaustividad'] + 1) * 20;
  $fuente->field_supercanal_exhaustividad[0]['target'] = 0;
  
  $fuente->field_supercanal_actualizacion[0]['rating'] = ($form_state['values']['actualizacion'] + 1) * 20;
  $fuente->field_supercanal_actualizacion[0]['target'] = 0;
  
   if($form_state['values']['check']==1){
    $fuente->field_tematica_gupos[0]['value'] = 'publico';
  }
  else{
    $fuente->field_tematica_gupos[0]['value'] = 'privado';
  }  
  $form_state['supercanal_obj'] = $fuente;
}
  

/**
 * Formulario para que introduzcan el número de argumentos que tendrá la fuente
 */
function supercanal_wizard_num_args_form(&$form, &$form_state){
  $form['num_args'] = array(
  '#title' => t('Número de argumentos.'),
  '#type' => 'textfield',
  '#required' => 1,
  );
  $form_state['no buttons'] = TRUE; 
}
function fuente_parametrizable_num_args_form_validate(&$form, &$form_state){
  if (!is_numeric($form_state['values']['num_args'])){
    form_set_error('num_args', t('El valor de este campo ha de ser numérico.'));
  }
}

function supercanal_wizard_num_args_form_submit(&$form, &$form_state){
  $form_state['supercanal_obj']->num_args = $form_state['values']['num_args'];
}


/**
 *Formulario para que introduzcan los nombres de los argumentos.
 **/
function supercanal_wizard_args_form(&$form, &$form_state){
  //dprint_r($form_state);
  $num_args = $form_state['supercanal_obj']->num_args;
  for($i = 0; $i < $num_args; $i++){
    $arg = 'args'.$i;
    $form[$arg] = array(
    '#title' => t('Nombre del argumento @num', array('@num' => $i + 1)),
    '#type' => 'fieldset',
    );
    $form[$arg]['arg_name'.$i] = array(
      '#title' => t("Nombre del argumento"),
      '#type' => 'textfield',
      '#size' => 30,
      '#required' => TRUE,
    );
    $form[$arg]['arg_desc'.$i] = array(
      '#title' => t("Descripción del argumento"),
      '#type' => 'textfield',
      '#size' => 30,
    );
  }
  $form_state['no buttons'] = TRUE; 
}

function supercanal_wizard_args_form_submit(&$form, &$form_state){
  $num_args = $form_state['supercanal_obj']->num_args;
  if ($num_args > 0){
    for($i = 0; $i < $num_args; $i++){
      $form_state['supercanal_obj']->field_supercanal_args[]['value'] = $form_state['values']['arg_name'.$i];
      $form_state['supercanal_obj']->field_supercanal_args_desc[]['value'] = $form_state['values']['arg_desc'.$i].' ';
    }
  }
}