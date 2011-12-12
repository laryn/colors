<?php

/**
 * @file
 * Provides Color integration on behalf of user.module.
 */

/**
 * Implements hook_colors_info().
 */
function user_colors_info() {
  return array(
    'user_role' => array(
      'title' => 'User role',
    ),
  );
}

/**
 * Configuration form for coloring user roles.
 */
function colors_admin_user_role_settings() {
  // CSS for our custom form.
  $form = colors_load_colorpicker();
  $form['#attached']['css'][] = ctools_attach_css('colors.admin', 'colors');

  $form['user_role_colors'] = array(
    '#type' => 'item',
    '#title' => t('User role colors'),
    '#description' => t('Colors for user roles. If enabled, you may set colors for each user role below.'),
  );
  $enabled = variable_get('colors_user_role_enabled', FALSE);
  $form['fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('User role colors'),
    '#collapsible' => TRUE,
    '#collapsed' => !$enabled,
  );
  $form['fieldset']['colors_user_role_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable colors for user role'),
    '#default_value' => $enabled,
  );
  foreach (user_roles() as $role_id => $role) {
    $class = 'colors_user_role_' . $role_id;
    $colors = colors_get_colors($class, 'colors');

    $form['fieldset'][$class] = array(
      '#title' => t($role),
      '#type' => 'textfield',
      '#attributes' => array('class' => array('colorpicker-input')),
      '#default_value' => $colors['background'],
      '#size' => 7,
      '#maxlength' => 7,
      '#states' => array(
        'visible' => array(
          ':input[name="colors_user_role_enabled"]' => array('checked' => TRUE),
        ),
      ),
    );
  }

  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  $form['#submit'][] = 'colors_admin_user_role_settings_submit';

  return $form;
}

/**
 * Submit handler for colors_admin_user_role_settings.
 */
function colors_admin_user_role_settings_submit($form, &$form_state) {
  variable_set('colors_user_role_enabled', (bool) $form_state['values']['colors_user_role_enabled']);

  // Save the user roles colors.
  foreach (user_roles() as $role_id => $role) {
    $class = 'colors_user_role_' . $role_id;
    _colors_set_colors($class, 'user_role', $form_state['values'][$class]);
  }

  drupal_set_message(t('The configuration options have been saved.'));
}

/**
 * Implements hook_colors_classes().
 *
 * Provide colors per user role.
 */
function user_colors_classes($entity) {
  $class_names = array();
  if (variable_get('colors_user_role_enabled', FALSE)) {
    if ($entity->entity_type == 'user' && !empty($entity->roles)) {
      foreach ($entity->roles as $role_id => $role) {
        $class_names[] = 'colors-user-role-' . $role_id;
      }
    }
  }
  return $class_names;
}
