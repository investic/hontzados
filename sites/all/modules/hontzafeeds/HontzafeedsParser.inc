<?php
// $Id: HontzafeedParser.inc,v 1.1 2010/07/16 21:35:08 karlosgliberal Exp $

/**
 * @file
 * First module on hontza distribution
 */

/**
 * A parser for the yql 
 */
class HontzafeedsParser extends FeedsParser {
 
 /**
 * Implementation of FeedsParser::parse().
 */
 public function parse(FeedsImportBatch $batch, FeedsSource $source) {
    
  $xml = new SimpleXMLElement($batch->getRaw()); 
  $sets = $xml->results->rss;
  $all = sizeof($sets);
  for ($i=0; $i<$all; $i++) {
    $r = $sets[$i];
      foreach ($r->channel->item as $elemento) {
        $item['title'] = $elemento->title;
        $item['description'] = $elemento->description;
        $item['link'] = $elemento->link;
        $item['guid'] = $elemento->guid;
        $batch->addItem($item);
      }
    }
  }


  /**
   * Implementation of FeedsParser::getMappingSources().
   */
   
  public function getMappingSources() {
    return array(
      'title' => array(
        'name' => t('titulo'),
        'description' => t('titulo'),
      ),
       'description' => array(
        'name' => t('descripcion'),
        'description' => 'Esto es una descripcion',
      ),
       'link' => array(
        'name' => t('link'),
        'descripcion' => 'Link del item',
      ),
       'guid' => array(
        'name' => t('guid'),
        'descripcion' => 'guid del item',
      ),
    );
  }
}
