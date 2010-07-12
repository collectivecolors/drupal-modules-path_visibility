<?php
// $Id$

/**
 * @file
 * Interface between pathfield.module and views.module.
 */

/**
 * Implementation of hook_views_handlers().
 */
function path_visibility_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'path_visibility') .'/includes/views/handlers',
    ),
    'handlers' => array(      
      // Argument handlers.    
      'path_visibility_handler_argument_path' => array(
        'parent' => 'views_handler_argument',
      ),
      // Filter handlers.
      'path_visibility_handler_filter_path' => array(
        'parent' => 'views_handler_filter_string',
      ), 
    ),
  );
}

/**
 * Implementation of hook_views_plugins().
 */
function path_visibility_views_plugins() {
  return array(
    'module' => 'path_visibility',
    'argument validator' => array(
      'path' => array(
        'title' => t('Valid Drupal Path'),
        'handler' => 'path_visibility_plugin_argument_validate_path',
        'path' => drupal_get_path('module', 'path_visibility') . '/includes/views/plugins',
      ),
    ),
  );  
}

/**
 * Return CCK Views data for the link_field_settings($op == 'views data').
 */
function pathfield_views_data($field) {
  
	// First build the automatic views data provided for us by CCK.
  
  $data        = content_views_field_views_data($field);
  $table_alias = content_views_tablename($field);
   
  // Alter the views path field.
  
  unset($data[$table_alias][$field['field_name'] .'_path']['argument']);
  unset($data[$table_alias][$field['field_name'] .'_path']['filter']);
  
  // Alter the views path regexp field.
  
  $data[$table_alias][$field['field_name'] .'_regexp']['argument']['handler'] 
    = 'path_visibility_handler_argument_path';
  
  $data[$table_alias][$field['field_name'] .'_regexp']['filter']['handler']
    = 'path_visibility_handler_filter_path';
    
  unset($data[$table_alias][$field['field_name'] .'_regexp']['sort']);
    
  // Alter the views path negate field.
  
  unset($data[$table_alias][$field['field_name'] .'_negate']['argument']);
      
  // Alter the views path weight field. 
  
  unset($data[$table_alias][$field['field_name'] .'_weight']['argument']);
  return $data;
}
