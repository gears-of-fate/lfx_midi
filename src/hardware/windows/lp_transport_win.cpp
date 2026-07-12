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

#include "lp_transport_win.h"
#include "../../utils/logger.h"
#include <iomanip>
#include <utility>
#include <Windows.h>

std::map<HMIDIIN, lp_transport_win*> lp_transport_win::s_midi_in_map;

lp_transport_win::lp_transport_win(std::string type, void* midi_in, void* midi_out)
    : m_callback(nullptr), 
      m_midi_in(static_cast<HMIDIIN>(midi_in)), 
      m_midi_out(static_cast<HMIDIOUT>(midi_out)),
      model(std::move(type)) {
    LOG("lp_transport_win::lp_transport_win");
    if (m_midi_in) {
        s_midi_in_map[m_midi_in] = this;
    }
}

lp_transport_win::~lp_transport_win() {
    LOG("lp_transport_win::~lp_transport_win");
    if (m_midi_in) {
        midiInStop(m_midi_in);
        midiInReset(m_midi_in);
        s_midi_in_map.erase(m_midi_in);
    }
}

void lp_transport_win::set_callback(MidiInputCallback cb) {
    LOG("lp_transport_win::set_callback");
    m_callback = std::move(cb);
}

void lp_transport_win::start_listening() {
    LOG("lp_transport_win::start_listening");
    if (m_midi_in) {
        if (midiInStart(m_midi_in) != MMSYSERR_NOERROR) {
            LOG("ERROR: Failed to start MIDI IN listening.");
        }
    }
}

void CALLBACK lp_transport_win::midi_in_proc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
    if (wMsg == MIM_DATA) {
        auto it = s_midi_in_map.find(hMidiIn);
        if (it != s_midi_in_map.end()) {
            lp_transport_win* instance = it->second;
            if (instance && instance->m_callback) {
                int status = (dwParam1 & 0xFF);
                int note = (dwParam1 >> 8) & 0xFF;
                int velocity = (dwParam1 >> 16) & 0xFF;
                int command = status & 0xF0;

                if ((command == 0x90 || command == 0xB0) && velocity > 0) {
                    instance->m_callback(instance->model, note, dwParam2);
                }
            }
        }
    }
}

void lp_transport_win::send_sysex(const std::vector<uint8_t>& final_sysex) {
    if (!m_midi_out || final_sysex.empty()) return;

    MIDIHDR hdr;
    ZeroMemory(&hdr, sizeof(MIDIHDR));
    hdr.lpData = (LPSTR)final_sysex.data();
    hdr.dwBufferLength = static_cast<DWORD>(final_sysex.size());
    hdr.dwBytesRecorded = static_cast<DWORD>(final_sysex.size());

    if (midiOutPrepareHeader(m_midi_out, &hdr, sizeof(MIDIHDR)) == MMSYSERR_NOERROR) {
        if (midiOutLongMsg(m_midi_out, &hdr, sizeof(MIDIHDR)) == MMSYSERR_NOERROR) {
            while (!(hdr.dwFlags & MHDR_DONE)) {
                SwitchToThread();
            }
        }
        midiOutUnprepareHeader(m_midi_out, &hdr, sizeof(MIDIHDR));
    }
}