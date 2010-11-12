<?php 

/**
 * Llamada para crear el formulario multistep
 */
function canal_wizard() {
  $step = arg(2);
  ctools_include('wizard');
  ctools_include('object-cache');
  $form_info = array(
    'id' => 'supercanal_basic',
    'path' => 'crear/canal-supercanal/%step',
    'show trail' => TRUE,
    'show back' => TRUE,
    'show cancel' => TRUE,
    'show return' => FALSE,
    'next text' => t('Siguiente paso'),
    'next callback' =>  'canal_next',
    'finish callback' => 'canal_finish',
    'cancel callback' => 'canal_cancel',
    'order' => array(
      'datos_canal' => t('Datos del canal'),
      'elegir_fuente' => t('Elegir fuente'),
      'valor_args' => t('Valor'), 
      'previsualizar' => t("Previsualizar")
    ),
    'forms' => array(
      'datos_canal' => array(
        'form id' => 'datos_canal_form',
      ),
      'elegir_fuente' => array(
        'form id' => 'elegir_fuente_form',
      ),
      'valor_args' => array(
        'form id' => 'valor_args_form'
      ),
      'previsualizar' => array(
        'form id' => 'previsualizar_form'
      ),
    ),
  );
  $form_state = array(
    'cache name' => NULL,
  );
  $canal = ctools_object_cache_get('canal_obj', NULL);
  if (!$canal) {
    global $user;
    $canal = new stdClass();
    $canal->type = 'canal_de_supercanal';
    $canal->uid = $user->uid;
    $canal->status = 1;
    ctools_object_cache_set('canal_obj', NULL, $canal);
    $step = current(array_keys($form_info['order']));
  }
  $form_state['canal_obj'] = $canal;
  $output = ctools_wizard_multistep_form($form_info, $step, $form_state);
  return $output;
}

/**
 * Datos generales del canal
 */
function datos_canal_form(&$form, &$form_state) {
  $form['titulo'] = array(
    '#type' => 'textfield',
    '#title' => t('Nombre del canal'),
    '#required' => TRUE,
  );
  //Ahora se podran crear dos tipo de super canal dapper, pipe
  $form['tipo'] = array(
    '#title' => t('Tipo de fuente (dapper o yahoo pipes):'),
    '#type' => 'select',
    '#required' => TRUE,
    '#options' => array('yahoo pipes', 'dapper'),
    '#default_value' => 2,
  );
  if (og_get_group_context()) {
    //Obtener el ID del grupo en el que se esta
    $id_categoria = db_result(db_query("SELECT og.vid FROM {og_vocab} og WHERE  og.nid=%s", og_get_group_context()->nid));
    
    //Funcion del modulo taxonomy que dado un el id de una categoria devuelve todos los terminos de la misma
    $categorias=taxonomy_get_tree($id_categoria);
    $form['cat'] = array(
      '#type' => 'fieldset',
      '#title' => t('Categor&iacute;as'),
      '#description' => t('<u>Tienes que seleccionar al menos una categor&iacute;a.</u>'),
    );
    foreach ($categorias as $id => $contenido) {
      profundidad($contenido->tid);
      $pro=variable_get('profundidad_valor', 0);
      $form['cat'][$contenido->tid] = array(
        '#required' => TRUE,
        '#type' => 'checkbox',
        '#prefix' => '<div class=taxo'. $pro .'>',
        '#suffix' => '</div>', 
        '#title' => $contenido->name,
      );
    }
  }
}

function datos_canal_form_validate(&$form, &$form_state) {
  //Validar categorias
  $categoria=0;
  foreach ($form_state['values'] as $tid => $cont) {
    if ($cont!=0 and $tid!='numero_rss') {
      $categoria=1;
    }
  }
  if ($categoria==0) {
    form_error($form['cat'], 'Debes seleccionar al menos una categor&iacute;a.');
  }
}



