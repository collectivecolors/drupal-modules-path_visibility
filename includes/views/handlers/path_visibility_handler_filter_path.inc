<?php
// $Id$

/**
 * Basic textfield filter to handle path filtering.
 */
class path_visibility_handler_filter_path extends views_handler_filter_string {
  
  /****************************************************************************
   * Parent overrides                                                         *
   ****************************************************************************/
    
  /**
   * Specify handler definition data.
   */
  function option_definition() {
    $options = parent::option_definition();

    // We don't care about case.
    unset($options['case']);

    return $options;
  }
  
  /**
   * Add our allowed operators for this filter.
   */
  function operators() {
    
    $operators = array(
      'match' => array(
        'title' => t('Matches'),
        'short' => t('REGEXP'),
        'method' => 'op_match',
        'values' => 1,
      ),
      'not_match' => array(
        'title' => t('Does not match'),
        'short' => t('NOT REGEXP'),
        'method' => 'op_not_match',
        'values' => 1,
      ),      
    );
    // if the definition allows for the empty operator, add it.
    if (!empty($this->definition['allow empty'])) {
      $operators += array(
        'empty' => array(
          'title' => t('Is empty (NULL)'),
          'method' => 'op_empty',
          'short' => t('empty'),
          'values' => 0,
        ),
        'not empty' => array(
          'title' => t('Is not empty (NOT NULL)'),
          'method' => 'op_empty',
          'short' => t('not empty'),
          'values' => 0,
        ),
      );
    }
    return $operators;
  }

  /**
   * Provide the basic form which calls through to subforms.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    unset($form['case']);
  }
  
  /**
   * Provide a simple textfield for equality
   */
  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);
    
    if ($form['value']['#type'] == 'textfield') {
      $form['value']['#title'] = t('Internal Drupal path');
    }    
  }

  /**
   * We don't need case translation.  This is something that the parent class 
   * adds.
   */
  function case_transform() { return ''; }
  
  /**
   * Add this filter to the query.
   *
   * Due to the nature of fapi, the value and the operator have an unintended
   * level of indirection. You will find them in $this->operator
   * and $this->value respectively.
   */
  function query() {
    $this->ensure_my_table();
    
    $field  = "$this->table_alias.$this->real_field";
    $negate = $this->content_field_name('negate');
    
    $info = $this->operators();
    if (!empty($info[$this->operator]['method'])) {
      $this->{$info[$this->operator]['method']}($field, $negate);
    }
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
  
  /****************************************************************************
   * Query handlers                                                           *
   ****************************************************************************/
   
  /**
   * Query match handler.
   */
  function op_match($field, $negate) {
    
    $source = drupal_get_normal_path($this->value);
    $alias  = drupal_get_path_alias($this->value);
  
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

  /**
   * Query "not" match handler.
   */
  function op_not_match($field, $negate) {
    
    $source = drupal_get_normal_path($this->value);
    $alias  = drupal_get_path_alias($this->value);
  
    if ($negate) {
      if ($source != $alias) {
        $normal_where = "(%s = 1 AND ('%s' REGEXP %s OR '%s' REGEXP %s))";
        $where_args = array($negate, $source, $field, $alias, $field);
      
        $negated_where = "(%s = 0 AND ('%s' NOT REGEXP %s AND '%s' NOT REGEXP %s))";
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
        $where      = "('%s' NOT REGEXP %s AND '%s' NOT REGEXP %s)";
        $where_args = array($source, $field, $alias, $field);
      }
      else {
        $where      = "('%s' NOT REGEXP %s)";
        $where_args = array($source, $field);  
      }
    }      
    $this->query->add_where($this->options['group'], $where, $where_args);
  }

  /**
   * Query empty and "not" empty handler.
   */
  function op_empty($field, $negate) {
    if ($this->operator == 'empty') {
      $operator = "IS NULL";
    }
    else {
      $operator = "IS NOT NULL";
    }
    $this->query->add_where($this->options['group'], "$field $operator");
  }
}
