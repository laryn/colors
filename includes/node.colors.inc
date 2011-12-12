<?php

/**
 * @file
 * Provides Color integration on behalf of node.module.
 */

/**
 * Implements hook_colors_info().
 */
function node_colors_info() {
  return array(
    'node_type' => array(
      'title' => 'Node type',
    ),
  );
}

/**
 * Configuration form for coloring node types.
 */
function colors_admin_node_type_settings() {
  // CSS for our custom form.
  $form = colors_load_colorpicker();
  $form['#attached']['css'][] = ctools_attach_css('colors.admin', 'colors');

  $form['node_type_colors'] = array(
    '#type' => 'item',
    '#title' => t('Node type colors'),
    '#description' => t('Colors for node types. If enabled, you may set colors for each node type below.'),
  );
  $enabled = variable_get('colors_node_type_enabled', FALSE);
  $form['fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Node type colors'),
    '#collapsible' => TRUE,
    '#collapsed' => !$enabled,
  );
  $form['fieldset']['colors_node_type_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable colors for node types'),
    '#default_value' => $enabled,
  );
  foreach (node_type_get_names() as $type => $name) {
    $class = 'colors_node_type_' . $type;
    $colors = colors_get_colors($class, 'colors');

    $form['fieldset'][$class] = array(
      '#title' => t($name),
      '#type' => 'textfield',
      '#attributes' => array('class' => array('colorpicker-input')),
      '#default_value' => $colors['background'],
      '#size' => 7,
      '#maxlength' => 7,
      '#states' => array(
        'visible' => array(
          ':input[name="colors_node_type_enabled"]' => array('checked' => TRUE),
        ),
      ),
    );
  }

  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  $form['#submit'][] = 'colors_admin_node_type_settings_submit';

  return $form;
}

/**
 * Submit handler for colors_admin_node_type_settings.
 */
function colors_admin_node_type_settings_submit($form, &$form_state) {
  variable_set('colors_node_type_enabled', (bool) $form_state['values']['colors_node_type_enabled']);

  // Save the node colors.
  foreach (node_type_get_names() as $type => $name) {
    $class = "colors_node_type_$type";
    _colors_set_colors($class, 'node_type', $form_state['values'][$class]);
  }

  drupal_set_message(t('The configuration options have been saved.'));
}

/**
 * Implements hook_colors_classes().
 *
 * Provide colors per node type.
 */
function node_colors_classes($entity) {
  $class_names = array();
  if (variable_get('colors_node_type_enabled', FALSE)) {
    if ($entity->entity_type == 'node') {
      $class_names[] = 'colors-node-type-' . $entity->type;
    }
  }
  return $class_names;
}
