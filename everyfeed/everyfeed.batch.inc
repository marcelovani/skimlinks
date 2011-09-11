<?php
/**
 * @file
 * Batch import functions for the Everyfeed API module.
 *
 * by Mike Carter ("budda", http://drupal.org/user/13164)
 */
 
define('BATCH_PRODUCTS_PER_CHUNK', 100);

function everyfeed_batch_check($product, $category_id, &$context) {

  // Filter the products by category if one is provided
  $category = '';
  if ($category_id) {
    $category = '/category_id/' . $category_id;
  }

  $data = everyfeed_callapi('search/' . $product . $category . '/start/0/rows/1/currency/' . variable_get('everyfeed_currency', 'GBP'));
  
  // Do we have some results
  if ($data['status'] == 200 && $data['numFound'] > 0) {
    $context['sandbox']['max'] = $data['numFound']; 
    $context['sandbox']['product'] = $product;
    $context['sandbox']['category_id'] = $category_id;
    $context['sandbox']['api'] = $product . $category;
    
    $context['message'] = t('Found @count products about %keyword', array('@count' => $data['numFound'], '%keyword' => $product));
    return TRUE;
  }
  else {
    //!@todo mark this search as crap and let user know
    $context['message'] = t('Failed');
    return FALSE;
  }
}


/**
 * Import a product search catalogue based on a search set.
 * 
 * @access public
 * @param mixed $searchid
 * @return void
 */
function everyfeed_batch_process($product, $category_id, $phrase_id, &$context) {
  if (empty($context['sandbox'])) {
    $context['finished'] = FALSE;
    $context['sandbox']['progress'] = 0;
    
    $result = everyfeed_batch_check($product, $category_id, $context);
    if (!$result) {
      $context['finished'] = TRUE;
      return;
    }
  }
    
  // Filter the products by category if one is provided
  $category = '';
  if ($category_id) {
    $category = '/category_id/' . $category_id;
  }
  
  $method  = 'search/' . $product . $category . '/start/' . $context['sandbox']['progress'] . '/rows/' . BATCH_PRODUCTS_PER_CHUNK . '/currency/' . variable_get('everyfeed_currency', 'GBP');
  $url = 'http://api.everyfeed.net/json/' . variable_get('everyfeed_apikey', '') . '/' . $method . '/';

  $result = drupal_http_request($url);
  $data = drupal_json_decode($result->data); $data = $data['everyFeed'];
  
  if ($data['status'] == 200) {

    // How many products are in this batch? (might be less than BATCH_PRODUCTS_PER_CHUNK)
    $batch_total = count($data['products']);

    foreach ($data['products'] as $row) {
      $context['results'][] = check_plain($row['title']);
      
      $context['sandbox']['progress']++;
      $context['message'] = t('Adding product @progress-@chunklimit out of @max which includes "%product" !pic', array(
        '@progress' => $context['sandbox']['progress'],
        '@max' => $context['sandbox']['max'],
        '@chunklimit' => $context['sandbox']['progress'] + $batch_total,
        '%product' => $row['title'],
        '!pic' => theme('image', array(
          'path' => $row['image_url'],
          'height' => '200px',
          'alt' => t('Image of @title', array('@title' => $row['title'])),
          'title' => check_plain($row['title']),
          'attributes' => array('style' => 'vertical-align: text-top'))
        )
      ));
      
      $row['searchphrase_id'] = $phrase_id;
      
      everyfeed_batch_createnode($row);
    }
    
  }
    
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max']; //!@todo must return true
  }
}


function everyfeed_batch_complete($success, $results, $operations) {
  // The 'success' parameter means no fatal PHP errors were detected. All
  // other error management should be handled using 'results'.
  if ($success) {
    $message = format_plural(count($results), 'One post processed.', '@count posts processed.');
    $message .= theme('item_list', $results);
  }
  else {
    $message = t('Finished with an error.');
  }
  
  drupal_set_message($message); 
}


/**
 * Populates a node with product data
 * 
 * @access public
 * @param mixed $data
 * @return void
 */
function everyfeed_batch_createnode($data) {
    global $user;

    $node = array(
     'type' => 'ef_product',
     'language' => LANGUAGE_NONE, 
    );

    $form_state['values'] = array();
    $form_state['values']['language'] = $lang = $node['language']; // substitute for the language of the node
    $form_state['values']['name'] = $user->name;
    $form_state['values']['uid'] = 1;
    $form_state['values']['status'] = 1; // node is published
    $form_state['values']['comment'] = 1; // comments disabled
    
    $form_state['values']['date'] = $data['firstseen'];
    $form_state['values']['op'] = t('Save');
    
    $form_state['values']['field_searchphraseid'][LANGUAGE_NONE][0]['value'] = $data['searchphrase_id'];
    $form_state['values']['field_productid'][LANGUAGE_NONE][0]['value'] = $data['product_id'];
    
    $form_state['values']['title'] = everyfeed_clean_title($data['title']);
    $form_state['values']['body'][LANGUAGE_NONE][0]['value'] =  $data['description'];    
    $form_state['values']['field_price'][LANGUAGE_NONE][0]['value'] = $data['price'] / 100;
    $form_state['values']['field_currency'][LANGUAGE_NONE] = $data['currency'];
    $form_state['values']['field_buylink'][LANGUAGE_NONE][0]['url'] = $data['url'];
    
    /* Lots of nodes fail to create because of the image denied error.
     * This is a bug in Filefield Sources module see http://drupal.org/node/1221486
     */
    if ($data['verified_image'] == 'True') {
      $form_state['values']['field_image'][LANGUAGE_NONE][0] = array(
        'filefield_remote' => array('url' => $data['image_url']),
        'title' => check_plain(substr($data['title'], 0, 70)),
        'alt' => t('Image of !title', array('!title' => substr($data['title'], 0, 70))) // Image Field imposes 80 character limit
      );
    }
    
    if (isset($data['category'][0])) {
      // Only take the first category which is the most specific.
      $form_state['values']['field_tags'][LANGUAGE_NONE] = ucwords($data['category'][0]['name']);
    }

    $form_state['values']['field_merchant_id'][LANGUAGE_NONE][0]['value'] = $data['merchant']['id'];
    $form_state['values']['field_merchant_name'][LANGUAGE_NONE][0]['value'] = check_plain($data['merchant']['name']);
    $form_state['values']['field_manufacturer'][LANGUAGE_NONE][0]['value'] = check_plain($data['manufacturer']);
    
    // Create the node!
    module_load_include('inc', 'node', 'node.pages');
    drupal_form_submit('ef_product_node_form', $form_state, (object) $node);
}


/**
 * Examine title and clean up closing brackets if it detects uneven amount.
 * This problem appears to be caused by Everyfeed shortening down merchants product
 * titles by removing characters from the middle of the string, often including the 
 * opening bracket.
 * 
 * @access public
 * @param mixed $title
 * @return void
 */
function everyfeed_clean_title($title) {
  $bad = array(
    array(')', '('),
    array(']', '[')
  );

  foreach ($bad as $char) {
    if ($closebracket = stripos($title, $char[0]) != FALSE) {
      if ($openbracket = stripos($title, $char[1]) == FALSE) {
        $title = str_replace($char[0], '', $title);
      }
    }
  }
  
  return $title;
}