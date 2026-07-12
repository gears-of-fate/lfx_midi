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
#include "memory/lp_arena.h"

/**
 * @class lp_dirty_check
 * @brief Optimizes rendering by filtering out unchanged frames.
 *
 * This class compares the current frame (lp_arena) with the previously sent
 * frame to determine if an update is necessary, preventing redundant MIDI messages.
 */
class lp_dirty_check {
public:
    lp_dirty_check();

    /**
     * @brief Filters the current frame against the last sent frame.
     * @param current_frame A pointer to the current frame to be processed.
     * @return A pointer to current_frame if changes are detected or if it's the first frame; otherwise, nullptr.
     */
    const lp_arena* filter_arena(const lp_arena* current_frame);

    /// @brief Resets the internal state, forcing the next frame to be sent entirely.
    void reset();

private:
    lp_arena m_last_arena;
    bool m_is_first_arena;
};