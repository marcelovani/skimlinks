<?php

/**
 * Implements hook_admin_settings() for module settings configuration.
 */
function skimlinks_admin_settings_form($form_state) {
  $form['account'] = array(
    '#type' => 'fieldset',
    '#title' => t('General settings'),
  );

  $form['account']['skimlinks_publisherid'] = array(
    '#title' => t('Publisher ID'),
    '#type' => 'textfield',
    '#default_value' => variable_get('skimlinks_publisherid', ''),
    '#size' => 15,
    '#maxlength' => 20,
    '#required' => TRUE,
    '#description' => t('This ID is unique to each site you want to affiliate your links on, and is in the form of XXXXxYYYYYY. To get a Web Property ID
                        !skimreg, or if you already have registered your site, go to your Skimlinks !managesites page to see the ID next to your domain. !webpropertyid.',
                        array(
                          '!skimreg' => l('register your site with Skimlinks', 'http://www.skimlinks.com'),
                          '!managesites' => l('Manage sites', 'https://accounts.skimlinks.com/sites'),
                          '!webpropertyid' => l('Find more information in the documentation', 'https://accounts.skimlinks.com/support'),
                        )
                      )
  );

  return system_settings_form($form);
}

