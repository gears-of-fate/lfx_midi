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

#include <iostream>
#include <windows.h>
#include <mmsystem.h>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <cmath>
#include <optional>
#include "lfx_midi/ALaunchpad.h"
#include "../src/utils/logger.h"
#include "../src/hardware/windows/lp_transport_win.h"

struct DiscoveredDevice {
    std::string name;
    std::string serial;
    int out_port_id = -1;
    int in_port_id = -1;
    e_lp_model model=e_lp_model::LAUNCHPAD_MK2;
    HMIDIIN hMidiIn = nullptr;
    HMIDIOUT hMidiOut = nullptr;
};

struct lp_led { uint8_t r, g, b; };
struct lp_arena { lp_led leds[128]; };

void my_midi_callback(const std::string& type, int pad_id, uint64_t timestamp) {
    LOG("MIDI Callback: Received input from: " << type << "  from PAD: " << pad_id << " (timestamp: " << timestamp << ")");
}

std::optional<e_lp_model> get_model_from_output_name(const std::string& name) {
    if (name == "LPProMK3 MIDI") return e_lp_model::LAUNCHPAD_PRO_MK3;
    if (name == "MIDIOUT3 (LPProMK3 MIDI)") return e_lp_model::LAUNCHPAD_PRO_MK3;
    if (name == "MIDIOUT3 (Launchpad Pro)") return e_lp_model::LAUNCHPAD_PRO;
    if (name == "LPX MIDI") return e_lp_model::LAUNCHPAD_X;
    if (name == "LPMiniMK3 MIDI") return e_lp_model::LAUNCHPAD_MK3_MINI;
    if (name == "Launchpad MK2") return e_lp_model::LAUNCHPAD_MK2;
    return std::nullopt;
}

