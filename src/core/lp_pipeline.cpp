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

#include "lp_pipeline.h"
#include "utils/lp_translator.h"
#include "utils/logger.h"

lp_pipeline::lp_pipeline(lp_triple_buffer* buffer_sys, IMidiTransport* transport, e_lp_model model)
    : m_buffer_sys(buffer_sys), m_transport(transport), m_model(model) {
    LOG("lp_pipeline::lp_pipeline");
}

void lp_pipeline::process_frame() {
    if (!m_buffer_sys || !m_transport) return;

    if (m_needs_reset.exchange(false, std::memory_order_relaxed)) {
        m_dirty_checker.reset();
    }

    const lp_arena* frame = m_buffer_sys->get_read_buffer();
    if (!frame) return;

    const lp_arena* frame_to_process = m_dirty_checker.filter_arena(frame);

    if (!frame_to_process) {
        return;
    }

    std::vector<std::vector<uint8_t>> sysex_chunks = lp_translator::build_frame_sysex(m_model, frame_to_process);

    for (const auto& chunk : sysex_chunks) {
        if (!chunk.empty()) {
            m_transport->send_sysex(chunk);
        }
    }
}

void lp_pipeline::request_reset() {
    LOG("lp_pipeline::request_reset");
    m_needs_reset.store(true, std::memory_order_relaxed);
}