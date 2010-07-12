<?php
// $Id$

/**
 * Argument handler to filter results by regular expression matches to path.
 */
class path_visibility_handler_argument_path extends views_handler_argument {
  
  /****************************************************************************
   * Parent overrides                                                         *
   ****************************************************************************/
  
  /**
   * We don't need a breadcrumb.  So far.
   */
  function uses_breadcrumb() { return FALSE; }

  /**
   * Since our argument is an entire path, there is no wildcard.
   */
  function is_wildcard($arg = NULL) {  return FALSE; }
  function wildcard_title() { return ''; }
  
  /**
   * Specify handler definition data.
   */
  function option_definition() {
    $options = parent::option_definition();
    
    unset($options['wildcard']);
    unset($options['wildcard_substitution']);
    unset($options['breadcrumb']);

    return $options;
  }
  
  /**
   * Provide a default options form for the argument.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    unset($form['breadcrumb']);
    unset($form['wildcard']);
    unset($form['wildcard_substitution']);
    
    $options = array();
    
    if (array_key_exists('none', $form['validate_type']['#options'])) {
      $options['none'] = $form['validate_type']['#options']['none'];  
    }
    if (array_key_exists('php', $form['validate_type']['#options'])) {
      $options['php'] = $form['validate_type']['#options']['php'];    
    }
    if (array_key_exists('path', $form['validate_type']['#options'])) {
      $options['path'] = $form['validate_type']['#options']['path'];    
    }
    $form['validate_type']['#options'] = $options;
  }
  
  /**
   * Provide a list of default behaviors for this argument if the argument
   * is not present.
   *
   * Override this method to provide additional (or fewer) default behaviors.
   */
  function default_actions($which = NULL) {
        
    $defaults = array(
      'ignore' => array(
        'title' => t('Display all values'),
        'method' => 'default_ignore',
        'breadcrumb' => FALSE,
      ),
      'empty' => array(
        'title' => t('Display empty text'),
        'method' => 'default_empty',
        'breadcrumb' => FALSE,
      ),
      'default' => array(
        'title' => t('Provide default argument'),
        'method' => 'default_default',
        'form method' => 'default_argument_form',
        'has default argument' => TRUE,
        'default only' => TRUE, // this can only be used for missing argument, not validation failure
      ),
    );

    if ($which) {
      if (!empty($defaults[$which])) {
        return $defaults[$which];
      }
    }
    else {
      return $defaults;
    }
  }
  
  /**
   * Provide a form for selecting the default argument when the
   * default action is set to provide default argument.
   */
  function default_argument_form(&$form, &$form_state) {
    parent::default_argument_form($form, $form_state);
    
    $options = array();
    
    if (array_key_exists('fixed', $form['default_argument_type']['#options'])) {
      $options['fixed'] = $form['default_argument_type']['#options']['fixed'];  
    }
    if (array_key_exists('php', $form['default_argument_type']['#options'])) {
      $options['php'] = $form['default_argument_type']['#options']['php'];    
    }
    $form['default_argument_type']['#options'] = $options;
  }
  
  /**
   * Set up the query for this argument.
   *
   * The argument sent may be found at $this->argument.
   */
  function query() {
    $this->ensure_my_table();
    
    $field  = "$this->table_alias.$this->real_field";
    $negate = $this->content_field_name('negate');
    
    $source = drupal_get_normal_path($this->argument);
    $alias  = drupal_get_path_alias($this->argument);
  
    if ($negate) {
      if ($source != $alias) {
        $normal_where = "(%s = 0 AND ('%s' REGEXP %s OR '%s' REGEXP %s))";
        $where_args = array($negate, $source, $field, $alias, $field);
      
        $negated_where = "(%s = 1 AND ('%s' NOT REGEXP %s AND '%s' NOT REGEXP %s))";
        $where_args = array_merge($where_args, 
          array($negate, $source, $field, $alias, $field)
        );
      }
      else {
        $normal_where = "(%s = 0 AND '%s' REGEXP %s)";
        $where_args = array($negate, $source, $field);
      
        $negated_where = "(%s = 1 AND '%s' NOT REGEXP %s)";
        $where_args = array_merge($where_args, 
          array($negate, $source, $field)
        );  
      }
      $where = "($normal_where OR $negated_where)";
    }
    else {
      if ($source != $alias) {
        $where      = "('%s' REGEXP %s OR '%s' REGEXP %s)";
        $where_args = array($source, $field, $alias, $field);
      }
      else {
        $where      = "('%s' REGEXP %s)";
        $where_args = array($source, $field);  
      }
    }      
    $this->query->add_where($this->options['group'], $where, $where_args);
  }
    
  /****************************************************************************
   * Accessor / modifier methods                                              *
   ****************************************************************************/
  
  /**
   * Get the database field accessor from a local name.
   */
  function content_field_name($name) {
    if ($name && array_key_exists('content_field_name', $this->definition)) {
      return $this->table_alias . '.' . $this->definition['content_field_name'] . '_' . $name;
    }
    return NULL;
  }  
}
