<?php

/**
 * Llamada para crear el formulario multistep
 */
function yql_wizard(){
  $step = arg(2);
  ctools_include('wizard');
  ctools_include('object-cache');
  $form_info = array(
    'id' => 'yql_basic',
    'path' => 'crear/canal-yql/%step',
    'show trail' => TRUE,
    'show back' => TRUE,
    'show cancel' => true,
    'show return' =>false,
    'next text' => t('Siguiente paso'),
    'next callback' =>  'yql_next',
    'finish callback' => 'yql_finish',
    'cancel callback' => 'yql_cancel',
    'order' => array(
      'yql_canal' => t('Datos del canal'),
      'agregar_fuente' => t('Agregar fuente'),
      'filtros' => t('Filtros'), 
      'previsualizar' => t("Previsualizar")
    ),
    'forms' => array(
      'yql_canal' => array(
        'form id' => 'yql_canal_form',
      ),
      'agregar_fuente' => array(
        'form id' => 'agregar_fuente_form',
      ),
      'filtros' => array(
        'form id' => 'filtros_form'
      ),
      'previsualizar' => array(
        'form id' => 'previsualizar_form'
      ),
    ),
  );
  $form_state = array(
    'cache name' => NULL,
  );
  $canal = ctools_object_cache_get('yql_obj', NULL);
  if (!$canal){
    global $user;
    $canal = new stdClass();
    $canal->type = 'canal_de_yql';
    $canal->uid = $user->uid;
    $canal->status = 1;
    $canal->field_fuente_canal[0]['value']='YQL';
    ctools_object_cache_set('yql_obj', NULL, $canal);
    $step = current(array_keys($form_info['order']));
  }
  $form_state['yql_obj'] = $canal;
  $output = ctools_wizard_multistep_form($form_info, $step, $form_state);
  return $output;
}

/**
 * Datos generales del canal
 */
