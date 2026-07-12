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
#include "lp_arena.h"
#include <atomic>

/**
 * @class lp_triple_buffer
 * @brief A lock-free triple buffer implementation for single-producer, single-consumer scenarios.
 *
 * This class manages three frame buffers (arenas) to allow a producer thread
 * (e.g., a JNI client) and a consumer thread (the rendering pipeline) to
 * exchange data without blocking or tearing.
 */
class lp_triple_buffer {
public:
    lp_triple_buffer();

    /**
     * @brief Gets a raw pointer to the current write buffer.
     * @return A pointer to the write buffer.
     */
    uint8_t* get_jni_write_pointer();

    /**
     * @brief Commits the write buffer, making it available for the consumer.
     */
    void commit_from_jni();

    /**
     * @brief Gets a pointer to the current read buffer.
     * @return A pointer to the read buffer.
     */
    const lp_arena* get_read_buffer();

    /**
     * @brief Checks if a new frame is ready for consumption.
     * @return True if a new frame is available, false otherwise.
     */
    [[nodiscard]] bool is_frame_ready() const;

private:
    lp_arena m_buffers[3];

    int m_write_index;
    int m_read_index;

    std::atomic<uint8_t> m_staging_state;
};