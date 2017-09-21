<?php
/**
 * @file
 * Website administrator settings for Skimlinks.
 *
 * @todo
 * settings: rss feed modification
 *
 * by Mike Carter ("budda", http://drupal.org/user/13164)
 */

/**
 * Implements hook_admin_settings() for module settings configuration.
 * @todo rename "skimlinks_publisherid" variable to skimlinks_domainid
 */
function skimlinks_admin_settings_form($form_state) {
  $form['account'] = array(
    '#type' => 'fieldset',
    '#title' => t('General settings'),
  );

  $form['account']['skimlinks_publisherid'] = array(
    '#title' => t('Domain ID'),
    '#attributes' => array(
      'placeholder' => t('000000X000000'),
    ),
    '#type' => 'textfield',
    '#default_value' => variable_get('skimlinks_publisherid', ''),
    '#size' => 15,
    '#maxlength' => 20,
    '#required' => TRUE,
    '#description' => t(
      'This ID is unique to each site you affiliate with Skimlinks. Get your Domain ID on the !hub. If you don\'t have a Skimlinks account you can apply for one !apply.',
      array(
        '!hub' => l(t('Skimlinks Hub'), 'https://hub.skimlinks.com/setup/install', array('attributes' => array('target' => ' _blank'))),
        '!apply' => l(t('here'), 'https://signup.skimlinks.com/', array('attributes' => array('target' => ' _blank'))),
      )
    )
  );

  $form['account']['skimlinks_subdomain'] = array(
    '#title' => t('Custom redirection sub domain'),
    '#type' => 'textfield',
    '#field_prefix' => 'http://',
    '#default_value'  => variable_get('skimlinks_subdomain', 'go.redirectingat.com'),
    '#description' => t('You may use a custom subdomain to redirect your affiliated links through rather than the default go.redirectingat.com. For more information please see the !customredirect', array('!customredirect' => l(t('Skimlinks redirect help'), 'https://accounts.skimlinks.com/customdomain')))
  );

  return system_settings_form($form);
}

/**
 * Validates the Skimlinks admin form.
 * @todo Improve domain id validation.
 */
function skimlinks_admin_settings_form_validate($form, &$form_state) {
  // Ensure the skimlinks account consist of only numbers and letters
  if (!preg_match('/^[a-zA-Z0-9]*$/', $form_state['values']['skimlinks_publisherid'])) {
    form_set_error('skimlinks_publisherid', 'A valid Domain ID should have the following format: 000000X000000');
    return FALSE;
  }

  if ($form_state['values']['skimlinks_subdomain']) {
  
    // If the admin included http:// at the start, remove it rather than moan with an error about it
    if (substr($form_state['values']['skimlinks_subdomain'], 0, 7) == 'http://') {
      form_set_value($form['account']['skimlinks_subdomain'], str_replace('http://', '', $form_state['values']['skimlinks_subdomain']), $form_state);
    }
  
    // Validate the provided sub domain by comparing the Skimlinks default response with the new subdomain response
    $standard_url = 'http://redirectingat.com/check/domain_check.html';
    $cnamecheck_url = 'http://' . $form_state['values']['skimlinks_subdomain'] . '/check/domain_check.html';
    $original = drupal_http_request($standard_url);
    $new = drupal_http_request($cnamecheck_url);
    
    if ($original->data !== $new->data) {
      form_set_error('skimlinks_subdomain', t('Your custom redirection sub-domain is not currently pointing at Skimlinks servers.'));
    }
  }
  else {
    form_set_value($form['account']['skimlinks_subdomain'], 'http://go.redirectingat.com', $form_state);
  }
}