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
#include <functional>
#include <string>

using MidiInputCallback = std::function<void(std::string type, int pad_id, uint64_t timestamp)>;

/**
 * @class IMidiTransport
 * @brief An interface for platform-specific MIDI communication.
 *
 * This abstract class defines the contract for sending and receiving MIDI
 * data on a specific platform (e.g., Windows, Android).
 */
class IMidiTransport {
public:
    virtual ~IMidiTransport() = default;

    /**
     * @brief Sets the callback function for handling incoming MIDI events.
     * @param cb The function to be called when MIDI data is received.
     */
    virtual void set_callback(MidiInputCallback cb) = 0;

    /**
     * @brief Sends a raw SysEx message to the device.
     * @param data The vector of bytes representing the SysEx message.
     */
    virtual void send_sysex(const std::vector<uint8_t>& data) = 0;

    /**
     * @brief Starts listening for incoming MIDI messages.
     *
     * This might involve starting a new thread or registering a system callback,
     * depending on the platform implementation.
     */
    virtual void start_listening() = 0;
};