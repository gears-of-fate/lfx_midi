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
#include <cstdint>
#include <functional>
#include "lp_device_info.h"

/**
 * @brief Callback function type for handling MIDI input events.
 * @param type The type of the MIDI event (e.g., "NoteOn").
 * @param pad_id The ID of the pad that was pressed.
 * @param timestamp The timestamp of the event.
 */
using MidiInputCallback = std::function<void(std::string type, int pad_id, uint64_t timestamp)>;

/**
 * @class ALaunchpad_h
 * @brief A static facade class for global access to the LFX_MIDI system.
 *
 * This class provides a simple, static interface to initialize, manage, and
 * shut down the MIDI system and all connected Launchpad devices.
 */
class ALaunchpad_h {
public:
    /**
     * @brief Initializes the global MIDI system.
     * @param cb The callback function to be invoked for MIDI input events.
     * @return True if initialization was successful, false if it was already initialized.
     */
    static bool init_system(MidiInputCallback cb);
    
    /**
     * @brief Shuts down the entire system, releasing all resources.
     */
    static void shutdown_system();
    
    /**
     * @brief Pauses the rendering threads of all connected devices.
     */
    static void pause_system();

    /**
     * @brief Resumes the rendering threads of all connected devices.
     */
    static void resume_system();
    
    /**
     * @brief Registers a new Launchpad device with the system.
     * @param info A struct containing all necessary information about the device.
     * @return True if the device was successfully connected, false otherwise.
     */
    [[nodiscard]] static bool pass_device_info(const lp_device_info& info);

    /**
     * @brief Unregisters a device from the system using its serial number.
     * @param serial_number The unique serial number of the device to disconnect.
     * @return True if the device was found and disconnected, false otherwise.
     */
    [[nodiscard]] static bool device_disconnected(const std::string& serial_number);
    
    /**
     * @brief Pauses the rendering thread of a specific device.
     * @param serial_number The serial number of the device to pause.
     * @return True if the device was found and paused, false otherwise.
     */
    [[nodiscard]] static bool pause_device(const std::string& serial_number);

    /**
     * @brief Resumes the rendering thread of a specific device.
     * @param serial_number The serial number of the device to resume.
     * @return True if the device was found and resumed, false otherwise.
     */
    [[nodiscard]] static bool resume_device(const std::string& serial_number);
    
    /**
     * @brief Pauses all currently connected devices.
     */
    static void pause_all_device();

    /**
     * @brief Resumes all currently paused devices.
     */
    static void resume_all_device();

    /**
     * @brief Gets a direct pointer to the write buffer for a specific device.
     *
     * This is intended for high-performance updates (e.g., from JNI) where the
     * client writes directly into the library's memory.
     * @param serial_number The serial number of the target device.
     * @return A pointer to the write buffer, or nullptr if the device is not found.
     */
    static uint8_t* get_jni_write_pointer(const std::string& serial_number);

    /**
     * @brief Commits the changes made to the write buffer obtained via get_jni_write_pointer.
     * @param serial_number The serial number of the target device.
     */
    static void commit_frame(const std::string& serial_number);
};