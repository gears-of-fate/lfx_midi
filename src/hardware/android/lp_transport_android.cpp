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

#include "lp_transport_android.h"
#include "../../utils/logger.h"
#include <chrono>
#include <utility>

lp_transport_android::lp_transport_android(std::string type, void* midi_in, void* midi_out)
    : m_callback(nullptr), m_midi_in(midi_in), m_midi_out(midi_out), model(std::move(type)) {
    LOG("lp_transport_android::lp_transport_android");
}

lp_transport_android::~lp_transport_android() {
    LOG("lp_transport_android::~lp_transport_android");
    if (m_is_running.exchange(false)) {
        if (m_read_thread.joinable()) {
            m_read_thread.join();
        }
    }
}

void lp_transport_android::set_callback(MidiInputCallback cb) {
    LOG("lp_transport_android::set_callback");
    m_callback = std::move(cb);
}

void lp_transport_android::start_listening() {
    LOG("lp_transport_android::start_listening");
    if (m_is_running.exchange(true)) return;

    m_read_thread = std::thread(&lp_transport_android::read_thread_function, this);
}

void lp_transport_android::read_thread_function() {
    LOG("lp_transport_android::read_thread - Started");
    if (!m_midi_in) return;

    auto* device_output_port = static_cast<AMidiOutputPort*>(m_midi_in);
    uint8_t buffer[128];
    int32_t opcode;

    while (m_is_running) {
        const ssize_t num_bytes_received = AMidiOutputPort_receive(device_output_port, &opcode, buffer, sizeof(buffer), nullptr, nullptr);

        if (num_bytes_received > 0 && m_callback) {
            for (ssize_t i = 0; i < num_bytes_received; ) {
                int status = buffer[i];
                int command = status & 0xF0;

                if ((command == 0x90 || command == 0xB0) && i + 2 < num_bytes_received) {
                    int note = buffer[i + 1];
                    int velocity = buffer[i + 2];
                    if (velocity > 0) {
                        uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now().time_since_epoch()
                        ).count();
                        m_callback(model, note, timestamp);
                    }
                    i += 3;
                } else {
                    i++;
                }
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    LOG("lp_transport_android::read_thread - Terminated");
}

void lp_transport_android::send_sysex(const std::vector<uint8_t>& final_sysex) {
    if (!m_midi_out || final_sysex.empty()) return;

    auto* device_input_port = static_cast<AMidiInputPort*>(m_midi_out);

    if (ssize_t result = AMidiInputPort_send(device_input_port, final_sysex.data(), final_sysex.size()); result < 0) {
        LOG("lp_transport_android::send_sysex - Send failed with code: " << result);
    }
}