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
    $context['sandbox']['max'] = $data['numFound'];
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
  
  $limit = 500;
  
  // Filter the products by category if one is provided
  $category = '';
  if ($category_id) {
    $category = '/category_id/' . $category_id;
  }
  
  //!@todo make currency configurable
  $data = everyfeed_callapi('search/' . $product . $category . '/start/' . $context['sandbox']['progress'] . '/rows/' . $limit . '/currency/GBP');

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