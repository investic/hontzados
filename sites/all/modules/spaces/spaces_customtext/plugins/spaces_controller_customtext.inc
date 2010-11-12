<?php
// $Id: spaces_controller_customtext.inc,v 1.1.2.1 2009/12/19 18:31:55 yhahn Exp $

/**
 * Customtext variable controller.
 */
class spaces_controller_customtext extends spaces_controller_variable {
  /**
   * Override of init_overrides().
   */
  function init_overrides() {
    parent::init_overrides();
    global $language;
    $language->language = spaces_customtext_cache();
  }
}
