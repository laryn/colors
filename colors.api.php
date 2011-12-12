<?php

/**
 * @file
 * Hooks provided by the Colors module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Gets the color configuration mapping.
 *
 * @return
 *   Mapping between the colorable features and the CSS input.
 */
function hook_colors_get_color_mapping() {
  return array(
    'background' => 'background-color',
    'text' =>  'color',
    'border' => 'border-color',
  );
}

/**
 * Builds a selector string.
 *
 * @param $class
 *   Class name used for the new selector string.
 *
 * @return
 *   The built selector.
 */
function hook_colors_build_selector($class) {
  $selector = '.' . $class . ',';
  $selector .= '.' . $class . 'a';
  return $selector;
}

/**
 * Declare if the CSS file should be automatically rebuilt.
 *
 * @return bool
 *   TRUE if the file should be rebuilt automatically, FALSE if it should be
 *   rebuilt at runtime.
 */
function hook_colors_rebuild() {
  return TRUE;
}

/**
 * Provide a way for modules to add the classes used to their markup.
 *
 * @param object $entity
 *   The entity object.
 */
function hook_colors_classes($entity) {
  $class_names = array();
  if (variable_get('colors_node_type_enabled', FALSE)) {
    if ($entity->entity_type == 'node') {
      $class_names[] = 'colors-node-type-' . $entity->type;
    }
  }
  return $class_names;
}

/**
 * @} End of "addtogroup hooks".
 */
