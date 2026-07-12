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

#include "lp_discovery.h"
#include "../utils/logger.h"
#include <ranges>

bool lp_discovery::on_device_connected(const lp_device_info& info, std::unique_ptr<IMidiTransport> transport) {
    LOG("lp_discovery::on_device_connected");
    if (!transport) {
        return false;
    }

    on_device_disconnected(info.serial_number);

    std::unique_lock lock(m_mutex);

    auto new_instance = std::make_unique<lp_instance>(info, std::move(transport));
    if (!new_instance) {
        LOG("ERROR: Failed to allocate a new instance for " << info.serial_number);
        return false;
    }

    m_direct_buffers[info.serial_number] = new_instance->get_triple_buffer();
    new_instance->start();
    m_instances[info.serial_number] = std::move(new_instance);
    return true;
}

bool lp_discovery::on_device_disconnected(const std::string& serial_number) {
    LOG("lp_discovery::on_device_disconnected");
    {
        std::unique_lock lock(m_mutex);
        auto it = m_instances.find(serial_number);
        if (it != m_instances.end()) {
            std::unique_ptr<lp_instance> instance_to_destroy = std::move(it->second);
            m_instances.erase(it);
            m_direct_buffers.erase(serial_number);
        } else {
            return false;
        }
    }
    return true;
}

lp_triple_buffer* lp_discovery::get_fast_buffer(const std::string& serial_number) {
    std::shared_lock lock(m_mutex);
    auto it = m_direct_buffers.find(serial_number);
    if (it != m_direct_buffers.end()) {
        return it->second;
    }
    return nullptr;
}

bool lp_discovery::pause_specific_instance(const std::string& serial_number) {
    LOG("lp_discovery::pause_specific_instance");
    std::shared_lock lock(m_mutex);
    auto it = m_instances.find(serial_number);
    if (it != m_instances.end()) {
        it->second->pause();
        return true;
    }
    return false;
}

bool lp_discovery::resume_specific_instance(const std::string& serial_number) {
    LOG("lp_discovery::resume_specific_instance");
    std::shared_lock lock(m_mutex);
    auto it = m_instances.find(serial_number);
    if (it != m_instances.end()) {
        it->second->resume();
        return true;
    }
    return false;
}

void lp_discovery::pause_all_instances() {
    LOG("lp_discovery::pause_all_instances");
    std::shared_lock lock(m_mutex);
    for (auto &val: m_instances | std::views::values) {
        val->pause();
    }
}

void lp_discovery::resume_all_instances() {
    LOG("lp_discovery::resume_all_instances");
    std::shared_lock lock(m_mutex);
    for (auto &val: m_instances | std::views::values) {
        val->resume();
    }
}