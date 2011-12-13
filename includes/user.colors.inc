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
      'short_description' => t('Enable colors for user role'),
      'long_description' => t('Colors for user roles. If enabled, you may set colors for each user role below.'),
      'function' => 'user_roles',
    ),
  );
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