function datos_canal_form_submit(&$form, &$form_state) {
  $fuente = $form_state['canal_obj'];
  $fuente->title = $form_state['values']['titulo'];
  $fuente->tipo = $form_state['values']['tipo'];
  
  //alamacenar tipo de fuente en el campo fuente_canal del nodo
  if ($form_state['values']['tipo']==0) {
    $fuente->field_fuente_canal[0]['value']='PIPE';
  }
  elseif ($form_state['values']['tipo']==1) {
    $fuente->field_fuente_canal[0]['value']='Dapper';
  }
  
   //Almacenar taxonomias seleccionadas
  foreach ($form_state['values'] as $tid => $cont) {
    if ($cont!=0 and $tid!='numero_rss') {
      $fuente->taxonomy[$tid] = taxonomy_get_term($tid);
    }
  }
  $form_state['yql_obj'] = $fuente;
}

/**
 * Formulario para elegir la fuente del canal
 */
function elegir_fuente_form(&$form, &$form_state) {
  //obtenemos el tipo de supernacal 
  $tip = $form_state['canal_obj']->tipo;
  //si es 0 es yahoopipes si es uno es dapper y lo asociamos a su tipo de contenidos
  $tipo = ($tip == 0) ? 'supercanal' : 'fuentedapper';
  $grupo = og_get_group_context();
  //Obtenemos las Ãºltimas fuentes parametrizables y construÃ­mos un array con las opciones teniendo en cuenta el tipo n
  $result = db_query("SELECT n.nid, title FROM {node} n INNER JOIN {og_ancestry} oa ON n.nid = oa.nid WHERE n.type = '". $tipo ."' AND oa.group_nid = %d ORDER BY n.created DESC", $grupo->nid);
  while ($row = db_fetch_object($result)) {
    $opciones[$row->nid] = $row->title;
  }
  $form['super'] = array(
    '#title' => t('Fuentes disponibles'),
    '#type' => 'fieldset',
  );
  if (!$opciones) {
    $form['super']['fuente'] = array(
      '#type' => 'item',
      '#value' => t('No existen fuentes.'),
    );
    drupal_set_message('No existen fuentes creadas.', 'status');
    drupal_goto('vigilancia');
  }
  else{
  $form['super']['fuente'] = array(
    '#type' => 'select',
    '#title' => t('Elige una fuente'),
    '#options' => $opciones,
  );
  }
}

function elegir_fuente_form_submit(&$form, &$form_state) {
  $form_state['canal_obj']->supercanal_nid = $form_state['values']['fuente'];
  
  //Almacenar el titulo de la fuente en el campo de fuentetitulo_canal del nodo
  $nod=node_load($form_state['values']['fuente']);
  $form_state['canal_obj'] ->field_nombrefuente_canal[0]['value']=$nod->title;
  
 
}

/**
 * Formulario para pasarle un valor a los parÃ¡metros
 */
