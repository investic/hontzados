<?php
// $Id: node.tpl.php,v 1.5 2010/06/06 09:51:29  Exp $
?>
<div id="node-<?php print $node->nid; ?>" class="node<?php if ($sticky) { print ' sticky'; } ?><?php if (!$status) { print ' node-unpublished'; } ?>">

  
  <?php if ($page == 0):?>
    <div id="flagtitulo">
        <div class="f-interesante"><?php print $node->links['flag-leido_interesante']['title']; ?></div>
        <div class="f-no-interesante"><?php print $node->links['flag-leido_no_interesante']['title']; ?></div>
        <div class="f-titulo"> <h2><a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2></div>
    </div>
    <?php if($node->body): ?>
      <div class="noticias-teaser-texto"><?php print drupal_substr(strip_tags($node->body), 0, 250).' ...'; ?></div>
    <?php endif; ?>
    
    
    <div id="i-contenedor">
         <?php //Bloque de FiveStar y comentarios ?>
      <div class="item-fivestar">
        <?php print $node->content['fivestar_widget']['#value'] ?>
        <?php if($node->comment_count): ?>
          <div class="items-coments">
            <?php if($node->comment_count==1): ?>
              <a href="/node/<?php print $node->nid ?>#comments" title="Ver comentario"><?php print t('Hay %num_coment comentario', array('%num_coment'=>$node->comment_count)) ?></a>
            <?php else: ?>
              <a href="/node/<?php print $node->nid ?>#comments" title="Ver comentarios"><?php print t('Hay %num_coment comentarios', array('%num_coment'=>$node->comment_count)) ?></a>
            <?php endif; ?>
          </div>
        <?php endif; ?>          
      </div>
    <?php //Fin de bloque de FiveStar y comentarios ?>
     <div id="ffc">
          <div class="item-fuente">
                  <a href="<?php print $node->feeds_node_item->guid; ?>" title="Ver fuente original" target= "_blank">Ver fuente original</a>
                </div>
                <div class="item-fecha">
                  <?php print format_date($node->created, 'custom','d/m/Y'); ?>
                </div>
     </div>  
          <?php //Tags ?>   
          <div class="item-categorias">
              <?php if(is_array($node->taxonomy)): ?>
              <span class="etiqueta-gris">Etiquetas:</span>
                <?php foreach($node->taxonomy as $etiqueta):?>
                  <?php print $etiqueta->name; ?>
                <?php endforeach; ?>
               
              <?php endif; ?>
           </div>
          <?php //Fin tags?>   
    </div>
   
    <div class="n-opciones-item"> 
        <?php //A–adir comentario ?>
          <div class="n-item-comentar">
            <a href="<?php print 'comment/reply/'.$node->nid.'#comment-form'?>" title="Comentar">Comentar</a>
          </div>
        <?php //Fin a–adir comentario ?>
        
        <?php //A–adir tag ?>
          <div class="n-item-etiquetar">
            <a href="<?php print 'node/'.$node->nid.'/tag'?>" title="Etiquetar">Etiquetar</a>
          </div>
        <?php //Fin a–adir tag ?>
          
        <?php //Borrar noticias ?>
          <div class="n-item-borrar">
            <a href="<?php print 'node/'.$node->nid.'/delete'?>" title="Borrar">Borrar</a>
          </div>
        <?php //Fin borrar noticias ?>
        
        <?php //Enviar debate ?>
          <div class="n-item-debate">
            <a href="<?php print 'node/add/debate/'.$node->nid?>" title="Enviar a debate" target= "_blank">Enviar a &Aacute;rea de debate</a>
          </div>
        <?php //Fin enviar debate ?>
        
        <?php //Enviar trabajo ?>
          <div class="n-item-trabajo">
            <a href="<?php print 'node/add/wiki/'.$node->nid ?>" title="Enviar a trabajo"target= "_blank">Enviar a &Aacute;rea de trabajo</a>
          </div>
        <?php //Fin enviar trabajo ?>
    </div>  
  <?php endif; ?>
 
  
  <?php if ($page == 1): ?>    
    <div id="flagtitulo">
      <div class="f-interesante"><?php print $node->links['flag-leido_interesante']['title']; ?></div>
      <div class="f-no-interesante"><?php print $node->links['flag-leido_no_interesante']['title']; ?></div>
      <div class="f-titulo"> <h2><?php print $title ?></h2></div>
  </div>

    <?php if($node->body): ?>
      <div class="noticias-full-texto"><?php print $node->body; ?></div>
    <?php endif; ?>
    
    <div class="item-fecha">
      <span class="etiqueta-gris">Fecha captura:</span>
      <?php print format_date($node->created, 'custom','d/m/Y'); ?>
    </div>
    
    <?php if($node->comment_count): ?>
      <div class="item-coments">
        <?php if($node->comment_count==1): ?>
          <a href="/node/<?php print $node->nid ?>#comments" title="Ver comentario"><?php print t('Hay %num_coment comentario', array('%num_coment'=>$node->comment_count)) ?></a>
        <?php else: ?>
          <a href="/node/<?php print $node->nid ?>#comments" title="Ver comentarios"><?php print t('Hay %num_coment comentarios', array('%num_coment'=>$node->comment_count)) ?></a>
        <?php endif; ?>
      </div>
    <?php endif; ?>
    
    <?php //Tags ?>   
    <div class="item-categorias">
        <?php if(is_array($node->taxonomy)): ?>
        <span class="etiqueta-gris">Etiquetas:</span>
          <?php foreach($node->taxonomy as $etiqueta):?>
            <?php print $etiqueta->name; ?>
          <?php endforeach; ?>
          
        <?php endif; ?>
    </div>
    <?php //Fin tags?>
    <?php //Obtener el grupo ?>
    <?php $grupo=og_get_group_context()->purl; ?>

  <div class="opciones-item">   
    <?php //A–adir comentario ?>
      <div class="item-comentar">
        <a href="<?php print '/comment/reply/'.$node->nid.'#comment-form'?>" title="Comentar">Comentar</a>
      </div>
    <?php //Fin a–adir comentario ?>
    
    
    <?php //Borrar noticias ?>
      <div class="item-borrar">
        <a href="<?php print '/'.$grupo.'/node/'.$node->nid.'/delete'?>" title="Borrar">Borrar</a>
      </div>
    <?php //Fin borrar noticias ?>
    
    <?php //Enviar debate ?>
      <div class="item-debate">
        <a href="<?php print '/'.$grupo.'/node/add/debate/'.$node->nid?>" title="Enviar a debate" target= "_blank">Enviar a &Aacute;rea de debate</a>
      </div>
    <?php //Fin enviar debate ?>
    
    <?php //Enviar trabajo ?>
      <div class="item-trabajo">
        <a href="<?php print '/'.$grupo.'/node/add/wiki/'.$node->nid ?>" title="Enviar a trabajo"target= "_blank">Enviar a &Aacute;rea de trabajo</a>
      </div>
    <?php //Fin enviar trabajo ?>
  </div>  
  <?php endif; ?>

</div>
