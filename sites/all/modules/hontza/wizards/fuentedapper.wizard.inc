<?php

/**
 * Formulario multistep para que añadan una fuente parametrizable (algunos le llaman supercanal de tipo dapper)
 */
function fuentedapper_wizard(){
  $step = arg(2);
  ctools_include('wizard');
  ctools_include('ajax');
  ctools_include('object-cache');
  
  $form_info = array(
    'id' => 'fuentedapper_wizard_form',
    'path' => "crear/fuente-dapper/%step",
    'show trail' => TRUE,
    'show back' => TRUE,
    'show cancel' => true,
    'show return' =>false,
    'next text' => t('Siguiente paso'),
    'next callback' =>  'fuentedapper_wizard_next',
    'finish callback' => 'fuentedapper_wizard_finish',
    'cancel callback' => 'fuentedapper_wizard_cancel',
    'ajax' => TRUE,
    'order' => array(
      'fuentedapper_wizard_nombre' => t('Paso 1: Información de dapper'),
      'fuentedapper_wizard_item_title' => t('Paso 2: Cantidad de items'),
      'fuentedapper_wizard_extargs' => t('Paso 3: Nombre de los items'),
      
      'fuentedapper_wizard_num_args' => t('Paso 4: Número de filtros'),
      'fuentedapper_wizard_args' => t('Paso 5: Nombre de los filtros'),
      'fuentedapper_wizard_desp' => t('Paso 6: Filtros de tipo desplegable'),
    ),
    'forms' => array(
      'fuentedapper_wizard_nombre' => array(
        'form id' => 'fuentedapper_wizard_nombre_form',
      ),
      'fuentedapper_wizard_item_title' => array(
        'form id' => 'fuentedapper_wizard_item_title_form',
      ),
      'fuentedapper_wizard_extargs' => array(
        'form id' => 'fuentedapper_wizard_extargs_form',
      ),
      'fuentedapper_wizard_num_args' => array(
        'form id' => 'fuentedapper_wizard_num_args_form',
      ),
      'fuentedapper_wizard_args' => array(
        'form id' => 'fuentedapper_wizard_args_form',
      ),
      'fuentedapper_wizard_desp' => array(
        'form id' => 'fuentedapper_wizard_desp_form',
      ),
    ),
  );
  $form_state = array(
    'cache name' => NULL,
  );
  // no matter the step, you will load your values from the callback page
  $fuente = ctools_object_cache_get('fuentedapper_obj', NULL);
  if (!$fuente) {
    global $user;
    // set form to first step -- we have no data
    $step = current(array_keys($form_info['order']));
    $fuente = new stdClass();
    $fuente->type = 'fuentedapper';
    $fuente->uid = $user->uid;
    $fuente->status = 1;
    // ** set the storage object so its ready for whatever comes next
    ctools_object_cache_set('fuentedapper_obj', $form_state['cache name'], $fuente);
  }
  //THIS IS WHERE WILL STORE ALL FORM DATA
  $form_state['fuentedapper_obj'] = $fuente;
  
  $output = ctools_wizard_multistep_form($form_info, $step, $form_state);
  return $output;
}



function fuentedapper_wizard_next(&$form_state) {
  $fuente = $form_state['fuentedapper_obj'];
  $cache = ctools_object_cache_set('fuentedapper_obj', $form_state['cache name'], $fuente);
}

function fuentedapper_wizard_finish(&$form_state){
  node_save($form_state['fuentedapper_obj']);
  
  //$node = node_load(array('title' => $form_state['supercanal_obj']->title));
  //$node = $form_state['fuentedapper_obj'];
  
  //drupal_set_message(t('Se ha creado la fuente !name de tipo dapper', array('!name' => l($node->title, 'node/'.$node->nid))));
  ctools_object_cache_clear('fuentedapper_obj', $form_state['cache name']);
  //$form_state['redirect'] = 'node/'.$node->nid;
  $form_state['redirect'] = 'fuentes';
}

function fuentedapper_wizard_cancel(&$form_state){
  ctools_object_cache_clear('fuentedapper_obj', $form_state['cache name']);
  //$form_state['redirect'] = 'fuente';
  drupal_goto('fuentes/todas');

}


/**
 * Formulario para el primer paso. Aquí introducimos los campos de título,
 * categoría y valoración
 */
