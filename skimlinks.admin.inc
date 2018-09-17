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
 *
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
    '#default_value' => variable_get('skimlinks_subdomain', 'go.redirectingat.com'),
    '#description' => t(
      'You may use a custom subdomain to redirect your affiliate links rather than the default go.redirectingat.com. Do not include http:// or https://. Visit the !settings page for more details.',
      array(
        '!settings' => l(t('Skimlinks Advanced Settings'), 'https://hub.skimlinks.com/setup/settings', array('attributes' => array('target' => ' _blank')))
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
    '#title' => t('Server side settings'),
    '#states' => array(
      'visible' => array(
        ':input[name="skimlinks_environment"]' => array('value' => '1')
      )
    ),
  );

  $form['server_side']['api'] = array(
    '#type' => 'fieldset',
    '#title' => t('API settings'),
  );

  $form['server_side']['api']['skimlinks_merchant_api_endpoint'] = array(
    '#type' => 'textfield',
    '#title' => t('API Endpoint'),
    '#description' => t('The API key for the Merchant API https://hub.skimlinks.com/toolbox/apis/merchant.'),
    '#default_value' => variable_get('skimlinks_merchant_api_endpoint', 'https://merchants.skimapis.com/v3/merchants'),
  );

  $form['server_side']['api']['skimlinks_merchant_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('API Key'),
    '#description' => t('The API key for the Merchant API https://hub.skimlinks.com/toolbox/apis/merchant.'),
    '#default_value' => variable_get('skimlinks_merchant_api_key'),
  );

  $form['server_side']['api']['skimlinks_merchant_api_account_type'] = array(
    '#type' => 'textfield',
    '#title' => t('Account type'),
    '#description' => t('The account type for the Merchant API https://hub.skimlinks.com/toolbox/apis/merchant.'),
    '#default_value' => variable_get('skimlinks_merchant_api_account_type'),
  );

  $form['server_side']['api']['skimlinks_merchant_api_account_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Account Id'),
    '#description' => t('The account Id for the Merchant API https://hub.skimlinks.com/toolbox/apis/merchant.'),
    '#default_value' => variable_get('skimlinks_merchant_api_account_id'),
  );

  $form['server_side']['skimlinks_api_cron_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Cron to update known domains using the Skimlinks Merchant API'),
    '#description' => t('Tick the box to turn on the cron job.'),
    '#default_value' => variable_get('skimlinks_api_cron_enabled', 1),
  );

  $form['server_side']['cron'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cron settings'),
    '#states' => array(
      'visible' => array(
        ':input[name="skimlinks_api_cron_enabled"]' => array('checked' => TRUE)
      )
    ),
  );

  $form['server_side']['cron']['skimlinks_cron_process_time'] = array(
    '#type' => 'textfield',
    '#title' => t('Cron processing time (Seconds)'),
    '#size' => 4,
    '#description' => t('The number in seconds to spend when running cron.'),
    '#default_value' => (int) variable_get('skimlinks_cron_process_time', 60),
  );

  $form['server_side']['cron']['skimlinks_domains_update_threshold'] = array(
    '#type' => 'textfield',
    '#title' => t('Cron update threshold (Minutes)'),
    '#size' => 4,
    '#description' => t('Used to control how often you want to update the list of known domains. Default: Every 12 hours.'),
    '#default_value' => (int) variable_get('skimlinks_domains_update_threshold', 720),
  );

  $form['server_side']['skimlinks_update_known_domains_on_entity_update'] = array(
    '#type' => 'checkbox',
    '#title' => t('Update known domains when content changes'),
    '#description' => t('Tick the box to update the kwnon domains list when content is saved. The list is automatically populated by the Merchant API.'),
    '#default_value' => variable_get('skimlinks_update_known_domains_on_entity_update', 1),
  );

  $form['server_side']['skimlinks_domain_blacklist'] = array(
    '#type' => 'textarea',
    '#title' => t('Domain blacklist'),
    '#description' => t('These domains will not be altered by skimlinks module. Please enter one domain per line. i.e. example.com. You do not need to specify the protocol.'),
    '#default_value' => implode("\n", skimlinks_domain_blacklist()),
  );

  $form['server_side']['skimlinks_link_new_window'] = array(
    '#type' => 'checkbox',
    '#title' => t('Open links in a new window'),
    '#description' => t('Tick the box to make the links open on a new window.'),
    '#default_value' => variable_get('skimlinks_link_new_window', 1),
  );

  $form['server_side']['skimlinks_link_nofollow'] = array(
    '#type' => 'checkbox',
    '#title' => t('Make links nofollow'),
    '#description' => t('Tick the box to make the links nofollow.'),
    '#default_value' => variable_get('skimlinks_link_nofollow', 1),
  );

  // Custom submit handler must be defined as system_settings_form()
  // adds its own.
  $form['#submit'] = array('skimlinks_admin_settings_form_submit');

  return system_settings_form($form);
}

/**
 * Validates the Skimlinks admin form.
 *
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
    $subdomain = _skimlinks_clean_url($subdomain);
  }
  else {
    form_set_value($form['account']['skimlinks_subdomain'], 'go.redirectingat.com', $form_state);
  }
}

/**
 * Actions on submission of the skimlinks form.
 */
function skimlinks_admin_settings_form_submit($form, &$form_state) {
  // Update blacklist.
  skimlinks_create_blacklist($form_state['values']['skimlinks_domain_blacklist']);

  // Clear the javascript cache so the settings are applied immediately.
  drupal_clear_js_cache();
}
