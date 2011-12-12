<?php

/**
 * @file
 * Color page callbacks for the Colors module.
 */

/**
 * Form constructor for the Colors admin form.
 */
function colors_admin_settings() {
  $form = colors_load_colorpicker();
  $default_colors = colors_get_colors('colors_default');

  $form['default_color'] = array(
    '#type' => 'item',
    '#title' => t('Default color'),
    '#tree' => TRUE,
    'input' => array(
      '#title' => t('default color'),
      '#type' => 'textfield',
      '#attributes' => array('class' => array('colorpicker-input')),
      '#default_value' => $default_colors['background'],
      '#size' => 7,
      '#maxlength' => 7,
      '#title_display' => 'invisible',
    ),
  );

  $form['process_order'] = array(
    '#tree' => TRUE,
    'info' => array(
      '#type' => 'item',
      '#title' => t('Process order'),
    ),
    'enabled' => array(
      '#type' => 'checkbox',
      '#title' => t('Change the CSS processing order.'),
      '#default_value' => variable_get('colors_process_order_enabled', FALSE),
      '#description' => t('Color order is cascading, CSS from modules at the bottom will override the top.'),
    ),
  );

  colors_include_api();
  $form['modules'] = array(
    '#tree' => TRUE,
  );
  $delta = 0;
  foreach (module_invoke_all('colors_info') as $module => $info) {
    if (!variable_get("colors_$module" . '_enabled', FALSE)) {
      continue;
    }
    $weight = variable_get('colors_weight_' . $module, $delta);
    $form['modules'][$module]['#name'] = $info['title'];
    $form['modules'][$module]['#weight'] = $weight;
    $form['modules'][$module]['weight'] = array(
      '#type' => 'textfield',
      '#title' => t('Weight for @title', array('@title' => $info['title'])),
      '#title_display' => 'invisible',
      '#size' => 4,
      '#default_value' => $weight,
      '#attributes' => array('class' => array('colors-weight')),
    );
    $delta++;
  }
  uasort($form['modules'], 'element_sort');

  $form['order_settings'] = array(
    '#type' => 'container',
    '#states' => array(
      'visible' => array(
        'input[name="process_order[enabled]"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save settings'),
      '#submit' => array('colors_admin_settings_submit'),
    ),
  );

  return $form;
}

/**
 * Form submission handler for colors_admin_settings().
 */
function colors_admin_settings_submit($form, &$form_state) {
  _colors_set_colors('colors_default', 'colors', $form_state['values']['default_color']['input']);
  variable_set('colors_process_order_enabled', $form_state['values']['process_order']['enabled']);

  foreach ($form_state['values']['modules'] as $module => $weight) {
    variable_set('colors_weight_' . $module, $weight['weight']);
  }

  colors_css_clear();
}

/**
 * Returns HTML for the settings form.
 *
 * @param array $variables
 *   An associative array containing:
 *   - form: A render element representing the form.
 */
function theme_colors_admin_settings($variables) {
  $form = $variables['form'];

  $rows = array();
  foreach (element_children($form['modules']) as $module) {
    $row = array();
    $row[] = $form['modules'][$module]['#name'];
    $row[] = drupal_render($form['modules'][$module]['weight']);

    $rows[] = array('data' => $row, 'class' => array('draggable'));
  }

  $form['order_settings']['table'] = array(
    '#theme' => 'table',
    '#header' => array(
      t('Module'),
      t('Weight'),
    ),
    '#rows' => $rows,
    '#attributes' => array('id' => 'colors-settings'),
  );
  drupal_add_tabledrag('colors-settings', 'order', 'sibling', 'colors-weight');

  return drupal_render_children($form);
}
