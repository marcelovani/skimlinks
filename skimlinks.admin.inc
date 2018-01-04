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
    '#type' => 'textfield',
    '#default_value' => variable_get('skimlinks_publisherid', ''),
    '#size' => 15,
    '#maxlength' => 20,
    '#required' => TRUE,
    '#attributes' => array(
      'placeholder' => t('000000X000000'),
    ),
    '#description' => t(
      'This ID is unique to each site you affiliate with Skimlinks. Get your Domain ID on the !hub. If you don\'t have a Skimlinks account you can apply for one !apply.',
      array(
        '!hub' => l(t('Skimlinks Hub'), 'https://hub.skimlinks.com/setup/install', array('attributes' => array('target' => ' _blank'))),
        '!apply' => l(t('here'), 'https://signup.skimlinks.com', array('attributes' => array('target' => ' _blank'))),
      )
    )
  );

  $form['account']['skimlinks_subdomain'] = array(
    '#title' => t('Custom redirection sub domain'),
    '#type' => 'textfield',
    '#default_value'  => variable_get('skimlinks_subdomain', 'go.redirectingat.com'),
    '#description' => t(
      'You may use a custom subdomain to redirect your affiliate links rather than the default go.redirectingat.com. Please include the http:// or https://. Visit the !settings page for more details.',
      array(
        '!settings' => l(t('Skimlinks Advanced Settings'), 'https://hub.skimlinks.com/setup/settings', array('attributes' =>array('target' => ' _blank')))
      )
    )
  );

  $form['skimlinks_environment'] = array(
    '#type' => 'radios',
    '#title' => t('Environment'),
    '#options' => array(t('Client side'), t('Server side')),
    '#description' => t('Client side uses Javascript, server side runs on the server.'),
    '#default_value' => variable_get('skimlinks_environment', 0),
  );

  $form['server_side'] = array(
    '#type' => 'fieldset',
    '#title' => t('Optional settings'),
    '#states' => array('visible' => array(
      ':input[name="skimlinks_environment"]' => array('value' => '1'))
    ),
  );

  $form['server_side']['skimlinks_domain_whitelist'] = array(
    '#type' => 'textarea',
    '#title' => t('Domain whitelist'),
    '#description' => t('List domains that are Skimlinks Merchants. Please enter one domain per line. i.e. example.com. You do not need to specify the protocol.'),
    '#default_value' => variable_get('skimlinks_domain_whitelist'),
  );

  $form['server_side']['skimlinks_domain_blacklist'] = array(
    '#type' => 'textarea',
    '#title' => t('Domain blacklist'),
    '#description' => t('These domains will not be altered by skimlinks module. Please enter one domain per line. i.e. example.com. You do not need to specify the protocol.'),
    '#default_value' => variable_get('skimlinks_domain_blacklist'),
  );

  $form['server_side']['skimlinks_link_new_window'] = array(
    '#type' => 'checkbox',
    '#title' => t('Open links in a new window'),
    '#description' => t('Tick the box to make the links open on a new window.'),
    '#default_value' => variable_get('skimlinks_link_new_window', 1),
  );

  // Custom submit handler must be defined as system_settings_form()
  // adds its own.
  $form['#submit'] = array('skimlinks_admin_settings_form_submit');

  return system_settings_form($form);
}

/**
 * Validates the Skimlinks admin form.
 * @todo Improve domain id validation.
 */
function skimlinks_admin_settings_form_validate($form, &$form_state) {

  $publisher_id = &$form_state['values']['skimlinks_publisherid'];

  // Trim whitespace
  $publisher_id = trim($publisher_id);

  // Ensure the skimlinks account consist of only numbers and letters
  if (!preg_match('/^[a-zA-Z0-9]*$/', $publisher_id)) {
    form_set_error('skimlinks_publisherid', 'A valid Domain ID should have the following format: 000000X000000');
    return FALSE;
  }

  if ($form_state['values']['skimlinks_subdomain']) {
    $subdomain = &$form_state['values']['skimlinks_subdomain'];

    // Trim whitespace
    $subdomain = trim($subdomain);

    // Check the user has included the URL schema in the subdomain value.
    $valid_url = FALSE;
    foreach (array('http://', 'https://') as $protocol) {
      if (substr($subdomain, 0, strlen($protocol)) === $protocol) {
        $valid_url = TRUE;
        break;
      }
    }
    if (!$valid_url) {
      form_set_error('skimlinks_subdomain', t('Your custom redirection sub-domain is not a valid URL. Please include the http:// or https://'));
    }

    // Validate the provided sub domain by comparing the Skimlinks default response with the new subdomain response
    $standard_url = 'https://go.redirectingat.com/check/domain_check.html';
    $cnamecheck_url = $subdomain . '/check/domain_check.html';
    $original = drupal_http_request($standard_url);
    $new = drupal_http_request($cnamecheck_url);

    if (empty($new->data) || $original->data !== $new->data) {
      form_set_error('skimlinks_subdomain', t('Your custom redirection sub-domain is not currently pointing at Skimlinks servers.'));
    }
  }
  else {
    form_set_value($form['account']['skimlinks_subdomain'], 'https://go.redirectingat.com', $form_state);
  }
}

/**
 * Actions on submission of the skimlinks form.
 */
function skimlinks_admin_settings_form_submit($form, &$form_state) {
  // Clear the javascript cache so the settings are applied
  // immediately.
  drupal_clear_js_cache();
}
