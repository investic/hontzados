<?php // $Id$

/**
 * @file
 * Main template file
 *
 * @see template_preprocess_page(), preprocess/preprocess-page.inc
 * http://api.drupal.org/api/function/template_preprocess_page/6
 */
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
  <html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $language->language; ?>" xml:lang="<?php print $language->language; ?>">
    <head>
      <?php print $head; ?>
      <title><?php print $head_title; ?></title>
      <?php print $styles; ?>
      <?php print $ie_styles; ?>
      <?php print  $scripts; ?>
    </head>
  <body<?php print $body_attributes; ?>>
  <?php if (!empty($admin)) print $admin; // support for: http://drupal.org/project/admin ?>
  <div id="wrapper">
   <div<?php print $header_attributes; ?>> 
      
        <div id="uno">
         <?php if ($site_name): ?>
        <div id="site-name"> <a href="<?php print $front_page; ?>" title="<?php print $site_name; ?>"><?php print $site_name; ?></a> </div>
        <?php endif; ?>
        <?php if ($site_slogan): ?>
          <div id="site-slogan"><?php print $site_slogan; ?></div>
        <?php endif; ?>
        <?php if ($logo): ?>
        <a href="<?php print $front_page; ?>" title="<?php print $site_name; ?>" id="logo"><img src="<?php print $logo; ?>" alt="<?php if ($site_name): print $site_name;  endif; ?>" /></a>
        <?php endif; ?>            
        </div>
       <div id="dos">
         <div id="menutop"><?php print $menuuser; ?></div>
       </div>
       <div id="tres">
        <?php if ($nombredelgrupo): ?>
         <div id="gruponame"><?php print $nombredelgrupo; ?></div>
        <?php endif; ?>
       </div>
       
    </div>
    <?php if ($menutop): ?>
      <div id="navigation"><?php print $menutop; ?></div>
    <?php endif; ?>
<div id="color" class="azul"></div>
    <div id="color" class="azul-2"></div>
    <div id="container" class="layout-region">
  