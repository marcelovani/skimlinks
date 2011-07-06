<?php
/*
download first feed url.
get total results.
calculate how many loops of the url in chunks are needed
concatenate the chunks in to one big file.
prod feeds to import the updated file.  see http://drupal.org/node/622700

test - http://api.everyfeed.net/json/5f847cef199fffa001a443f83f78e313/search/dinosaur/start/500/rows/500/currency/GBP
*/
function everyfeed_batch_check($product, $category_id, &$context) {

  // Filter the products by category if one is provided
  $category = '';
  if ($category_id) {
    $category = '/category_id/' . $category_id;
  }

  //!@todo make currency configurable
  $data = everyfeed_callapi('search/' . $product . $category . '/start/0/rows/1/currency/GBP');

  // Do we have some results
  if($data['status'] == 200 && $data['numFound'] > 0) {
    $context['sandbox']['max'] = 5; // $data['numFound']; //@debug
    $context['sandbox']['product'] = $product;
    $context['sandbox']['category_id'] = $category_id;
    $context['sandbox']['api'] = $product . $category;
        
    $context['message'] = t('Found @count products about %keyword', array('@count' => $data['numFound'], '%keyword' => $product));
  } else {
    //!@todo mark this search as crap and let user know
    $context['message'] = t('Failed');
  }
}


/**
 * Import a product search catalogue based on a search set.
 * 
 * @access public
 * @param mixed $searchid
 * @return void
 */
function everyfeed_batch_process($product, $category_id, &$context) {
  if (empty($context['sandbox'])) {
    $context['sandbox']['progress'] = 0;
    everyfeed_batch_check($product, $category_id, $context);
  }
  
  $limit = 5; // 500
  
  // Filter the products by category if one is provided
  $category = '';
  if ($category_id) {
    $category = '/category_id/' . $category_id;
  }
  
  //!@todo make currency configurable
  $method  = 'search/' . $product . $category . '/start/' . $context['sandbox']['progress'] . '/rows/' . $limit . '/currency/GBP';
  $url = 'http://api.everyfeed.net/json/' . variable_get('everyfeed_apikey', '') . '/' . $method . '/';

  $result = drupal_http_request($url);
  $data = drupal_json_decode($result->data); $data = $data['everyFeed'];
  
  $context['sandbox']['progress'] += $limit;

  if($data['status'] == 200) {
    foreach ($data['products'] as $row) {
      $context['results'][] = $row['title'];
      $context['sandbox']['progress']++;
      $context['message'] = t('Adding product @progress of @max which includes "%product" !pic', array(
        '@progress' => $context['sandbox']['progress'],
        '@max' => $context['sandbox']['max'],
        '%product' => $row['title'],
        '!pic' => theme('image', array(
          'path' => $row['image_url'],
          'height' => '200px',
          'alt' => t('Image of @title', array('@title' => $row['title'])),
          'title' => $row['title'],
          'attributes' => array('style' => 'vertical-align: text-top'))
        )
      ));
      everyfeed_batch_createnode($row);
    }
  }
    
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
  
}

function everyfeed_batch_complete($success, $results, $operations) {
  // The 'success' parameter means no fatal PHP errors were detected. All
  // other error management should be handled using 'results'.
  if ($success) {
    $message = format_plural(count($results), 'One post processed.', '@count posts processed.');
    $message .= count($results) .' processed.';
    $message .= theme('item_list', $results);
  }
  else {
    $message = t('Finished with an error.');
  }
  
  drupal_set_message($message);
  
  // Providing data for the redirected page is done through $_SESSION.
/*
  foreach ($results as $result) {
    $items[] = t('Loaded node %title.', array('%title' => $result));
  }
  $_SESSION['my_batch_results'] = $items;
*/
}


/**
 * Populates a node with product data
 * 
 * @access public
 * @param mixed $data
 * @return void
 */
function everyfeed_batch_createnode($data) {
    module_load_include('inc', 'node', 'node.pages');
    
    global $user;
    $node = array(
     'type' => 'ef_product',
     'language' => 'en', 
     'created' => strtotime($data['firstseen'])
    );

    $form_state['values'] = array();
    $form_state['values']['language'] = $lang = "und"; // substitute for the language of the node
/*     $form_state['values']['name'] = $user->name; */
    $form_state['values']['status'] = 1;
    $form_state['values']['promote'] = 0;
    $form_state['values']['sticky'] = 0;
/*     $form_state['values']['created'] = ; */
    $form_state['values']['op'] = t('Save');
    
    $form_state['values']['field_productid']['und'][0]['value'] = $data['product_id'];
    $form_state['values']['title'] = $data['title'];
    $form_state['values']['body']['und'][0]['value'] =  $data['description'];    
    $form_state['values']['field_price']['und'][0]['value'] = $data['price'] / 100;
    $form_state['values']['field_currency']['und'] = $data['currency'];
    $form_state['values']['field_buylink']['und'][0]['url'] = $data['url'];
    
    //!@todo should we check 'verified_image' is true before using the image?
    $form_state['values']['field_image']['und'][0] = array(
      'filefield_remote' => array('url' => $data['image_url']),
      'title' => $data['title'],
      'alt' => t('Image of !title', array('!title' => $data['title']))
    );
    
    // Build up a comma seperated list of categories for this product
    if ($data['category']) {
      $cats = array();
      
      foreach($data['category'] as $term) {
        $cats[] = $term['name'];
      }
      
      if (count($cats) > 0) {
        $form_state['values']['field_tags']['und'] = implode(',', $cats);
      }
    }
    
	watchdog('everyfeed', print_r($data, true), array(), WATCHDOG_DEBUG);

    // Create the node!
    drupal_form_submit('ef_product_node_form', $form_state, (object) $node);

}

