<?php

/**
 * The settings form at admin/settings/couch_migrate
 */
function couch_migrate_settings_form() {
  $queries = variable_get('couch_test_queries', FALSE);
  $queries = unserialize($queries);
  
  $form['couch_migrate_location'] = array(
    '#type' => 'textfield',
    '#title' => t('CouchDB Location'),
    '#description' => t('URL for CouchDB instance you want to populate'),
    '#default_value' => variable_get('couch_migrate_location', NULL),
    '#required' => TRUE,    
  );
  
  $form['couch_migrate_db'] = array(
    '#type' => 'textfield',
    '#title' => t('CouchDB DB Name'),
    '#description' => t('Which CouchDB at '.variable_get('couch_migrate_location', NULL).' do you want to use?'),
    '#default_value' => variable_get('couch_migrate_db', NULL),
    '#required' => TRUE,    
  );  
  
  // $form['#validate'][] = 'couch_migrate_settings_form_validate';
  // $form['#submit'][] = 'couch_migrate_settings_form_submit';
  return system_settings_form($form);
}

// not using this at the moment
function couch_test_settings_form_validate(&$form, &$form_state) {
}

// not using this at the moment
function couch_test_settings_form_submit(&$form, &$form_state) {
  print "hello there";
  print_r($form_state);
  // save queries if save query is checked
  if ($form_state['values']['query_save'] && $form_state['values']['query'] && $form_state['values']['query_name']) {
    $query = array('name' => $form_state['values']['query_name'], 'query' => $form_state['values']['query']);
    
    $queries = unserialize(variable_get('couch_test_queries', array()));
    // print_r($queries);
    
    array_push($queries, $query);
    
    // $queries[$form_state['values']['query_name']] = $form_state['values']['query'];
    
    // print_r($queries);
    
    // array_push($queries, $query);
    
    variable_set('couch_test_queries', $queries);
  } else {
    // execute a query
    if ($form_state['values']['queries']) {
      couch_test_migrate($form_state['values']['queries']);
    }
  } 
}
