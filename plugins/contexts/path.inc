<?php
// $Id$

/**
 * @file
 *
 * Plugin to provide a path context
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t('Path'),
  'description' => t('An internal site path.'),
  'context' => 'ctools_context_create_path',
  'settings form' => 'ctools_context_path_settings_form',
  'settings form validate' => 'ctools_context_path_settings_form_validate',
  'defaults' => array('type' => 'current', 'path' => ''),
  'keyword' => 'path',
  'context name' => 'path',
  'convert list' => array(
    'source' => t('Internal path'),
    'alias' => t('Path alias'),
  ),
  'convert' => 'ctools_context_path_convert',
);

/**
 * It's important to remember that $conf is optional here, because contexts
 * are not always created from the UI.
 */
function ctools_context_create_path($empty, $data = NULL, $conf = FALSE) {
  
  $context = new ctools_context('path');
  $context->plugin = 'path';

  if ($empty) {
    return $context;
  }
    
  if ($conf) {
  	$path_info = new stdClass();
  	
    if ($data['type'] == 'current') {
    	$path_info->source = $_GET['q'];
    	
      $title = t('Current path');
    }
    else {
      $path_info->source = drupal_get_normal_path($data['path']);
           
      $title = t('Specified path');
    }
    $path_info->alias = drupal_get_path_alias($path_info->source);

    $data = $path_info;
  }
  
  if (!empty($data)) {
  	$context->data  = $data;
    $context->title = $title;
    
    return $context;
  }
}

/**
 * Settings form for path context creation.
 */
function ctools_context_path_settings_form($conf) {
  ctools_include('dependent');
  
  $form['type'] = array(
    '#title' => t('Enter the context type'),
    '#type' => 'radios',
    '#options' => array(
      'current' => t('Current path'),
      'specify' => t('Specify a path'),      
    ),
    '#default_value' => $conf['type'],
  );

  $form['path'] = array(
    '#title' => t('Enter an internal site path'),
    '#type' => 'textfield',
    '#maxlength' => 512,
    '#process' => array('ctools_dependent_process'),
    '#dependency' => array('radio:context[context_settings][type]' => array('specify')),
    '#default_value' => $conf['path'],
  );

  return $form;
}

/**
 * Validate a path.
 */
function ctools_context_path_settings_form_validate($form, &$form_values, &$form_state) {
  
	if ($form_values['type'] != 'specify') {
    return;
  }
  // Validate the path
  if (empty($form_values['path'])) {
    form_error($form['path'], t('No path was specified.'));
  }
  else if (!_pathfield_validate_path($form_values['path'], FALSE)) {
    form_error($form['path'], t('Invalid path specified.'));  
  }
}

/**
 * Convert a context into a string.
 */
function ctools_context_path_convert($context, $type) {
  switch ($type) {
    case 'source':
      return $context->data->source;
    case 'alias':
    	return $context->data->alias;
  }
}

