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
#include <string>

/**
 * @enum e_lp_model
 * @brief Represents all supported Launchpad models.
 */
enum class e_lp_model {
    LAUNCHPAD_PRO_MK3,
    LAUNCHPAD_X,
    LAUNCHPAD_MK3_MINI,
    LAUNCHPAD_MK2,
    LAUNCHPAD_PRO
};

/**
 * @struct lp_device_info
 * @brief A plain data structure holding all necessary information to identify and communicate with a Launchpad device.
 */
struct lp_device_info {
    /// @brief The platform-specific device ID, primarily used for opening MIDI ports.
    int device_id;
    /// @brief The specific model of the Launchpad.
    e_lp_model model;
    /// @brief A unique serial number to identify the device instance.
    std::string serial_number;
    /// @brief A platform-specific handle to the MIDI input port.
    void* midi_in;
    /// @brief A platform-specific handle to the MIDI output port.
    void* midi_out;
};