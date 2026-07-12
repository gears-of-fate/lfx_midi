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

#include "lp_instance.h"
#include "utils/lp_translator.h"
#include "../utils/logger.h"
#include <chrono>
#include <vector>

lp_instance::lp_instance(const lp_device_info& info, std::unique_ptr<IMidiTransport> transport)
    : m_instance_info(info),
      m_transport(std::move(transport)), 
      m_pipeline(&m_buffer, m_transport.get(), info.model),
      m_is_running(false), 
      m_is_paused(false) {
    LOG("lp_instance::lp_instance");
}

lp_instance::~lp_instance() {
    LOG("lp_instance::~lp_instance");
    stop();
}

void lp_instance::start() {
    LOG("lp_instance::start");
    if (m_is_running.exchange(true)) return;
    
    m_is_paused = false;

    if (m_transport) {
        auto programmer_sysex = lp_translator::get_programmer_mode_sysex(m_instance_info.model);
        m_transport->send_sysex(programmer_sysex);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    m_gameloop_thread = std::thread(&lp_instance::thread_execution, this);
}

void lp_instance::stop() {
    LOG("lp_instance::stop");
    if (!m_is_running.exchange(false)) return;
    
    if (m_gameloop_thread.joinable()) {
        m_gameloop_thread.join();
    }

    if (m_transport) {
        LOG("lp_instance::stop - Clearing LEDs...");
        lp_arena black_frame = {};
        auto clear_sysex_chunks = lp_translator::build_frame_sysex(m_instance_info.model, &black_frame);
        for (const auto& chunk : clear_sysex_chunks) {
            m_transport->send_sysex(chunk);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        LOG("lp_instance::stop - Exiting Programmer Mode...");
        auto live_sysex = lp_translator::get_live_mode_sysex(m_instance_info.model);
        m_transport->send_sysex(live_sysex);
    }
}

void lp_instance::pause() {
    LOG("lp_instance::pause");
    m_is_paused = true;
    m_pipeline.request_reset();
    LOG("lp_instance::pause - Clearing LEDs...");
    lp_arena black_frame = {};
    auto clear_sysex_chunks = lp_translator::build_frame_sysex(m_instance_info.model, &black_frame);
    for (const auto& chunk : clear_sysex_chunks) {
        m_transport->send_sysex(chunk);
    }
}

void lp_instance::resume() {
    LOG("lp_instance::resume");
    m_is_paused = false;
}

lp_triple_buffer* lp_instance::get_triple_buffer() {
    return &m_buffer;
}

void lp_instance::thread_execution() {
    using namespace std::chrono_literals;

    bool is_new_gen = m_instance_info.model == e_lp_model::LAUNCHPAD_PRO_MK3 ||
                      m_instance_info.model == e_lp_model::LAUNCHPAD_X ||
                      m_instance_info.model == e_lp_model::LAUNCHPAD_MK3_MINI;
    std::chrono::milliseconds target_frame_time = is_new_gen ? 16ms : 50ms; // ~60 FPS vs ~20 FPS

    while (m_is_running) {
        auto start_time = std::chrono::steady_clock::now();

        if (!m_is_paused && m_buffer.is_frame_ready()) {
            m_pipeline.process_frame();
        }

        auto end_time = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        if (elapsed < target_frame_time) {
            std::this_thread::sleep_for(target_frame_time - elapsed);
        }
    }
}