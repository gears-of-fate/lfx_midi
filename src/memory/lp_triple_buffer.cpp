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

#include "lp_triple_buffer.h"
#include "../utils/logger.h"

lp_triple_buffer::lp_triple_buffer() 
    : m_write_index(0), m_read_index(2), m_staging_state(1) { 
    LOG("lp_triple_buffer::lp_triple_buffer");
}

uint8_t* lp_triple_buffer::get_jni_write_pointer() {
    return reinterpret_cast<uint8_t*>(&m_buffers[m_write_index]);
}

void lp_triple_buffer::commit_from_jni() {
    uint8_t current_state = m_staging_state.load(std::memory_order_relaxed);
    uint8_t new_state;
    do {
        new_state = static_cast<uint8_t>((m_write_index & 0x03) | 0x04);
    } while (!m_staging_state.compare_exchange_weak(current_state, new_state, std::memory_order_acq_rel, std::memory_order_relaxed));
    
    m_write_index = current_state & 0x03;
}

const lp_arena* lp_triple_buffer::get_read_buffer() {
    uint8_t current_state = m_staging_state.load(std::memory_order_acquire);
    
    if (current_state & 0x04) {
        uint8_t new_state;
        do {
            new_state = static_cast<uint8_t>(m_read_index & 0x03);
        } while (!m_staging_state.compare_exchange_weak(current_state, new_state, std::memory_order_acq_rel, std::memory_order_relaxed));
        
        m_read_index = current_state & 0x03;
    }
    
    return &m_buffers[m_read_index];
}

bool lp_triple_buffer::is_frame_ready() const {
    return (m_staging_state.load(std::memory_order_relaxed) & 0x04) != 0;
}