function yql_canal_form(&$form, &$form_state){
  $form['titulo'] = array(
    '#type' => 'textfield',
    '#title' => t('Nombre del canal'),
    '#required' => TRUE,
  );
  $form['numero_rss'] = array(
    '#title' => t('Cantidad de RSS.'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#description' => 'N&uacute;mero de RSS sobre los que realizar el filtrado.',
    '#default_value' => 1,
    '#size' => 10,
  );
 
  if(og_get_group_context()){
    //Obtener el ID del grupo en el que se esta
    $id_categoria = db_result(db_query("SELECT og.vid FROM {og_vocab} og WHERE  og.nid=%s",og_get_group_context()->nid));
    
    //Funcion del modulo taxonomy que dado un el id de una categoria devuelve todos los terminos de la misma
    $categorias=taxonomy_get_tree($id_categoria);
    $salida = array();
    $form['cat'] = array(
      '#type' => 'fieldset',
      '#title' => t('Categor&iacute;as'),
      '#description' => t('<u>Tienes que seleccionar al menos una categor&iacute;a.</u>'),
    );
    foreach ($categorias as $id => $contenido){
      profundidad($contenido->tid);
      $pro=variable_get('profundidad_valor',0);
      $form['cat'][$contenido->tid] = array(
        '#required' => TRUE,
        '#type' => 'checkbox',
        '#prefix' => '<div class=taxo'.$pro.'>',
        '#suffix' => '</div>', 
        '#title' => $contenido->name,
      );
    }
  }


  $form_state['no buttons'] = TRUE; 
}

function yql_canal_form_validate(&$form, &$form_state){
  if(!is_numeric($form_state['values']['numero_rss'])){
    form_error($form['numero_rss'],'El valor debe ser num&eacute;rico.');
  }
  if($form_state['values']['numero_rss']<1 or $form_state['values']['numero_rss']>100){
    form_error($form['numero_rss'],'El valor debe estar comprendido entre 1-100.');
  }
  //Validar categorias
  $categoria=0;
  foreach($form_state['values'] as $tid => $cont){
    if($cont!=0 and $tid!='numero_rss'){
      $categoria=1;
    }
  }
  if($categoria==0){
    form_error($form['cat'],'Debes seleccionar al menos una categor&iacute;a.');
  }
}

function yql_canal_form_submit(&$form, &$form_state){
  $fuente = $form_state['yql_obj'];
  $fuente->title = $form_state['values']['titulo'];
  $fuente->numero_rss = $form_state['values']['numero_rss'];
  
  //Almacenar taxonomias seleccionadas
  foreach($form_state['values'] as $tid => $cont){
    if($cont!=0 and $tid!='numero_rss'){
      $fuente->taxonomy[$tid] = taxonomy_get_term($tid);
    }
  }
  $form_state['yql_obj'] = $fuente;
}

/**
 * Formulario donde a–adimos las fuentes RSS
 */
function agregar_fuente_form(&$form, &$form_state){
  //obtenemos cantidad de rss
  $num_rss = $form_state['yql_obj']->numero_rss;
  
  $form['fuentes'] = array(
    '#title' => t('Agregar fuentes RSS'),
    '#type' => 'fieldset',
  );
  for($i=1;$i<=$num_rss;$i++){
    $form['fuentes'][$i] = array(
      '#title' => t('RSS '.$i),
      '#type' => 'textfield',
      '#description' => 'RSS de la fuente.',
      '#size' => 50,
    );
  }
}

function agregar_fuente_form_submit(&$form, &$form_state){
  foreach ($form_state['values'] as $campo=>$valor){
    if (is_numeric($campo) and $valor){
      $rss[$campo].=$valor;
    }
  }
  $form_state['yql_obj']->campos_rss= $rss;
  $form_state['yql_obj']->field_nombrefuente_canal[0]['value'] = implode(', ',$form_state['yql_obj']->campos_rss);
}


/**
 * Formulario donde incluir los filtros.
 */
function filtros_form(&$form, &$form_state){
  if($form_state['yql_obj']->campos_rss){
    $form['info'] = array(
      '#type' => 'item',
      '#value' => t('<b>Para obtener todos los contenidos, no rellenar ning&uacute;n filtro.<br> En cambio si quieres filtrar el contenido, s&oacute;lo puedes aplicar uno de los cuatro filtros disponibles.</b>'),
    );
    $form['filtros1'] = array(
      '#title' => t('Aplicar filtro 1 a las fuentes RSS'),
      '#type' => 'fieldset',
      '#description' => t(''),
    );
    $form['filtros1']['todos'] = array(
      '#title' => t('Busqueda general'),
      '#type' => 'textfield',
      '#size' => 20,
      '#description' => t('Se mostraran solo aquellos contenidos que coincidan en el titulo o descripcion con el termino de filtrado.'),
    );
    $form['filtros2'] = array(
      '#title' => t('Aplicar filtro 2 a las fuentes RSS'),
      '#type' => 'fieldset',
      '#description' => t(''),
    );
    $form['filtros2']['titulo'] = array(
      '#title' => t('Contiene esta palabra en el t&iacute;tulo'),
      '#type' => 'textfield',
      '#size' => 20,
    );
    $form['filtros2']['cbox1'] = array(
      '#type' => 'radios',
      '#default_value' => 0,
      '#options' => array(t('OR'), t('AND')),
    );
    $form['filtros2']['descripcion'] = array(
      '#title' => t('Contiene esta palabra en la descripci&oacute;n'),
      '#type' => 'textfield',
      '#size' => 20,
    );
    $form['filtros3'] = array(
      '#title' => t('Aplicar filtro 3 a las fuentes RSS'),
      '#type' => 'fieldset',
      '#description' => t(''),
    );
    $form['filtros3']['no_titulo'] = array(
      '#title' => t('No contiene esta palabra en el t&iacute;tulo'),
      '#type' => 'textfield',
      '#size' => 20,
    );
    $form['filtros3']['cbox2'] = array(
      '#type' => 'radios',
      '#default_value' => 0,
      '#options' => array(t('OR'), t('AND')),
    );
    $form['filtros3']['no_descripcion'] = array(
      '#title' => t('No contiene esta palabra en la descripci&oacute;n'),
      '#type' => 'textfield',
      '#size' => 20,
    );
    $form['filtros4'] = array(
      '#title' => t('Aplicar filtro 4 a las fuentes RSS'),
      '#type' => 'fieldset',
      '#description' => t(''),
    );
    $form['filtros4']['contiene'] = array(
      '#title' => t('Contiene esta palabra en el/la '),
      '#type' => 'textfield',
      '#size' => 20,
    );
    $form['filtros4']['select1'] = array(
      '#type' => 'radios',
      '#default_value' => 0,
      '#options' => array(t('T&iacute;tulo'), t('Descripci&oacute;n')),
    );
    $form['filtros4']['con'] = array(
      '#title' => t('Anidaci&oacute;n de filtros'),
      '#type' => 'fieldset',
    );
    $form['filtros4']['con']['cbox3'] = array(
      '#type' => 'radios',
      '#default_value' => 0,
      '#options' => array(t('OR'), t('AND')),
    );
    $form['filtros4']['no_contiene'] = array(
      '#title' => t('No contiene esta palabra el/la'),
      '#type' => 'textfield',
      '#size' => 20,
    );
    $form['filtros4']['select2'] = array(
      '#type' => 'radios',
      '#default_value' => 1,
      '#options' => array(t('T&iacute;tulo'), t('Descripci&oacute;n')),
    );
    
    //textarea entrada manual de filtros
    $form['filtros5'] = array(
      '#title' => t('Aplicar filtro 5 a las fuentes RSS'),
      '#type' => 'fieldset',
      '#description' => t('Al habilitar el filtro 5 el resto de filtros se desactivan, y es responsabilidad del usuario su correcta creaci&oacute;n.<br>')
      .'<A HREF="http://developer.yahoo.com/yql/guide/index.html" target="_blank"><b>Manual YQL</b></A>' ,
    );
    $form['filtros5']['checkarea'] = array(
      '#type' => 'checkbox',
      '#title' => t('Habilitar para crear filtros manulamente'),
      '#prefix' => '<div class="filtro-manual">',
      '#suffix' => '</div>',
    );
    $form['filtros5']['area'] = array(
      '#title' => t('C&oacute;digo para crear el filtro manualmente'),
      '#type' => 'textarea',
      '#default_value' => 'and channel.item.title like "%Android%"',
      '#disabled' => false,
      '#prefix' => '<div class="area-filtro-manual">',
      '#suffix' => '</div>',
      '#weight' => 5,
      '#description' =>t('Ej: and (channel.item.title like "%Android%" and channel.item.description not like "%HTC%"'),
    );
  }
  else{
    $form['text'] = array(
      '#title' => t('No existe RSS'),
      '#type' => 'item',
      '#size' => 20,
    );
    drupal_set_message('No ha agregado ningun RSS sobre el que aplicar filtros.','status');
  }
}

function filtros_form_validate(&$form, &$form_state){
  if($form_state['values']['todos'] and
      ($form_state['values']['titulo'] or $form_state['values']['descripcion']
      or $form_state['values']['no_titulo'] or $form_state['values']['no_descripcion']
      or $form_state['values']['contiene'] or $form_state['values']['no_contiene'])){
    form_error($form,'Demasiados filtros, quite terminos de alguno.'); 
  }
  elseif(($form_state['values']['titulo'] or $form_state['values']['descripcion']) and
      ($form_state['values']['no_titulo'] or $form_state['values']['no_descripcion'] or $form_state['values']['todos']
       or $form_state['values']['contiene'] or $form_state['values']['no_contiene'])){
    form_error($form,'Demasiados filtros, quite terminos de alguno.');
  }
  elseif(($form_state['values']['no_titulo'] or $form_state['values']['no_descripcion']) and
      ($form_state['values']['titulo'] or $form_state['values']['descripcion'] or $form_state['values']['todos']
       or $form_state['values']['contiene'] or $form_state['values']['no_contiene'])){  
    form_error($form,'Demasiados filtros, quite terminos de alguno.');
  }
  elseif(($form_state['values']['contiene'] or $form_state['values']['no_contiene']) and
      ($form_state['values']['titulo'] or $form_state['values']['descripcion'] or $form_state['values']['todos']
       or $form_state['values']['no_titulo'] or $form_state['values']['no_descripcion'])){  
    form_error($form,'Demasiados filtros, quite terminos de alguno.');
  }
}

function filtros_form_submit(&$form, &$form_state){

  $filtros=array();
  
  if($form_state['values']['checkarea']){
    $filtros['area'].=$form_state['values']['area'];
  }
  else{
    
    //Filtro 1
    if($form_state['values']['todos']){
      $filtros['todos'].=$form_state['values']['todos'];
    }
    //Filtro 2
    if($form_state['values']['titulo']){
      $filtros['titulo'].=$form_state['values']['titulo'];
    }
    if($form_state['values']['descripcion']){
      $filtros['descripcion'].=$form_state['values']['descripcion'];
    }
    //Filtro 3
    if($form_state['values']['no_titulo']){
      $filtros['no_titulo'].=$form_state['values']['no_titulo'];
    }
    if($form_state['values']['no_descripcion']){
      $filtros['no_descripcion'].=$form_state['values']['no_descripcion'];
    }
    //Filtro 4
    if($form_state['values']['contiene']){
      $filtros['contiene'].=$form_state['values']['contiene'];
    }
    if($form_state['values']['no_contiene']){
      $filtros['no_contiene'].=$form_state['values']['no_contiene'];
    }
    //Filtro 2
    $form_state['yql_obj']->filtrosSI = $form_state['values']['cbox1'];
    //Filtro 3
    $form_state['yql_obj']->filtrosNO = $form_state['values']['cbox2'];
    //Filtro 4
    $form_state['yql_obj']->campo_contiene = $form_state['values']['select1'];
    $form_state['yql_obj']->campo_no_contiene = $form_state['values']['select2'];
    $form_state['yql_obj']->conjuncion = $form_state['values']['cbox3'];
  }
  
  $form_state['yql_obj']->filtros= $filtros;
}

/**
 * Previsualizaci—n del canal YQL.
 */
function previsualizar_form(&$form, &$form_state){
  //Filtro manual
  if($form_state['yql_obj']->filtros['area']){
    $filter = $form_state['yql_obj']->filtros['area'];
  }
  //Resto de filtros
  else{
    //Filtro 1 todos
    if($form_state['yql_obj']->filtros['todos']){
      $filter=' and (channel.item.title like "%'.$form_state['yql_obj']->filtros['todos'].'%" or channel.item.description like "%'.$form_state['yql_obj']->filtros['todos'].'%")';
    }
    
    //Filtro 2 
    elseif($form_state['yql_obj']->filtros['titulo'] or $form_state['yql_obj']->filtros['descripcion']){
      if($form_state['yql_obj']->filtros['titulo'] and $form_state['yql_obj']->filtros['descripcion']){
        if($form_state['yql_obj']->filtrosSI==0){
          $f='or';
        }
        else{
          $f='and';
        }
        $filter=' and (channel.item.title like "%'.$form_state['yql_obj']->filtros['titulo'].'%" '.$f.' channel.item.description like "%'.$form_state['yql_obj']->filtros['descripcion'].'%")';
      }
      elseif($form_state['yql_obj']->filtros['titulo']){
        $filter=' and channel.item.title like "%'.$form_state['yql_obj']->filtros['titulo'].'%"';
      }
      elseif($form_state['yql_obj']->filtros['descripcion']){
        $filter=' and channel.item.description like "%'.$form_state['yql_obj']->filtros['descripcion'].'%"';
      }
    }
    
    //Filtro 3
    elseif($form_state['yql_obj']->filtros['no_titulo'] or $form_state['yql_obj']->filtros['no_descripcion']){
      if($form_state['yql_obj']->filtros['no_titulo'] and $form_state['yql_obj']->filtros['no_descripcion']){
        if($form_state['yql_obj']->filtrosNO==0){
          $f='or';
        }
        else{
          $f='and';
        }
        $filter=' and (channel.item.title not like "%'.$form_state['yql_obj']->filtros['no_titulo'].'%" '.$f.' channel.item.description not like "%'.$form_state['yql_obj']->filtros['no_descripcion'].'%")';
      }
      elseif($form_state['yql_obj']->filtros['no_titulo']){
        $filter=' and channel.item.title like "%'.$form_state['yql_obj']->filtros['no_titulo'].'%"';
      }
      elseif($form_state['yql_obj']->filtros['no_descripcion']){
        $filter=' and channel.item.description like "%'.$form_state['yql_obj']->filtros['no_descripcion'].'%"';
      }
    }
    //Filtro 4
    elseif($form_state['yql_obj']->filtros['contiene'] or $form_state['yql_obj']->filtros['no_contiene']){
      //Tipo del contiene
      if($form_state['yql_obj']->campo_contiene==0){
          $name1='title';
        }
        else{
          $name1='description';
        }
        //Tipo del no_contiene
      if($form_state['yql_obj']->campo_no_contiene==0){
          $name2='title';
        }
        else{
          $name2='description';
        }
      if($form_state['yql_obj']->filtros['contiene'] and $form_state['yql_obj']->filtros['no_contiene']){
        if($form_state['yql_obj']->conjuncion==0){
          $f='or';
        }
        else{
          $f='and';
        }
        $filter=' and (channel.item.'.$name1.' like "%'.$form_state['yql_obj']->filtros['contiene'].'%" '.$f.' channel.item.'.$name2.' not like "%'.$form_state['yql_obj']->filtros['no_contiene'].'%")';
      }
      elseif($form_state['yql_obj']->filtros['contiene']){
        $filter=' and channel.item.'.$name1.' like "%'.$form_state['yql_obj']->filtros['contiene'].'%"';
      }
      elseif($form_state['yql_obj']->filtros['no_contiene']){
        $filter=' and channel.item.'.$name2.' not like "%'.$form_state['yql_obj']->filtros['no_descripcion'].'%"';
      }
    }
  }
  
  //Obtener fuentes RSS
  $data=$form_state['yql_obj']->campos_rss;
  $url ='http://query.yahooapis.com/v1/public/yql?q=';
  $query = "select channel.title, channel.link, channel.item.title, channel.item.link, channel.item.description, channel.item.guid from xml where url in('".implode("', '",$data)."')";
  $url.=urlencode($query.$filter.'LIMIT 20');
  
  $form['consulta'] = array(
    '#title' => t('Peticiones YQL con filtros'),
    '#type' => 'fieldset',
  );
  $form['consulta']['url'] = array(
    '#title' => t('Consulta YQL'),
    '#type' => 'item',
    '#value' => $url,
  );
  
  //Almacenar la url en una variable del objeto para poder utilizarla despues
  $form_state['yql_obj']->urlsearch = $url;
  $dxml = file_get_contents($url);
  if(!strpos($dxml,'yahoo:count="0"')){
    $data = simplexml_load_string($dxml);
    $sets = $data->results->rss;
    $all = sizeof($sets);
    $output;
    for($i=0;$i<$all;$i++){
      $r = $sets[$i];
      $title = $r->channel->title.'';
      if($title != $oldtitle){
        $output.='<div><h3><a href="'.($r->channel->link.'').'">'.($r->channel->title.'').'</a></h3><ul>';
      }
        $output.='<li><a href="'.($r->channel->item->link.'').'">'.($r->channel->item->title.'').'</a></li>';
      if($title != $sets[$i+1]->channel->title.''){
        $output.='</ul></div>';
      }
      $oldtitle = $r->channel->title.'';
    }
      $form['datos'] = array(
      '#title' => t('Resultado de la petici&oacute;n'),
      '#type' => 'fieldset',
    );
      $form['datos']['content'] = array(
      '#type' => 'item',
      '#value' => $output,
    );
  }
  else{
    $form['datos'] = array(
      '#title' => t('Resultado de la petici&oacute;n'),
      '#type' => 'fieldset',
    );
    $form['datos']['content'] = array(
      '#type' => 'item',
      '#value' => 'NO existen contenidos con estos filtros.',
    );
  }
  
}


function get($url){
  $ch = curl_init();
  curl_setopt($ch, CURLOPT_URL, $url);
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
  $output = curl_exec($ch);
  curl_close($ch);
  return $output;
}

 
function previsualizar_form_submit(&$form, &$form_state){
  $form_state['yql_obj']->feeds = array('FeedsHTTPFetcher' => array('source' =>  $form_state['yql_obj']->urlsearch));
}


function yql_next(&$form_state){
  $canal = $form_state['yql_obj'];
  $cache = ctools_object_cache_set('yql_obj', $form_state['cache name'], $canal);
}


function yql_finish(&$form_state){
  node_save($form_state['yql_obj']);
  $canal = $form_state['yql_obj'];
  $form_state['redirect'] = 'node/'.$canal->nid;
  ctools_object_cache_clear('yql_obj', $form_state['cache name']);
}


function yql_cancel(&$canal){
  //$form_state['redirect'] = 'node';
  drupal_goto('vigilancia');
  ctools_object_cache_clear('yql_obj', $form_state['cache name']);
}
