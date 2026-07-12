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

#pragma once
#include <vector>
#include <cstdint>
#include "memory/lp_arena.h"
#include "lfx_midi/lp_device_info.h"

/**
 * @class lp_translator
 * @brief A static utility class for translating frame data into MIDI SysEx messages.
 */
class lp_translator {
public:
    /**
     * @brief Gets the SysEx message to enable Programmer Mode on a device.
     * @param model The model of the Launchpad.
     * @return A vector of bytes representing the SysEx message.
     */
    static std::vector<uint8_t> get_programmer_mode_sysex(e_lp_model model);

    /**
     * @brief Gets the SysEx message to disable Programmer Mode (return to Live Mode).
     * @param model The model of the Launchpad.
     * @return A vector of bytes representing the SysEx message.
     */
    static std::vector<uint8_t> get_live_mode_sysex(e_lp_model model);

    /**
     * @brief Builds one or more SysEx message chunks from a frame arena.
     * @param model The model of the Launchpad.
     * @param frame A pointer to the frame data to translate.
     * @return A vector of byte vectors, where each inner vector is a complete SysEx message chunk.
     */
    static std::vector<std::vector<uint8_t>> build_frame_sysex(e_lp_model model, const lp_arena* frame);

    /**
     * @brief Checks if a given pad index is valid for a specific Launchpad model.
     * @param model The model of the Launchpad.
     * @param pad_index The index of the pad to check.
     * @return True if the pad is valid, false otherwise.
     */
    static bool is_valid_pad(e_lp_model model, int pad_index);

private:
    static std::vector<uint8_t> get_header_for_model(e_lp_model model);
    static void start_new_sysex_chunk(std::vector<uint8_t>& chunk, e_lp_model model);
};