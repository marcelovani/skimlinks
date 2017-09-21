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

  	$form['account']['skimlinks_domainid'] = [
      '#default_value' => $config->get('domainid'),
      '#description' => $this->t(
        'This ID is unique to each site you affiliate with Skimlinks. Get your Domain ID on the <a href=":hub" target="_blank">Skimlinks Hub</a>. If you don\'t have a Skimlinks account you can apply for one <a href=":apply" target="_blank">here</a>.',
      	[
      		':hub' => Url::fromUri('https://hub.skimlinks.com/setup/install')->toString(),
      		':apply' => Url::fromUri('https://signup.skimlinks.com')->toString(),
      	]
     	),
      '#maxlength' => 20,
      // '#placeholder' => 'UA-',
      '#required' => TRUE,
      '#size' => 15,
      '#title' => $this->t('Domain ID'),
      '#type' => 'textfield',
      '#attributes' => array(
        'placeholder' => t('000000X000000'),
      ),
    ];
   
   return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   * @todo Improve domain id validation.
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    // Ensure the skimlinks domainid consist of only numbers and letters
    if (!preg_match('/^[a-zA-Z0-9]*$/', $form_state->getValue('skimlinks_domainid'))) {
      $form_state->setErrorByName('skimlinks_domainid', t('A valid Domain ID should have the following format: 000000X000000'));
      return FALSE;
    }
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $config = $this->config('skimlinks.settings');
    $config
    	->set('domainid', $form_state->getValue('skimlinks_domainid'))
    	->save();

	  _drupal_flush_css_js();

    parent::submitForm($form, $form_state);
  }

}