function valor_args_form(&$form, &$form_state) {
  //obtenemos el tipo de supernacal 
  $typ = $form_state['canal_obj']->tipo;
  if ($typ == 0 ) { 
    $supercanal = node_load($form_state['canal_obj']->supercanal_nid);
    $form_state['canal_obj']->supercanal = $supercanal;
    $form['filtros'] = array(
      '#type' => 'fieldset',
      '#title' => t('Valor de los argumentos'),
    ); 
    foreach ($supercanal->field_supercanal_args as $id => $arg) {
      $form['filtros'][$arg['value']] = array(
        '#type' => 'textfield',
        '#title' => t('Valor del par&aacute;metro  @val', array('@val' => $arg['value'])),
        '#description' => $supercanal->field_supercanal_args_desc[$id]['value'],
      );
    }
  }
  
  //Supercanal Dapper
  else{
    $supercanal = node_load($form_state['canal_obj']->supercanal_nid);
    $form_state['canal_obj']->supercanal = $supercanal;
    
    if ($supercanal->field_fuentedapper_args) {
      //Filtros de caja de texto
      $form['filtros_texto'] = array(
        '#type' => 'fieldset',
        '#title' => t('Filtros de caja de texto'),
      ); 
      foreach ($supercanal->field_fuentedapper_args as $id => $arg) {
        $form['filtros_texto']['filtros'][$arg['value']] = array(
          '#type' => 'textfield',
          '#title' => t('Valor del par&aacute;metro  @val', array('@val' => $arg['value'])),
          '#description' => $supercanal->field_fuentedapper_args_desc[$id]['value'],
        );
      }
    }
    if ($supercanal->field_fuentedapper_filtrosnombre) {
      //Filtros de listas de valores
      $form['filtros_lista'] = array(
        '#type' => 'fieldset',
        '#title' => t('Filtros de seleccion de valor'),
      );
      foreach ($supercanal->field_fuentedapper_filtrosnombre as $id => $arg) {
        //Crear array con las opciones de la lista
        $lista = array();
        $elementos = $supercanal->field_fuentedapper_desplegable[$id]['value'];
        $lineas = explode("\n", $elementos);
        for ( $i=0; $i < count($lineas); $i++ ) {
          $palabras = explode("/", $lineas[$i]);
          $lista[$palabras[0]]= $palabras[1]; 
        }
        $form['filtros_lista']['filtros'][$arg['value']] = array(
          '#type' => 'select',
          '#title' => t('Valor del par&aacute;metro  @val', array('@val' => $arg['value'])),
          '#options' => $lista,
          '#description' => $supercanal->field_fuentedapper_filtrosdesc[$id]['value'],
        );
      }
    }
  } 
}

/**
 * Parece ser que el unserialize falla como una escopeta de feria y la peÃa ofrece esta soluciÃn (Mael NO USO)
 */
function mb_unserialize($serial_str) { 
  $out = preg_replace('!s:(\d+):"(.*?)";!se', "'s:'.strlen('$2').':\"$2\";'", $serial_str ); 
  return unserialize($out); 
}


/**
 *
 */
function valor_args_form_submit(&$form, &$form_state) {
  //Supercanal pipe
  if ($form_state['canal_obj']->tipo == 0) {
    foreach ($form_state['canal_obj']->supercanal->field_supercanal_args as $arg) {
      $form_state['canal_obj']->$arg['value'] = urlencode($form_state['values'][$arg['value']]);
    }
  }
  //Supercanal Dapper
  else{
    foreach ($form_state['values'] as $id=>$contenido){
      if($id!='previous' and $id!='op' and $id!='next' and $id!='cancel' and $id!='form_build_id' and$id!='form_token' and $id!='form_id' ){
      //$filtros.='&v_'.$id.'='.$contenido;
        $filtros.='&v_'.$id.'='.urlencode($contenido);

      }
    }
    $form_state['canal_obj']->filtros = $filtros;
    
  }
}

/**
 * Previsualizaci—n del canal.
 */
function previsualizar_form(&$form, &$form_state){
  
  $nodo = $form_state['canal_obj']->supercanal;
  //Supercanal Pipe
  if($form_state['canal_obj']->tipo == 0){
    $pipe_id = $nodo->field_supercanal_fuente[0]['value'];
    $pipe_params = '';
    foreach ($nodo->field_supercanal_args as $arg){
      $pipe_params .= '"'.$arg['value'].'":"'.$form_state['canal_obj']->$arg['value'].'",';
    }
    $output .='<script src="http://l.yimg.com/a/i/us/pps/listbadge_1.3.js">{"pipe_id":"'.$pipe_id.'","_btype":"list","pipe_params":{'.$pipe_params.'}}</script>';
    if(strpos($pipe_params,'+')){
      $form['mensaje'] = array(
      '#type' => 'item',
      '#value' => 'La previsualizaci&oacute;n no puede mostrarse, pero el PIPE puede ser alamacenado correctamente.',
    );
    }
    else{
      $form['valor_param'] = array(
      '#type' => 'item',
      '#value' => $output
    );
    }
    
  }
  //Supercanal Dapper
  else{
    
    $output ='http://open.dapper.net/transform.php?dappName='.$nodo->title.'&transformer='.$nodo->field_fuentedapper_fuente[0]['value'].
    $nodo->field_fuentedapper_extraargs[0]['value'].$form_state['canal_obj']->filtros;
    $enlace="<A HREF=".$output." TARGET='_blank'>Previsualizar en ventana nueva</A>";
    $form_state['canal_obj']->auxenlace=$output;
    $form['valor_param'] = array(
      '#type' => 'item',
      '#value' => $enlace,
      '#prefix' => '<div class="rss-dapper">',
      '#suffix' => '</div>',

    ); 
    
  }
}


