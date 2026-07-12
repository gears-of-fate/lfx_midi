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
#include <windows.h>
#include <mmsystem.h>
#include <vector>
#include <cstdint>
#include <map>

/**
 * @class lp_transport_win
 * @brief Windows-specific implementation of the IMidiTransport interface.
 *
 * This class uses the Windows Multimedia API (winmm) to communicate with
 * Launchpad devices. It uses a static system callback for MIDI input.
 */
class lp_transport_win : public IMidiTransport {
public:
    lp_transport_win(std::string type, void* midi_in, void* midi_out);
    ~lp_transport_win() override;

    void set_callback(MidiInputCallback cb) override;
    void send_sysex(const std::vector<uint8_t>& final_sysex) override;
    void start_listening() override;

    static void CALLBACK midi_in_proc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);

private:
    MidiInputCallback m_callback;
    
    HMIDIIN m_midi_in;
    HMIDIOUT m_midi_out;

    std::string model;

    static std::map<HMIDIIN, lp_transport_win*> s_midi_in_map;
};