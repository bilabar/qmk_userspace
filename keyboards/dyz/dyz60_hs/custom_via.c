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

#ifdef VIA_ENABLE

// Declaring enums for VIA config menu
enum via_socd_cleaner_value {
    // clang-format off
    id_socd_cleaner_enable = 1,
    id_socd_cleaner_h_enable = 2,
    id_socd_cleaner_h_left = 3,
    id_socd_cleaner_h_right = 4,
    id_socd_cleaner_h_resolution = 5,
    id_socd_cleaner_v_enable = 6,
    id_socd_cleaner_v_up = 7,
    id_socd_cleaner_v_down = 8,
    id_socd_cleaner_v_resolution = 9,
    // clang-format on
};

extern socd_cleaner_t socd_h;
extern socd_cleaner_t socd_v;
extern socd_cleaner_config_t socd_cleaner_config;

// Handle the data received by the keyboard from the VIA menus
void via_socd_cleaner_set_value(uint8_t *data) {
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch (*value_id) {
        case id_socd_cleaner_enable: {
            socd_cleaner_config.enable = value_data[0];
            if (socd_cleaner_config.enable) {
                socd_cleaner_enable_noeeprom();
            } else {
                socd_cleaner_disable_noeeprom();
            }
            break;
        }
        case id_socd_cleaner_h_enable: {
            socd_cleaner_config.pairs[0].resolution = value_data[0];
            socd_h.resolution = socd_cleaner_config.pairs[0].resolution;
            break;
        }
        case id_socd_cleaner_h_left: {
            socd_cleaner_config.pairs[0].keys[0] = value_data[1] | (value_data[0] << 8);
            socd_h.keys[0] = socd_cleaner_config.pairs[0].keys[0];
            break;
        }
        case id_socd_cleaner_h_right: {
            socd_cleaner_config.pairs[0].keys[1] = value_data[1] | (value_data[0] << 8);
            socd_h.keys[1] = socd_cleaner_config.pairs[0].keys[1];
            break;
        }
        case id_socd_cleaner_h_resolution: {
            socd_cleaner_config.pairs[0].resolution = value_data[0];
            socd_h.resolution = socd_cleaner_config.pairs[0].resolution;
            break;
        }
        case id_socd_cleaner_v_enable: {
            socd_cleaner_config.pairs[1].resolution = value_data[0];
            socd_h.resolution = socd_cleaner_config.pairs[1].resolution;
            break;
        }
        case id_socd_cleaner_v_up: {
            socd_cleaner_config.pairs[1].keys[0] = value_data[1] | (value_data[0] << 8);
            socd_v.keys[0] = socd_cleaner_config.pairs[1].keys[0];
            break;
        }
        case id_socd_cleaner_v_down: {
            socd_cleaner_config.pairs[1].keys[1] = value_data[1] | (value_data[0] << 8);
            socd_v.keys[1] = socd_cleaner_config.pairs[1].keys[1];
            break;
        }
        case id_socd_cleaner_v_resolution: {
            socd_cleaner_config.pairs[1].resolution = value_data[0];
            socd_v.resolution = socd_cleaner_config.pairs[1].resolution;
            break;
        }
        default: {
            // Unhandled value.
            break;
        }
    }
}

// Handle the data sent by the keyboard to the VIA menus
void via_socd_cleaner_get_value(uint8_t *data) {
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch (*value_id) {
        case id_socd_cleaner_enable: {
            value_data[0] = socd_cleaner_config.enable;
            break;
        }
        case id_socd_cleaner_h_enable: {
            value_data[0] = socd_cleaner_config.pairs[0].resolution ? 1 : 0;
            break;
        }
        case id_socd_cleaner_h_left: {
            value_data[0] = socd_cleaner_config.pairs[0].keys[0] >> 8;
            value_data[1] = socd_cleaner_config.pairs[0].keys[0] & 0xFF;
            break;
        }
        case id_socd_cleaner_h_right: {
            value_data[0] = socd_cleaner_config.pairs[0].keys[1] >> 8;
            value_data[1] = socd_cleaner_config.pairs[0].keys[1] & 0xFF;
            break;
        }
        case id_socd_cleaner_h_resolution: {
            value_data[0] = socd_cleaner_config.pairs[0].resolution;
            break;
        }
        case id_socd_cleaner_v_enable: {
            value_data[0] = socd_cleaner_config.pairs[1].resolution;
            break;
        }
        case id_socd_cleaner_v_up: {
            value_data[0] = socd_cleaner_config.pairs[1].keys[0] >> 8;
            value_data[1] = socd_cleaner_config.pairs[1].keys[0] & 0xFF;
            break;
        }
        case id_socd_cleaner_v_down: {
            value_data[0] = socd_cleaner_config.pairs[1].keys[1] >> 8;
            value_data[1] = socd_cleaner_config.pairs[1].keys[1] & 0xFF;
            break;
        }
        case id_socd_cleaner_v_resolution: {
            value_data[0] = socd_cleaner_config.pairs[1].resolution;
            break;
        }
        default: {
            // Unhandled value.
            break;
        }
    }
}


void via_socd_cleaner_save(void)
{
    eeconfig_update_socd_cleaner_current();
}

// Handle the commands sent and received by the keyboard with VIA
void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
    // data = [ command_id, channel_id, value_id, value_data ]
    uint8_t *command_id        = &(data[0]);
    uint8_t *channel_id        = &(data[1]);
    uint8_t *value_id_and_data = &(data[2]);

    if (*channel_id == id_custom_channel) {
        switch (*command_id) {
            case id_custom_set_value: {
                via_socd_cleaner_set_value(value_id_and_data);
                break;
            }
            case id_custom_get_value: {
                via_socd_cleaner_get_value(value_id_and_data);
                break;
            }
            case id_custom_save: {
                via_socd_cleaner_save();
                break;
            }
            default: {
                // Unhandled message.
                *command_id = id_unhandled;
                break;
            }
        }
        return;
    }

    // Return the unhandled state
    *command_id = id_unhandled;

    // DO NOT call raw_hid_send(data,length) here, let caller do this
}

#endif // VIA_ENABLE