function fuentedapper_wizard_nombre_form(&$form, &$form_state){
  
  
  $fuente = $form_state['fuentedapper_obj'];
  $form['titulo'] = array(
    '#title' => t('Nombre del ID del Dapp'),
    '#type' => 'textfield',
    '#required' => 1,
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
    '#title' => t('¿Qué categorías va a tener?'),
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

function fuentedapper_wizard_nombre_form_submit(&$form, &$form_state){
  $fuente = $form_state['fuentedapper_obj'];
  $fuente->title = $form_state['values']['titulo'];
  $vid = db_result(db_query("SELECT vid FROM {term_data} WHERE tid = %d", key($form_state['values']['taxonomia'])));
  foreach($form_state['values']['taxonomia'] as $tid){
    $fuente->taxonomy[$vid][$tid] = $tid;
  }
  //$fuente->field_pipe_args[0]['value'] = array('value' => 'http://pipes.yahoo.com/pipes/pipe.run?_id='.$form_state['values']['pipe_id'].'&_render=rss');
  //$fuente->field_fuentedapper_fuente[0] = array('value' => $form_state['values']['pipe_id']);
  $fuente->field_fuentedapper_fuente[0] = array('value' => 'RSS');
  $fuente->field_fuentedapper_calidad[0]['rating'] = ($form_state['values']['calidad'] + 1) * 20;
  $fuente->field_fuentedapper_calidad[0]['target'] = 0;
  
  $fuente->field_fuentedapper_exhaustividad[0]['rating'] = ($form_state['values']['exhaustividad'] + 1) * 20;
  $fuente->field_fuentedapper_exhaustividad[0]['target'] = 0;
  
  $fuente->field_fuentedapper_actualizacion[0]['rating'] = ($form_state['values']['actualizacion'] + 1) * 20;
  $fuente->field_fuentedapper_actualizacion[0]['target'] = 0;
  
  
  if($form_state['values']['check']==1){
    $fuente->field_tematica_gupos[0]['value'] = 'publico';
  }
  else{
    $fuente->field_tematica_gupos[0]['value'] = 'privado';
  }
  $form_state['fuentedapper_obj'] = $fuente;
}

/**
 * Formulario completar item titulo y cantidad de items
 */
function fuentedapper_wizard_item_title_form(&$form, &$form_state){
    $form['title'] = array(
    '#title' => t("Valor del 'item title' "),
    '#type' => 'textfield',
    '#description' => 'Valor que corresponde con el asignado en el Dapper.',
    '#required' => 1,
  );
    $form['num_items'] = array(
    '#title' => t("Cantidad de 'item text' "),
    '#type' => 'textfield',
    '#description' => 'Número de items a crear (sin contar el del title).',
    '#required' => 1,
  );
  $form_state['no buttons'] = TRUE; 
}

function fuentedapper_wizard_item_title_form_validate(&$form, &$form_state){
  if (!is_numeric($form_state['values']['num_items'])){
    form_set_error('num_items', t('El valor de este campo ha de ser numérico.'));
  }
}

function fuentedapper_wizard_item_title_form_submit(&$form, &$form_state){
  $form_state['fuentedapper_obj']->field_fuentedapper_extraargs[0]['value'] = '&'.'extraArg_title='.$form_state['values']['title'];
  $form_state['fuentedapper_obj']->num_items = $form_state['values']['num_items'];
}



/**
 * Formulario completar los argumentos (extArgs)
 */
function fuentedapper_wizard_extargs_form(&$form, &$form_state){
   $num_args = $form_state['fuentedapper_obj']->num_items;
  for($i = 0; $i < $num_args; $i++){
    $arg = 'args'.$i;
    $form[$arg] = array(
      '#title' => t("Nombre del 'item text' @num", array('@num' => $i + 1)),
      '#type' => 'fieldset',
    );
    $form[$arg]['arg_name'.$i] = array(
      '#type' => 'textfield',
      '#size' => 25,
      '#description' => t('Corresponde con el campo verificado de item text del Dapper.'),
      '#required' => 1,
    );
  }
  $form_state['no buttons'] = TRUE; 
}

function fuentedapper_wizard_extargs_form_submit(&$form, &$form_state){
  
  foreach ($form_state['values'] as $campo=>$valor){
    if (preg_match('/^arg_name([0-9]*)/', $campo)){
      $extraargs.='&extraArg_description[]='.$valor;
    }
  }
  $form_state['fuentedapper_obj']->field_fuentedapper_extraargs[0]['value'].= $extraargs;
}

 

/**
 * Formulario para que introduzcan el número de filtros que tiene el Dapper
 */
function fuentedapper_wizard_num_args_form(&$form, &$form_state){
  $form['num_args'] = array(
    '#title' => t('Número de filtros'),
    '#type' => 'textfield',
    '#description' => t('Valor numérico que define la cantidad de filtros (cajas de busqueda o despelgables) asociados al Dapper.'),
    '#required' => 1,
  );
  $form_state['no buttons'] = TRUE; 
}

function fuente_parametrizable_num_args_form_validate(&$form, &$form_state){
  if (!is_numeric($form_state['values']['num_args'])){
    form_set_error('num_args', t('El valor de este campo ha de ser numérico.'));
  }
}

function fuentedapper_wizard_num_args_form_submit(&$form, &$form_state){
  $form_state['fuentedapper_obj']->num_args = $form_state['values']['num_args'];
}


/**
 *Formulario para que introduzcan los nombres de los argumentos.
 **/
function fuentedapper_wizard_args_form(&$form, &$form_state){
  $num_args = $form_state['fuentedapper_obj']->num_args;
  for($i = 0; $i < $num_args; $i++){
    $arg = 'args'.$i;
    $form[$arg] = array(
      '#type' => 'fieldset',
      '#title' => t('Filtro <strong>@num</strong>', array('@num' => $i + 1)),
    );
    $form[$arg]['arg_name'.$i] = array(
      '#title' => t("Nombre del filtro"),
      '#type' => 'textfield',
      '#size' => 15,
      '#required' => TRUE,
    );
    $form[$arg]['arg_check'.$i] = array(
      '#type' => 'checkbox', 
      '#title' => t("¿Es una lista de valores (desplegable)?"),
    ); 
    $form[$arg]['arg_desc'.$i] = array(
      '#title' => t("Descripción del filtro"),
      '#type' => 'textfield',
      '#size' => 30,
    );
  }
  $form_state['no buttons'] = TRUE; 
}

function fuentedapper_wizard_args_form_submit(&$form, &$form_state){
  $num_args = $form_state['fuentedapper_obj']->num_args;
  if ($num_args > 0){
    for($i = 0; $i < $num_args; $i++){
      if( $form_state['values']['arg_check'.$i] == 1){
      $form_state['fuentedapper_obj']->field_fuentedapper_filtrosnombre[]['value'] = $form_state['values']['arg_name'.$i];
      $form_state['fuentedapper_obj']->field_fuentedapper_filtrosdesc[]['value'] = $form_state['values']['arg_desc'.$i];
      }
      else{
        $form_state['fuentedapper_obj']->field_fuentedapper_args[]['value'] = $form_state['values']['arg_name'.$i];
        $form_state['fuentedapper_obj']->field_fuentedapper_args_desc[]['value'] = $form_state['values']['arg_desc'.$i];
      }
    }
  }
}

function fuentedapper_wizard_desp_form(&$form, &$form_state){
  
  //Controlar que hay filtros con listados
  if($form_state['fuentedapper_obj']->field_fuentedapper_filtrosnombre){
  
    foreach( $form_state['fuentedapper_obj']->field_fuentedapper_filtrosnombre as $value ){
      $form['desplegable'][$value['value']] = array(
      '#title' => t('Listado de valores para el filtro de '.$value['value']),
      '#type' => 'textarea',
      '#required' => 1,
      '#description' => t('Para crear una lista de tipo desplegable, tienes que crear por cada linea una clave/valor (separados por la barra del 7) como en el siguiente ejemplo:<br><br> 0/Iruña<br>1/Donosti<br/>2/Vitoria'),
     );
    $form_state['no buttons'] = TRUE; 
    }
  }
  
}

function fuentedapper_wizard_desp_form_submit(&$form, &$form_state){
    //Controlar que hay filtros con listados
  if($form_state['fuentedapper_obj']->field_fuentedapper_filtrosnombre){
    foreach( $form_state['fuentedapper_obj']->field_fuentedapper_filtrosnombre as $key => $value ){
      //$nom_filtro = $value['value'];
      //$valor_textarea = $form_state['values'][$value['value']];
      //$textarea_lin = explode("\n", $valor_textarea);
      //$union =array($nom_filtro =>$textarea_lin);
      //$objeto_textarea = serialize($union);
      //
      //$form_state['fuentedapper_obj']->field_fuentedapper_desplegable[]['value'] = $objeto_textarea;
      $form_state['fuentedapper_obj']->field_fuentedapper_desplegable[]['value'] = $form_state['values'][$value['value']];
    }
  }
}


/*
function preview_test_modal_callback($js = FALSE){
 $output = t('<p>Lorem ipsum dolor sit amet...</p>');

  if ($js) {
    ctools_include('ajax');

    $commands = array();
    $commands[] = ctools_ajax_command_after('#test-ajax-link', $output);
    $commands[] = ctools_ajax_command_remove('#test-ajax-link');

    ctools_ajax_render($commands);
    // above command will exit().
  }
  else {
    return $output;
  }
} 
*/
