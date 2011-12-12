<?php

/**
 * @file
 * Provides Color integration on behalf of og.module.
 */

/**
 * Implements hook_colors_info().
 */
function og_colors_info() {
  return array(
    'og' => array(
      'title' => 'Organic groups',
    ),
  );
}

/**
 * Configuration form for coloring organic groups (og).
 */
function colors_admin_og_settings() {
  // CSS for our custom form.
  $form = colors_load_colorpicker();
  $form['#attached']['css'][] = ctools_attach_css('colors.admin', 'colors');

  $form['og_colors'] = array(
    '#type' => 'item',
    '#title' => t('Group colors'),
    '#description' => t('Colors for organic groups. If enabled, you may set colors for each group below.'),
  );
  $enabled = variable_get('colors_og_enabled', FALSE);
  $form['fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Group colors'),
    '#collapsible' => TRUE,
    '#collapsed' => !$enabled,
  );
  $form['fieldset']['colors_og_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable colors for groups'),
    '#default_value' => $enabled,
  );
  foreach (og_get_all_group() as $gid) {
    $class = 'colors_og_' . $gid;
    $colors = colors_get_colors($class, 'colors');

    $form['fieldset'][$class] = array(
      '#title' => og_label($gid),
      '#type' => 'textfield',
      '#attributes' => array('class' => array('colorpicker-input')),
      '#default_value' => $colors['background'],
      '#size' => 7,
      '#maxlength' => 7,
      '#states' => array(
        'visible' => array(
          ':input[name="colors_og_enabled"]' => array('checked' => TRUE),
        ),
      ),
    );
  }

  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  $form['#submit'][] = 'colors_admin_og_settings_submit';

  return $form;
}

/**
 * Submit handler for colors_admin_og_settings.
 */
function colors_admin_og_settings_submit($form, &$form_state) {
  variable_set('colors_og_enabled', (bool) $form_state['values']['colors_og_enabled']);

  // Save the node colors.
  foreach (og_get_all_group() as $gid) {
    $class = "colors_og_$gid";
    _colors_set_colors($class, 'og', $form_state['values'][$class]);
  }

  drupal_set_message(t('The configuration options have been saved.'));
}

/**
 * Implements hook_colors_classes().
 *
 * Provide colors per og type.
 */
function og_colors_classes($entity) {
  $class_names = array();
  if (variable_get('colors_og_enabled', FALSE)) {
    list($id) = entity_extract_ids($entity->entity_type, $entity);
    if (!empty($id)) {
      foreach (og_get_entity_groups($entity->entity_type, $entity) as $gid) {
        $class_names[] = 'colors_og' . $gid;
      }
    }
  }
  return $class_names;
}
