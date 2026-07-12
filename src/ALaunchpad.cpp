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

#include "lfx_midi/ALaunchpad.h"
#include "core/lp_discovery.h"
#include "utils/lp_transport_factory.h"
#include "utils/logger.h"
#include <utility>

static std::unique_ptr<lp_discovery> g_discovery = nullptr;
static MidiInputCallback g_callback = nullptr;

bool ALaunchpad_h::init_system(MidiInputCallback cb) {
    LOG("ALaunchpad_h::init_system");
    if (g_discovery) return false;
    g_callback = std::move(cb);
    g_discovery = std::make_unique<lp_discovery>();
    return true;
}

void ALaunchpad_h::shutdown_system() {
    LOG("ALaunchpad_h::shutdown_system");
    if (g_discovery) {
        g_discovery->pause_all_instances();
        g_discovery.reset();
    }
    g_callback = nullptr;
}

void ALaunchpad_h::pause_system() {
    LOG("ALaunchpad_h::pause_system");
    pause_all_device();
}

void ALaunchpad_h::resume_system() {
    LOG("ALaunchpad_h::resume_system");
    resume_all_device();
}

bool ALaunchpad_h::pass_device_info(const lp_device_info& info) {
    LOG("ALaunchpad_h::pass_device_info");
    if (!g_discovery) {
        LOG("ERROR: Attempting to pass a device but the system is not initialized.");
        return false;
    }

    auto transport = lp_transport_factory::create_transport(info);
    if (!transport) {
        LOG("ERROR: Failed to create transport for device " << info.serial_number);
        return false;
    }

    transport->set_callback(g_callback);
    transport->start_listening();

    return g_discovery->on_device_connected(info, std::move(transport));
}

bool ALaunchpad_h::device_disconnected(const std::string& serial_number) {
    LOG("ALaunchpad_h::device_disconnected");
    if (!g_discovery) return false;

    if (!g_discovery->on_device_disconnected(serial_number)) {
        LOG("WARNING: Attempt to disconnect a device that was not found: " << serial_number);
        return false;
    }
    return true;
}

bool ALaunchpad_h::pause_device(const std::string& serial_number) {
    LOG("ALaunchpad_h::pause_device");
    if (!g_discovery) return false;

    if (!g_discovery->pause_specific_instance(serial_number)) {
        LOG("WARNING: Attempt to pause a device that was not found: " << serial_number);
        return false;
    }
    return true;
}

bool ALaunchpad_h::resume_device(const std::string& serial_number) {
    LOG("ALaunchpad_h::resume_device");
    if (!g_discovery) return false;

    if (!g_discovery->resume_specific_instance(serial_number)) {
        LOG("WARNING: Attempt to resume a device that was not found: " << serial_number);
        return false;
    }
    return true;
}

void ALaunchpad_h::pause_all_device() {
    LOG("ALaunchpad_h::pause_all_device");
    if (g_discovery) g_discovery->pause_all_instances();
}

void ALaunchpad_h::resume_all_device() {
    LOG("ALaunchpad_h::resume_all_device");
    if (g_discovery) g_discovery->resume_all_instances();
}

uint8_t* ALaunchpad_h::get_jni_write_pointer(const std::string& serial_number) {
    if (!g_discovery) return nullptr;
    if (const auto buffer = g_discovery->get_fast_buffer(serial_number)) {
        return buffer->get_jni_write_pointer();
    }
    return nullptr;
}

void ALaunchpad_h::commit_frame(const std::string& serial_number) {
    if (!g_discovery) return;
    if (const auto buffer = g_discovery->get_fast_buffer(serial_number)) {
        buffer->commit_from_jni();
    }
}