// Copyright 2024 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file socd_cleaner.c
 * @brief SOCD Cleaner implementation
 *
 * For full documentation, see
 * <https://getreuer.info/posts/keyboards/socd-cleaner>
 */

#include "socd_cleaner.h"

#ifdef __cplusplus
extern "C" {
#endif

bool socd_cleaner_enabled = true;

static void update_key(uint8_t keycode, bool press) {
  if (press) {
    add_key(keycode);
  } else {
    del_key(keycode);
  }
}

bool process_socd_cleaner(uint16_t keycode, keyrecord_t* record,
                          socd_cleaner_t* state) {
  if (!socd_cleaner_enabled || !state->resolution ||
      (keycode != state->keys[0] && keycode != state->keys[1])) {
    return true;  // Quick return when disabled or on unrelated events.
  }
  // The current event corresponds to index `i`, 0 or 1, in the SOCD key pair.
  const uint8_t i = (keycode == state->keys[1]);
  const uint8_t opposing = i ^ 1;  // Index of the opposing key.

  // Track which keys are physically held (vs. keys in the report).
  state->held[i] = record->event.pressed;

  // Perform SOCD resolution for events where the opposing key is held.
  if (state->held[opposing]) {
    switch (state->resolution) {
      case SOCD_CLEANER_LAST:  // Last input priority with reactivation.
        // If the current event is a press, then release the opposing key.
        // Otherwise if this is a release, then press the opposing key.
        update_key(state->keys[opposing], !state->held[i]);
        break;

      case SOCD_CLEANER_NEUTRAL:  // Neutral resolution.
        // Same logic as SOCD_CLEANER_LAST, but skip default handling so that
        // the current key has no effect while the opposing key is held.
        update_key(state->keys[opposing], !state->held[i]);
        // Send updated report (normally, default handling would do this).
        send_keyboard_report();
        return false;  // Skip default handling.

      case SOCD_CLEANER_0_WINS:  // Key 0 wins.
      case SOCD_CLEANER_1_WINS:  // Key 1 wins.
        if (opposing == (state->resolution - SOCD_CLEANER_0_WINS)) {
          // The opposing key is the winner. The current key has no effect.
          return false;  // Skip default handling.
        } else {
          // The current key is the winner. Update logic is same as above.
          update_key(state->keys[opposing], !state->held[i]);
        }
        break;
    }
  }
  return true;  // Continue default handling to press/release current key.
}

socd_cleaner_config_t socd_cleaner_config;

void eeconfig_update_socd_cleaner(const void *data) {
  eeconfig_update_kb_datablock(data);
}

void eeconfig_update_socd_cleaner_current(void) {
  eeconfig_update_socd_cleaner(&socd_cleaner_config);
}

void eeconfig_update_socd_cleaner_default(void) {
  socd_cleaner_config.enable = false;
  socd_cleaner_config.pairs[0] = (socd_cleaner_pair_t){.keys = {KC_A, KC_D}, .resolution = SOCD_CLEANER_LAST};
  socd_cleaner_config.pairs[1] = (socd_cleaner_pair_t){.keys = {KC_W, KC_S}, .resolution = SOCD_CLEANER_LAST};
  eeconfig_update_socd_cleaner(&socd_cleaner_config);
}

void socd_cleaner_toggle(void) {
  if (socd_cleaner_config.enable) {
      socd_cleaner_disable();
  } else {
      socd_cleaner_enable();
  }
}

void socd_cleaner_toggle_noeeprom(void) {
  if (socd_cleaner_config.enable) {
    socd_cleaner_disable_noeeprom();
  } else {
    socd_cleaner_enable_noeeprom();
  }
}

void socd_cleaner_enable(void) {
  socd_cleaner_config.enable = true;
  eeconfig_update_socd_cleaner(&socd_cleaner_config);
  socd_cleaner_enable_noeeprom();
}

void socd_cleaner_enable_noeeprom(void) {
  socd_cleaner_enabled = true;
#ifdef RGBLIGHT_ENABLE
  rgblight_mode_noeeprom(RGBLIGHT_MODE_BREATHING);
#endif
}

void socd_cleaner_disable(void) {
  socd_cleaner_config.enable = false;
  eeconfig_update_socd_cleaner(&socd_cleaner_config);
  socd_cleaner_disable_noeeprom();
}

void socd_cleaner_disable_noeeprom(void) {
  socd_cleaner_enabled = false;
#ifdef RGBLIGHT_ENABLE
  rgblight_reload_from_eeprom();
#endif
}
