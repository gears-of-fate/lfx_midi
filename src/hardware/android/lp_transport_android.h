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
#include "../IMidiTransport.h"
#include <amidi/AMidi.h>
#include <vector>
#include <thread>
#include <atomic>
#include <string>

/**
 * @class lp_transport_android
 * @brief Android-specific implementation of the IMidiTransport interface.
 *
 * This class uses the Android NDK's AMidi library to communicate with
 * Launchpad devices. It uses a dedicated thread for non-blocking polling
 * of MIDI input.
 */
class lp_transport_android : public IMidiTransport {
public:
    lp_transport_android(std::string type, void* midi_in, void* midi_out);
    ~lp_transport_android() override;

    void set_callback(MidiInputCallback cb) override;
    void send_sysex(const std::vector<uint8_t>& final_sysex) override;
    void start_listening() override;

private:
    void read_thread_function();

    MidiInputCallback m_callback;
    
    void* m_midi_in;
    void* m_midi_out;

    std::string model;
    std::thread m_read_thread;
    std::atomic<bool> m_is_running{false};
};