function previsualizar_form_validate(&$form, &$form_state){
  
  //form_error('Debes seleccionar al menos una categor&iacute;a.');
  //Supercanal Pipe
  $nodo = $form_state['canal_obj']->supercanal;
  
  //Supercanal Pipe
  if($form_state['canal_obj']->tipo == 0){
    foreach ($nodo->field_supercanal_args as $arg){
      $params .= $arg['value'].'='.$form_state['canal_obj']->$arg['value'].'&';
    }
    $params = substr($params, 0, strlen($params)-1);
    $pipe_id = $nodo->field_supercanal_fuente[0]['value']; 


    $url ='http://pipes.yahoo.com/pipes/pipe.run?_id='.$pipe_id.'&_render=rss&'.$params;

    
    $dxml = file_get_contents($url);
    $dxml1 = file_get_contents($url);
    
    $data = simplexml_load_string($dxml);
    
    $sets = $data->channel->item;
    $all = sizeof($sets);
    
    $data1 = simplexml_load_string($dxml1);
    
    $sets1 = $data1->channel->item;
    $all1 = sizeof($sets1);
 

    for($i=0;$i<$all;$i++){
      if($i+2==$all){
        if(md5($sets[$i]->link)==md5($sets1[$i]->link)){
        }
        else{
          
          drupal_set_message('No se ha podido crear el canal: '.$form_state['canal_obj']->title.'. La fuente PIPE no se ha podido procesar.', 'error');
          drupal_goto('vigilancia');
        }
        
      }
    } 
    
  }
}

/**
 *
 */
function previsualizar_form_submit(&$form, &$form_state){
  $nodo = $form_state['canal_obj']->supercanal;
  
  //Supercanal Pipe
  if($form_state['canal_obj']->tipo == 0){
    foreach ($nodo->field_supercanal_args as $arg){
      $params .= $arg['value'].'='.$form_state['canal_obj']->$arg['value'].'&';
    }
    $params = substr($params, 0, strlen($params)-1);
    $pipe_id = $nodo->field_supercanal_fuente[0]['value']; 
    $form_state['canal_obj']->feeds = array('FeedsHTTPFetcher' => array('source' => 'http://pipes.yahoo.com/pipes/pipe.run?_id='.$pipe_id.'&_render=rss&'.$params));
  }
  //Supercanal Dapper
  else{
    $form_state['canal_obj']->feeds = array('FeedsHTTPFetcher' => array('source' =>  $form_state['canal_obj']->auxenlace));
  }
}

/**
 *
 */
function canal_next(&$form_state){
  $canal = $form_state['canal_obj'];
  $cache = ctools_object_cache_set('canal_obj', $form_state['cache name'], $canal);
}

/**
 *
 */
function canal_finish(&$form_state){
  node_save($form_state['canal_obj']);
  $canal = $form_state['canal_obj'];
  $form_state['redirect'] = 'node/'.$canal->nid;
  ctools_object_cache_clear('canal_obj', $form_state['cache name']);
}

/**
 *
 */
function canal_cancel(&$canal){
  //$form_state['redirect'] = 'node';
  drupal_goto('vigilancia');
  ctools_object_cache_clear('canal_obj', $form_state['cache name']);
}
