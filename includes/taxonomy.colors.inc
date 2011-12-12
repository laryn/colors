<?php

/**
 * @file
 * Provides Color integration on behalf of taxonomy.module.
 */

/**
 * Implements hook_colors_info().
 */
function taxonomy_colors_info() {
  return array(
    'taxonomy_term' => array(
      'title' => 'Taxonomy term',
    ),
  );
}

/**
 * Configuration form for coloring taxonomy.
 */
function colors_admin_taxonomy_term_settings() {
  // CSS for our custom form.
  $form = colors_load_colorpicker();
  $form['#attached']['css'][] = ctools_attach_css('colors.admin', 'colors');

  $form['taxonomy_term_colors'] = array(
    '#type' => 'item',
    '#title' => t('Taxonomy term colors'),
    '#description' => t('Colors on a per-taxonomy basis. After enabling a vocabulary, you can set colors for individual taxonomy terms below.'),
  );
  foreach (taxonomy_get_vocabularies() as $vid => $vocab) {
    $enabled = variable_get('colors_taxonomy_term_' . $vid . '_enabled', FALSE);
    $form[$vid] = array(
      '#type' => 'fieldset',
      '#title' => $vocab->name,
      '#collapsible' => TRUE,
      '#collapsed' => !$enabled,
    );
    $form[$vid]['colors_taxonomy_term_' . $vid . '_enabled'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable this vocabulary'),
      '#default_value' => $enabled,
    );

    if ($enabled) {
      // If enabled, get all the terms in the taxonomy.
      $term_ids = taxonomy_get_tree($vid);
      // Print textfield for each term so user can add a hex value for a color.
      foreach ($term_ids as $term) {
        $class = 'colors_taxonomy_term_' . $term->tid;
        $colors = colors_get_colors($class, 'colors');
        $form[$vid][$class] = array(
          '#title' => t($term->name),
          '#type' => 'textfield',
          '#attributes' => array('class' => array('colorpicker-input')),
          '#default_value' => $colors['background'],
          '#size' => 7,
          '#maxlength' => 7,
        );
      }
    }
  }

  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  $form['#submit'][] = 'colors_admin_taxonomy_term_settings_submit';

  return $form;
}

/**
 * Submit handler for colors_admin_taxonomy_term_settings.
 */
function colors_admin_taxonomy_term_settings_submit($form, &$form_state) {
  $enabled = FALSE;
  foreach (taxonomy_get_vocabularies() as $vid => $vocab) {
    if (variable_get('colors_taxonomy_term_' . $vid . '_enabled')) {
      $term_ids = taxonomy_get_tree($vid);
      foreach ($term_ids as $term) {
        $class = 'colors_taxonomy_term_' . $term->tid;
        _colors_set_colors($class, 'taxonomy_term', $form_state['values'][$class]);
      }
    }
    if ($form_state['values']['colors_taxonomy_term_' . $vid . '_enabled']) {
      $enabled = TRUE;
    }
    variable_set('colors_taxonomy_term_' . $vid . '_enabled', $form_state['values']['colors_taxonomy_term_' . $vid . '_enabled']);
  }
  variable_set('colors_taxonomy_term_enabled', $enabled);

  drupal_set_message(t('The configuration options have been saved.'));
}

/**
 * Implements hook_colors_classes().
 */
function taxonomy_colors_classes($entity) {
  $filtered_entity = array_intersect_key((array) $entity, field_info_instances($entity->entity_type, $entity->bundle));
  $tids = array();
  foreach ($filtered_entity as $key => $value) {
    foreach ($value as $language => $term) {
      foreach ($term as $content) {
        if (isset($content['tid'])) {
          $tids[] = $content['tid'];
        }
      }
    }
  }

  $class_names = array();
  foreach (taxonomy_get_vocabularies() as $vid => $vocab) {
    if (variable_get('colors_taxonomy_term_' . $vid . '_enabled', FALSE)) {
      // If enabled, get all the terms in the taxonomy.
      foreach (taxonomy_get_tree($vid) as $term) {
        if (in_array($term->tid, $tids)) {
          $class_names[] = 'colors-taxonomy-term-' . $term->tid;
        }
      }
    }
  }
  return $class_names;
}
