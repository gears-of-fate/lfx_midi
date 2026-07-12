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
#include <unordered_map>
#include <string>
#include <memory>
#include <shared_mutex>
#include "lfx_midi/lp_device_info.h"
#include "hardware/IMidiTransport.h"
#include "lp_instance.h"

class lp_triple_buffer;

/**
 * @class lp_discovery
 * @brief Manages the lifecycle of all connected Launchpad instances.
 *
 * This class acts as a central registry for all active devices, mapping
 * serial numbers to their corresponding lp_instance objects. It is responsible
 * for creating, destroying, and providing access to these instances in a
 * thread-safe manner.
 */
class lp_discovery {
public:
    /**
     * @brief Handles the connection of a new device.
     * @param info The device information.
     * @param transport The MIDI transport for the device.
     * @return True on success, false on failure.
     */
    bool on_device_connected(const lp_device_info& info, std::unique_ptr<IMidiTransport> transport);
    
    /**
     * @brief Handles the disconnection of a device.
     * @param serial_number The serial number of the device to disconnect.
     * @return True if the device was found and disconnected, false otherwise.
     */
    bool on_device_disconnected(const std::string& serial_number);
    
    /**
     * @brief Provides fast, O(1) access to a device's triple buffer.
     * @param serial_number The serial number of the target device.
     * @return A pointer to the buffer, or nullptr if not found.
     */
    lp_triple_buffer* get_fast_buffer(const std::string& serial_number);

    /**
     * @brief Pauses a specific device instance.
     * @param serial_number The serial number of the device to pause.
     * @return True if the device was found and paused, false otherwise.
     */
    bool pause_specific_instance(const std::string& serial_number);

    /**
     * @brief Resumes a specific device instance.
     * @param serial_number The serial number of the device to resume.
     * @return True if the device was found and resumed, false otherwise.
     */
    bool resume_specific_instance(const std::string& serial_number);
    
    /**
     * @brief Pauses all active device instances.
     */
    void pause_all_instances();

    /**
     * @brief Resumes all paused device instances.
     */
    void resume_all_instances();

private:
    /// @brief Map of serial numbers to their corresponding device instances.
    std::unordered_map<std::string, std::unique_ptr<lp_instance>> m_instances;
    /// @brief A cache for direct O(1) access to device buffers.
    std::unordered_map<std::string, lp_triple_buffer*> m_direct_buffers;
    /// @brief A mutex to protect concurrent access to the maps.
    mutable std::shared_mutex m_mutex;
};