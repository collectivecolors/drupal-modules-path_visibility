<?php
// $Id$
/**
 * @file
 * Contains the path argument validator plugin.
 */

/**
 * Argument validator plugin to validate path arguments.
 * 
 * Paths must be valid Drupal paths that return menu data.
 */
class path_visibility_plugin_argument_validate_path extends views_plugin {
  var $option_name = 'validate_path_argument';

  /**
   * Initialize this plugin with the view and the argument
   * it is linked to.
   */
  function init(&$view, &$argument, $id = NULL) {
    $this->view = &$view;
    $this->argument = &$argument;
    $this->id = $id;
  }

  /**
   * Determine if the administrator has the privileges to use this
   * plugin
   */
  function access() { 
    return TRUE; 
  }
  
  /**
   * We do not need form input for this validator.
   */
  function argument_form(&$form, &$form_state) {}
  function validate_form(&$form, &$form_state) {}
  function check_access(&$form) {}

  /**
   * Return the validate argument.
   */
  function get_argument() {
    return isset($this->argument->options[$this->option_name]) 
          ? $this->argument->options[$this->option_name] 
          : '';
  }

  /**
   * Do the actual work.  Determine if this is a valid Drupal path.
   */
  function validate_argument($arg) { 
    return _pathfield_validate_path($arg, FALSE);
  }
}
