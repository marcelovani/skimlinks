<?php

namespace Drupal\skimlinks\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Drupal\Component\Render\FormattableMarkup;
use GuzzleHttp\Exception\RequestException;

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

    $form['account']['skimlinks_subdomain'] = array(
      '#title' => t('Custom redirection sub domain'),
      '#type' => 'textfield',
      '#default_value'  => $config->get('subdomain') ? $config->get('subdomain') : 'go.redirectingat.com',
      '#description' => t(
        'You may use a custom subdomain to redirect your affiliate links rather than the default go.redirectingat.com. Please include the http:// or https://. Visit the <a href="https://hub.skimlinks.com/setup/settings" target="_blank">Skimlinks Advanced Settings</a> page for more details.', 
        array(
          ':settings' => Url::fromUri('https://hub.skimlinks.com/setup/install')->toString(),
        )
      )
    );
   
   return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   * @todo Improve domain id validation.
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    // Trim whitespace
    $form_state->setValue('skimlinks_domainid', trim($form_state->getValue('skimlinks_domainid')));
    $form_state->setValue('skimlinks_subdomain', trim($form_state->getValue('skimlinks_subdomain')));

    // Domain ID

    // Ensure the skimlinks domain ID consists of only numbers and letters
    if (!preg_match('/^[a-zA-Z0-9]*$/', $form_state->getValue('skimlinks_domainid'))) {
      $form_state->setErrorByName('skimlinks_domainid', t('A valid Domain ID should have the following format: 000000X000000'));
      return FALSE;
    }

    // Custom redirection sub domain
    
    // Check the user has included the URL schema in the subdomain value.
    $subdomain = $form_state->getValue('skimlinks_subdomain');
    $valid_url = FALSE;
    foreach (array('http://', 'https://') as $protocol) {
      if (substr($subdomain, 0, strlen($protocol)) === $protocol) {
        $valid_url = TRUE;
        break;
      }
    }
    if (!$valid_url) {
      $form_state->setErrorByName('skimlinks_subdomain', t('Your custom redirection sub-domain is not a valid URL. Please include the http:// or https://'));
    }

    // Validate the provided subdomain by comparing the Skimlinks default 
    // response with the new subdomain response
    $standard_url = 'https://go.redirectingat.com/check/domain_check.html';
    $cnamecheck_url = $subdomain . '/check/domain_check.html';
    $standard_data = FALSE;
    $cnamecheck_data = FALSE;
    $subdomain_error = t('Your custom redirection sub-domain is not currently pointing at Skimlinks servers.');
    // First try the standard URL
    try {
      $standard_data = \Drupal::httpClient()->get($standard_url,array('http_errors' => FALSE));
    }
    catch (RequestException $e) {
      $form_state->setErrorByName('skimlinks_subdomain', t('We\'re sorry, but we can\'t connect to the Skimlinks server at the moment. Please try again later'));
    }
    // Then the CNAME check URL
    try {
      $cnamecheck_data = \Drupal::httpClient()->get($cnamecheck_url, array('http_errors' => FALSE));
    }
    catch (RequestException $e) {
      $form_state->setErrorByName('skimlinks_subdomain', $subdomain_error);
      return;
    }
    // Check if they're equal
    if ((string) $standard_data->getBody() !== (string) $cnamecheck_data->getBody()) {
      $form_state->setErrorByName('skimlinks_subdomain', $subdomain_error);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $config = $this->config('skimlinks.settings');
    $config
    	->set('domainid', $form_state->getValue('skimlinks_domainid'))
      ->set('subdomain', $form_state->getValue('skimlinks_subdomain'))
    	->save();

	  _drupal_flush_css_js();

    parent::submitForm($form, $form_state);
  }

}