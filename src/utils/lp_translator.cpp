/*
 * Copyright (C) 2026 Toymacker-System <dev@toymacker-system.dev>
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "lp_translator.h"
#include <vector>
#include "logger.h"

constexpr size_t MAX_SYSEX_SIZE = 250;
constexpr uint8_t EOX = 0xF7;

constexpr uint8_t CMD_LP_MK3_LAYOUT = 0x0E;
constexpr uint8_t VAL_LP_MK3_PROG_MODE = 0x01;
constexpr uint8_t VAL_LP_MK3_LIVE_MODE = 0x00;
constexpr uint8_t CMD_LP_MK3_RGB_GLOBAL = 0x03;
constexpr uint8_t CMD_LP_MK3_RGB_PREFIX = 0x03;

constexpr uint8_t CMD_LP_MK2_LAYOUT = 0x22;
constexpr uint8_t VAL_LP_MK2_PROG_MODE = 0x01;
constexpr uint8_t VAL_LP_MK2_LIVE_MODE = 0x00;
constexpr uint8_t CMD_LP_MK2_RGB = 0x0B;

constexpr uint8_t CMD_LP_PRO_LAYOUT = 0x2C;
constexpr uint8_t VAL_LP_PRO_PROG_MODE = 0x03;
constexpr uint8_t VAL_LP_PRO_LIVE_MODE = 0x00;

std::vector<uint8_t> lp_translator::get_header_for_model(e_lp_model model) {
    switch (model) {
        case e_lp_model::LAUNCHPAD_PRO_MK3:  return {0xF0, 0x00, 0x20, 0x29, 0x02, 0x0E};
        case e_lp_model::LAUNCHPAD_X:        return {0xF0, 0x00, 0x20, 0x29, 0x02, 0x0C};
        case e_lp_model::LAUNCHPAD_MK3_MINI: return {0xF0, 0x00, 0x20, 0x29, 0x02, 0x0D};
        case e_lp_model::LAUNCHPAD_MK2:      return {0xF0, 0x00, 0x20, 0x29, 0x02, 0x18};
        case e_lp_model::LAUNCHPAD_PRO:      return {0xF0, 0x00, 0x20, 0x29, 0x02, 0x10};
    }
    return {};
}

bool lp_translator::is_valid_pad(e_lp_model model, int pad_index) {
    if (pad_index < 0 || pad_index > 127) return false;
    switch (model) {
        case e_lp_model::LAUNCHPAD_PRO_MK3:
            if (pad_index < 1 || pad_index > 108 || pad_index == 9 || pad_index == 100) return false;
            break;
        case e_lp_model::LAUNCHPAD_X:
        case e_lp_model::LAUNCHPAD_MK3_MINI:
            if (pad_index < 11 || pad_index > 99 || pad_index % 10 == 0) return false;
            break;
        case e_lp_model::LAUNCHPAD_MK2:
            if (!(((pad_index >= 11 && pad_index <= 89) && (pad_index % 10 != 0)) || (pad_index >= 104 && pad_index <= 111))) return false;
            break;
        case e_lp_model::LAUNCHPAD_PRO:
            if (pad_index < 1 || pad_index > 98 || pad_index == 9 || pad_index == 90) return false;
            break;
    }
    return true;
}

std::vector<uint8_t> lp_translator::get_programmer_mode_sysex(e_lp_model model) {
    auto sysex = get_header_for_model(model);
    if (sysex.empty()) return sysex;

    switch (model) {
        case e_lp_model::LAUNCHPAD_PRO_MK3:
        case e_lp_model::LAUNCHPAD_X:
        case e_lp_model::LAUNCHPAD_MK3_MINI:
            sysex.push_back(CMD_LP_MK3_LAYOUT);
            sysex.push_back(VAL_LP_MK3_PROG_MODE);
            break;
        case e_lp_model::LAUNCHPAD_MK2:
            sysex.push_back(CMD_LP_MK2_LAYOUT);
            sysex.push_back(VAL_LP_MK2_PROG_MODE);
            break;
        case e_lp_model::LAUNCHPAD_PRO:
            sysex.push_back(CMD_LP_PRO_LAYOUT);
            sysex.push_back(VAL_LP_PRO_PROG_MODE);
            break;
    }

    sysex.push_back(EOX);
    return sysex;
}

std::vector<uint8_t> lp_translator::get_live_mode_sysex(e_lp_model model) {
    auto sysex = get_header_for_model(model);
    if (sysex.empty()) return sysex;

    switch (model) {
        case e_lp_model::LAUNCHPAD_PRO_MK3:
        case e_lp_model::LAUNCHPAD_X:
        case e_lp_model::LAUNCHPAD_MK3_MINI:
            sysex.push_back(CMD_LP_MK3_LAYOUT);
            sysex.push_back(VAL_LP_MK3_LIVE_MODE);
            break;
        case e_lp_model::LAUNCHPAD_MK2:
            sysex.push_back(CMD_LP_MK2_LAYOUT);
            sysex.push_back(VAL_LP_MK2_LIVE_MODE);
            break;
        case e_lp_model::LAUNCHPAD_PRO:
            sysex.push_back(CMD_LP_PRO_LAYOUT);
            sysex.push_back(VAL_LP_PRO_LIVE_MODE);
            break;
    }

    sysex.push_back(EOX);
    return sysex;
}

std::vector<std::vector<uint8_t>> lp_translator::build_frame_sysex(e_lp_model model, const lp_arena* frame) {
    std::vector<std::vector<uint8_t>> all_chunks;
    if (!frame) return all_chunks;

    std::vector<uint8_t> current_chunk;
    start_new_sysex_chunk(current_chunk, model);

    bool is_new_format = (model == e_lp_model::LAUNCHPAD_PRO_MK3 || model == e_lp_model::LAUNCHPAD_X || model == e_lp_model::LAUNCHPAD_MK3_MINI);
    int divisor = is_new_format ? 2 : 4;
    size_t bytes_per_pad = is_new_format ? 5 : 4;

    for (size_t i = 0; i < frame->leds.size(); ++i) {
        if (!is_valid_pad(model, i)) continue;

        if (current_chunk.size() + bytes_per_pad > MAX_SYSEX_SIZE) {
            current_chunk.push_back(EOX);
            all_chunks.push_back(current_chunk);
            start_new_sysex_chunk(current_chunk, model);
        }

        const auto& led = frame->leds[i];

        if (is_new_format) {
            current_chunk.push_back(CMD_LP_MK3_RGB_PREFIX);
            current_chunk.push_back(static_cast<uint8_t>(i));
            current_chunk.push_back(led.r / divisor);
            current_chunk.push_back(led.g / divisor);
            current_chunk.push_back(led.b / divisor);
        } else {
            current_chunk.push_back(static_cast<uint8_t>(i));
            current_chunk.push_back(led.r / divisor);
            current_chunk.push_back(led.g / divisor);
            current_chunk.push_back(led.b / divisor);
        }
    }

    if (current_chunk.size() > get_header_for_model(model).size() + 1) {
        current_chunk.push_back(EOX);
        all_chunks.push_back(current_chunk);
    }

    return all_chunks;
}

void lp_translator::start_new_sysex_chunk(std::vector<uint8_t>& chunk, e_lp_model model) {
    chunk.clear();
    auto header = get_header_for_model(model);
    chunk.insert(chunk.end(), header.begin(), header.end());

    switch (model) {
        case e_lp_model::LAUNCHPAD_PRO_MK3:
        case e_lp_model::LAUNCHPAD_X:
        case e_lp_model::LAUNCHPAD_MK3_MINI:
            chunk.push_back(CMD_LP_MK3_RGB_GLOBAL);
            break;
        case e_lp_model::LAUNCHPAD_MK2:
        case e_lp_model::LAUNCHPAD_PRO:
            chunk.push_back(CMD_LP_MK2_RGB);
            break;
    }
}