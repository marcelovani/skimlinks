<?php

namespace Drupal\skimlinks\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Drupal\Component\Render\FormattableMarkup;

/**
 * Configure Skimlinks settings for this site.
 */
class SkimlinksAdminSettingsForm extends ConfigFormBase {

	/**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'skimlinks_admin_settings';
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return ['skimlinks.settings'];
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
  	$config = $this->config('skimlinks.settings');

  	$form['account'] = array(
	    '#type' => 'fieldset',
	    '#title' => $this->t('General settings'),
      '#open' => TRUE,
	  );

  	$form['account']['skimlinks_account'] = [
      '#default_value' => $config->get('account'),
      '#description' => $this->t(
      	'This ID is unique to each site you want to affiliate your links on, and is in the form of 123456X1234567. To get a SiteID <a href=":skimreg">register your site with Skimlinks</a>, or if you already have registered your site, go to your Skimlinks <a href=":managesites">Manage sites</a> page to see the ID next to your domain. <a href=":documentation">Find more information in the documentation</a>', 
      	[
      		':skimreg' => Url::fromUri('https://signup.skimlinks.com')->toString(),
      		':managesites' => Url::fromUri('https://hub.skimlinks.com/account')->toString(),
      		':documentation' => Url::fromUri('https://hub.skimlinks.com/support')->toString(),
      	]
     	),
      '#maxlength' => 20,
      // '#placeholder' => 'UA-',
      '#required' => TRUE,
      '#size' => 15,
      '#title' => $this->t('SiteID'),
      '#type' => 'textfield',
    ];
   
   return parent::buildForm($form, $form_state);
  }

  public function validateForm(array &$form, FormStateInterface $form_state) {
    if (!preg_match('/^[a-zA-Z0-9]*$/', $form_state->getValue('skimlinks_account'))) {
      $form_state->setErrorByName('skimlinks_account', t('A valid SiteID can contain only letters and numbers.'));
      return FALSE;
    }
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $config = $this->config('skimlinks.settings');
    $config
    	->set('account', $form_state->getValue('skimlinks_account'))
    	->save();

	  _drupal_flush_css_js();

    parent::submitForm($form, $form_state);
  }

}