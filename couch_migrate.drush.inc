<?php

/**
 * @file
 * Drush integration for CouchDB migrate. Let's us play with making some CouchDB data.
 */

/**
 * Implementation of hook_drush_help().
 */
function couch_migrate_drush_help($section) {
  switch ($section) {
    case 'drush:couch-migrate migrate':
      $output = dt("Execute a SQL query and insert that data in CouchDB.") ."\n\n";
      $output .= dt("Argument 1 (required):\n a SQL query to execute.") ."\n\n";
      $output .= dt("Argument 2 (optional):\n The db to query.  Defaults to default.") ."\n\n";      
      return $output;
    case 'drush:couch-migrate status':
      $output = dt("Check to see if CouchDB is running.") ."\n\n";
      return $output;      
  }
}

/**
 * Implementation of hook_drush_command().
 */
function couch_migrate_drush_command() {
  $items['couch-migrate migrate'] = array(
    'callback' => 'couch_migrate_drush_migrate',
    'description' => 'Executes a SQL query and stores results in the configured CouchDB instance. Arg1: a sql query.  Arg2: a db to execute that query against.',
    'aliases' => array('cmm'),    
  );
  $items['couch-migrate status'] = array(
    'callback' => 'couch_migrate_drush_status',
    'description' => 'Checks if CouchDB is running.',
  );  
  return $items;
}


/**
 * Execute a query against our db, and store the results in a CouchDB.
 */
function couch_migrate_drush_migrate() {
  $args = func_get_args();
  
  $query = array_shift($args);
  $db = array_shift($args);
  
  if (!$query) {
    drush_print(t("Supply a query!  Example: \"SELECT * FROM node LIMIT 10\""));
    return;
  }
  
  // if no db provided, default to default
  if (!$db) {
    $db = 'default';
  }
  
  $couch_migrate_location = variable_get('couch_migrate_location', NULL);
  $couchdb_db = variable_get('couch_migrate_db', NULL);
  
  if (!$couch_migrate_location) {
    drush_print(t("Needs a CouchDB location.  Go to /admin/settings/couch_migrate."));
    return;
  }
  
  // if couchdb/db is there, build and exec a query
  if (couch_migrate_couchdb_db_exists($couch_migrate_location, $couchdb_db)) {
    couch_migrate_couchdb_query($query, $db);
  } else {
    drush_print(t("The current CouchDB path and DB do not exist.  Womp womp."));
  }
}

/**
 * Execute a query against our db, and store the results in a CouchDB.
 */
function couch_migrate_drush_status() {
  $couchdb_location = variable_get('couch_migrate_location', NULL);
  
  // if couchdb/db is there, build and exec a query
  if ($version = couch_migrate_couchdb_db_exists($couchdb_location)) {  
    drush_print(t("CouchDB is running at ").$couchdb_location.".");
  } else {
    drush_print(t("CouchDB is NOT running at ").$couchdb_location.".");
  }
}