int main() {
    LOG("--- LFX MIDI Stress Test (Windows) ---");

    std::vector<DiscoveredDevice> devices;
    unsigned int numOutDevs = midiOutGetNumDevs();
    unsigned int numInDevs = midiInGetNumDevs();

    LOG("Scanning MIDI ports for devices...");

    for (int i = 0; i < numOutDevs; i++) {
        MIDIOUTCAPSA out_caps;
        if (midiOutGetDevCapsA(i, &out_caps, sizeof(out_caps)) != MMSYSERR_NOERROR) continue;
        
        std::string out_name = out_caps.szPname;
        LOG("  - Checking OUT port [" << i << "]: '" << out_name << "'");
        auto model_opt = get_model_from_output_name(out_name);

        if (!model_opt) continue;

        bool model_already_found = false;
        for(const auto& d : devices) {
            if (d.model == *model_opt) {
                model_already_found = true;
                break;
            }
        }
        if(model_already_found) continue;

        DiscoveredDevice dev;
        dev.model = *model_opt;
        dev.name = out_name;
        dev.out_port_id = i;
        dev.serial = "LFX-SN-" + std::to_string(i);

        for (int j = 0; j < numInDevs; j++) {
            MIDIINCAPSA in_caps;
            if (midiInGetDevCapsA(j, &in_caps, sizeof(in_caps)) != MMSYSERR_NOERROR) continue;
            std::string in_name = in_caps.szPname;
            LOG("    - Checking IN port [" << j << "]: '" << in_name << "'");

            bool match = false;
            switch(dev.model) {
                case e_lp_model::LAUNCHPAD_PRO_MK3:
                    if (in_name == "LPProMK3 MIDI") match = true;
                    break;
                case e_lp_model::LAUNCHPAD_PRO:
                    if (in_name == "MIDIIN3 (Launchpad Pro)") match = true;
                    break;
                default:
                    if (in_name == out_name) match = true;
                    break;
            }

            if (match) {
                dev.in_port_id = j;
                break;
            }
        }

        if (dev.in_port_id != -1) {
            if (dev.model == e_lp_model::LAUNCHPAD_PRO_MK3) {
                 for (int k = 0; k < numOutDevs; k++) {
                    MIDIOUTCAPSA daw_caps;
                    midiOutGetDevCapsA(k, &daw_caps, sizeof(daw_caps));
                    if (std::string(daw_caps.szPname).find("Launchpad Pro MK3 DAW") != std::string::npos) {
                        dev.out_port_id = k;
                        break;
                    }
                }
            }
            devices.push_back(dev);
            LOG("    --> FOUND and MATCHED: '" << dev.name << "' (IN: " << dev.in_port_id << ", OUT: " << dev.out_port_id << ")");
        }
    }

    if (devices.empty()) {
        LOG("No Launchpad devices found. Exiting.");
        return 1;
    }

    LOG("\nInitializing LFX_MIDI engine...");
    ALaunchpad_h::init_system(my_midi_callback);

    for (auto& dev : devices) {
        if (midiOutOpen(&dev.hMidiOut, dev.out_port_id, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR ||
            midiInOpen(&dev.hMidiIn, dev.in_port_id, reinterpret_cast<DWORD_PTR>(lp_transport_win::midi_in_proc), 0, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
            LOG("ERROR: Failed to open MIDI ports for " << dev.name << ". Device ignored.");
            if(dev.hMidiOut) midiOutClose(dev.hMidiOut);
            dev.hMidiOut = nullptr;
            continue;
        }

        lp_device_info info;
        info.device_id = dev.out_port_id;
        info.model = dev.model;
        info.serial_number = dev.serial;
        info.midi_in = dev.hMidiIn;
        info.midi_out = dev.hMidiOut;

        LOG("----------------------------------------------------");
        LOG("Passing device to library...");
        LOG("  - Detected Name: " << dev.name);
        LOG("  - Generated Serial: " << info.serial_number);
        LOG("  - Model Enum: " << static_cast<int>(info.model));
        LOG("  - IN Port ID: " << dev.in_port_id);
        LOG("  - OUT Port ID: " << dev.out_port_id);
        LOG("----------------------------------------------------");

        if (!ALaunchpad_h::pass_device_info(info)) {
            LOG("  - ERROR: Failed to pass device " << dev.serial);
        }
    }

    LOG("\nSending animation for 20 seconds...");
    auto start_time = std::chrono::steady_clock::now();
    bool is_paused = false;
    bool is_resumed = false;

    while (std::chrono::steady_clock::now() - start_time < std::chrono::seconds(20)) {
        using namespace std::chrono;
        auto elapsed = steady_clock::now() - start_time;
        double time = duration_cast<duration<double>>(elapsed).count();

        if (time > 5.0 && !is_paused) {
            LOG("\n--- PAUSING ALL DEVICES ---\n");
            ALaunchpad_h::pause_all_device();
            is_paused = true;
        }
        if (time > 12.0 && !is_resumed) {
            LOG("\n--- RESUMING ALL DEVICES ---\n");
            ALaunchpad_h::resume_all_device();
            is_resumed = true;
        }

        auto r = static_cast<uint8_t>((sin(time * 2.0) + 1.0) / 2.0 * 127);
        auto g = static_cast<uint8_t>((sin(time * 2.0 + 2.0) + 1.0) / 2.0 * 127);
        auto b = static_cast<uint8_t>((sin(time * 2.0 + 4.0) + 1.0) / 2.0 * 127);

        for (const auto& dev : devices) {
            if (!dev.hMidiOut) continue;
            uint8_t* write_ptr = ALaunchpad_h::get_jni_write_pointer(dev.serial);
            if (!write_ptr) continue;
            auto* arena = reinterpret_cast<lp_arena*>(write_ptr);
            for (int i = 0; i < 112; ++i) {
                arena->leds[i] = {r, g, b};
            }
            ALaunchpad_h::commit_frame(dev.serial);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    LOG("\nShutting down...");
    for (const auto& dev : devices) {
        if (dev.hMidiOut) {
            if (!ALaunchpad_h::device_disconnected(dev.serial)) {
                LOG("  - ERROR: Failed to disconnect device " << dev.serial);
            }
        }
    }
    ALaunchpad_h::shutdown_system();

    for (const auto& dev : devices) {
        if (dev.hMidiOut) midiOutClose(dev.hMidiOut);
        if (dev.hMidiIn) midiInClose(dev.hMidiIn);
    }

    LOG("Test completed successfully!");
    return 0;
}