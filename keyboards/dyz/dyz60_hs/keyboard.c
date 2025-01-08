/* Copyright 2023 Bilabar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "socd_cleaner.h"

socd_cleaner_t socd_h;
socd_cleaner_t socd_v;
socd_cleaner_config_t socd_cleaner_config;

void eeconfig_init_kb(void) {
    eeconfig_update_socd_cleaner_default();

    eeconfig_init_user();
}

void keyboard_post_init_kb(void) {
    eeconfig_read_kb_datablock(&socd_cleaner_config);

    if (socd_cleaner_config.enable) {
        socd_cleaner_enable_noeeprom();
    } else {
        socd_cleaner_disable_noeeprom();
    }

    void set_socd_pair(int index, socd_cleaner_t *socd) {
        socd->keys[0] = socd_cleaner_config.pairs[index].keys[0];
        socd->keys[1] = socd_cleaner_config.pairs[index].keys[1];
        socd->resolution = socd_cleaner_config.pairs[index].resolution;
    }

    set_socd_pair(0, &socd_h);
    set_socd_pair(1, &socd_v);

    keyboard_post_init_user();
}

bool process_record_kb(uint16_t keycode, keyrecord_t* record) {
    if (!process_socd_cleaner(keycode, record, &socd_v)) { return false; }
    if (!process_socd_cleaner(keycode, record, &socd_h)) { return false; }

    switch (keycode) {
        case SOCD_ON:  // Turn SOCD Cleaner on.
            if (record->event.pressed) {
                socd_cleaner_enable();
            }
            return false;
        case SOCD_OFF:  // Turn SOCD Cleaner off.
            if (record->event.pressed) {
                socd_cleaner_disable();
            }
            return false;
        case SOCD_TOGG:  // Toggle SOCD Cleaner.
            if (record->event.pressed) {
                socd_cleaner_toggle();
            }
            return false;
    }

    return process_record_user(keycode, record);
}
