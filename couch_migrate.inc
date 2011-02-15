<?php

// hit path, return TRUE if couch is running
function couch_migrate_couchdb_running($path = NULL) {
  $result = couch_migrate_couchdb_exec($path);
  
  $result = json_decode($result);
  
  if ($result->db_name == $db) {
    return TRUE;
  } else {
    return FALSE;
  }
}


// hit path and db, return TRUE if first result is same as db name
function couch_migrate_couchdb_db_exists($path = NULL, $db = NULL) {
  // if db is supplied, we're looking for a path and a specific path
  if ($db) {
    $result = couch_migrate_couchdb_exec($path, $db);
    $result = json_decode($result);
    if ($result->db_name == $db) {
      return TRUE;
    } else return FALSE;
    
  } else {
    // otherwise, just check that couch is running
    $result = couch_migrate_couchdb_exec($path);
    $result = json_decode($result);
    
    if ($result->couchdb == "Welcome" && $result->version) {
      drush_print(t("CouchDB is at version ").$result->version.".");
      return TRUE;
    } else return FALSE;
  }
  
}


// query mysql db, send those rows to couch
function couch_migrate_couchdb_query($query, $db) {
  // switch to whichever db is requested
  db_set_active($db);

  $result = db_query($query);

  while ($row = db_fetch_object($result)) {
    $json = drupal_to_js($row);
    couch_migrate_couchdb_exec(variable_get('couch_migrate_location', NULL), variable_get('couch_migrate_db', NULL), $json, uniqid());  
  }

  // make sure to switch back to default db
  db_set_active('default');
}

// query db, store results in a CouchDB
function couch_migrate_couchdb_exec($couch_path = NULL, $couch_db = NULL, $couch_data = NULL, $couch_doc_id = NULL, $couch_doc_rev = NULL) {
  // build the string to send to couch of location/db/doc_id
  $couch_string = string;
  
  if ($couch_path) {
    $couch_string = $couch_path;
  }
  
  if ($couch_db) {
    $couch_string .= "/".$couch_db;
  }
  
  if ($couch_doc_id) {
    $couch_string .= "/".$couch_doc_id;
  }  
  
  $json = $couch_data;

  $ch = curl_init();
  curl_setopt($ch, CURLOPT_URL, $couch_string);   
  curl_setopt($ch, CURLOPT_HEADER, 0); 
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);  //let's get the result back

  // send some data if we've been supplied it
  if ($json) {
    $file = tmpfile(); 
    fwrite($file, $json); 
    fseek($file, 0);    
    curl_setopt($ch, CURLOPT_PUT, true); 
    curl_setopt($ch, CURLOPT_INFILE, $file);
    curl_setopt($ch, CURLOPT_INFILESIZE, strlen($json));
  }
  
  $result = curl_exec($ch);
  
  if ($json) {
    fclose($file);
  }
  curl_close($ch);
  return $result;
}